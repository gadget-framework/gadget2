#include "renewal.h"
#include "errorhandler.h"
#include "readfunc.h"
#include "print.h"
#include "keeper.h"
#include "readword.h"
#include "gadget.h"

//It has to be decided how to distinguish between setting the stock numbers and adding to them

RenewalData::RenewalData(CommentStream& infile, const IntVector& Areas,
  const AreaClass* const Area, const TimeClass* const TimeInfo, Keeper* const keeper)
  : LivesOnAreas(Areas), CI(0), LgrpDiv(0) {

  ErrorHandler handle;
  keeper->AddString("renewaldata");
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);

  double minlength;
  double maxlength;
  double dl;
  infile >> text;
  if (strcasecmp(text, "normaldistribution") == 0) {
    ReadOption = 1;
    readWordAndVariable(infile, "minlength", minlength);
  } else if (strcasecmp(text, "minlength") == 0) {
    ReadOption = 0;
    infile >> minlength;
  } else
    handle.Unexpected("normaldistribution or minlength", text);

  readWordAndVariable(infile, "maxlength", maxlength);
  readWordAndVariable(infile, "dl", dl);

  LgrpDiv = new LengthGroupDivision(minlength, maxlength, dl);
  if (LgrpDiv->Error())
    printLengthGroupError(minlength, maxlength, dl, "length groups for renewal data");

  //We now expect to find:
  //year, step, area, age and then the renewal data

  char c;
  int i = 0;
  int year, step, area, age, ind, no;

  infile >> year >> step >> area >> age >> ws;
  while (isdigit(infile.peek()) && !infile.eof()) {
    if (TimeInfo->IsWithinPeriod(year, step)) {

      RenewalTime.resize(1);
      RenewalTime[i] = TimeInfo->CalcSteps(year, step);

      if (!this->IsInArea(Area->InnerArea(area)))
        handle.Message("Stock undefined on area for renewal");

      RenewalArea.resize(1);
      RenewalArea[i] = Area->InnerArea(area);

      Number.resize(1, keeper);

      if (ReadOption == 0) {
        infile >> minlength >> no >> ws;

        if (!(infile >> Number[i]))
          handle.Message("Wrong format for renewalmultiplier");
        Number[i].Inform(keeper);

        //Now we read first the length distribution pattern and then
        //the corresponding mean weights. Both are assumed to be vectors of
        //length no. We read them into the indexvectors numtmpindvec and
        //weighttmpindvec, create poptmp and then keep it in Distribution.
        DoubleIndexVector* numtmpindvec =
          readIndexVector(infile, no, LgrpDiv->NoLengthGroup(minlength));
        DoubleIndexVector* weighttmpindvec =
          readIndexVector(infile, no, LgrpDiv->NoLengthGroup(minlength));
        PopInfoIndexVector poptmp(no, LgrpDiv->NoLengthGroup(minlength));

        //Check if reading the vectors succeeded
        if (numtmpindvec == 0 || weighttmpindvec == 0)
          handle.Message("Wrong format for renewal data");

        for (ind = poptmp.Mincol(); ind < poptmp.Maxcol(); ind++) {
          poptmp[ind].N = (*numtmpindvec)[ind];
          poptmp[ind].W = (*weighttmpindvec)[ind];
          //Check if any (i.e. nonzero) part of the population has zero mean weight.
          if (poptmp[ind].N > 0 && isZero(poptmp[ind].W))
            handle.Message("Zero mean weight for nonzero number in renewal data");
        }

        Distribution.resize(1, new AgeBandMatrix(age, poptmp));
        delete numtmpindvec;
        delete weighttmpindvec;

      } else if (ReadOption == 1) { //use meanlengths.
        Meanlengths.resize(1, keeper);
        Sdev.resize(1, keeper);
        Wcoeff1.resize(1, keeper);
        Wcoeff2.resize(1, keeper);

        PopInfoIndexVector poptmp(LgrpDiv->NoLengthGroups(), 0);
        Distribution.resize(1, new AgeBandMatrix(age, poptmp));
        if (!(infile >> Number[i]))
          handle.Message("Wrong format for renewalmultiplier");
        Number[i].Inform(keeper);
        if (!(infile >> Meanlengths[i]))
          handle.Message("Wrong format for renewalmeanlength");
        Meanlengths[i].Inform(keeper);
        if (!(infile >> Sdev[i]))
          handle.Message("Wrong format for renewalsdev");
        Sdev[i].Inform(keeper);
        if (!(infile >> Wcoeff1[i]))
          handle.Message("Wrong format for renewalwcoeff1");
        Wcoeff1[i].Inform(keeper);
        if (!(infile >> Wcoeff2[i]))
          handle.Message("Wrong format for renewalwcoeff2");
        Wcoeff2[i].Inform(keeper);

      } else
        handle.Message("Unknown data format for renewal data ");

      //Now we move on and check whether we find a digit or not
      infile >> ws;
      i++;

    } else { //This year and step is not required - skip rest of line
      c = infile.peek();
      while (c != '\n' && !infile.eof())
        infile.get(c);
    }

    if (isdigit(infile.peek()) && !infile.eof())
      infile >> year >> step >> area >> age >> ws;
  }
  keeper->ClearLast();
}

RenewalData::~RenewalData() {
  delete LgrpDiv;
  delete CI;
}

void RenewalData::SetCI(const LengthGroupDivision* const GivenLDiv) {
  CI = new ConversionIndex(LgrpDiv, GivenLDiv);
}

void RenewalData::Print(ofstream& outfile) const {
  outfile << "\nRenewal\n\tDistribution\n";
  int i;
  for (i = 0; i < Distribution.Size(); i++) {
    outfile << "\tTime " << RenewalTime[i] << " internal area " << RenewalArea[i]
      << " age " << Distribution[i].Minage() << " number " << Number[i] << endl;
    Printagebandm(outfile, Distribution[i]);
    outfile << "\tmean weights\n";
    PrintWeightinagebandm(outfile, Distribution[i]);
  }
}

void RenewalData::Reset() {
  int i, age, l;
  double sum, length, N, tmpSdev;

  if (ReadOption == 1) {
    for (i = 0; i < Distribution.Size(); i++) {
      age = Distribution[i].Minage();
      sum = 0.0;
      N = 0.0;
      if (isZero(Sdev[i]))
        tmpSdev = 0.0;
      else
        tmpSdev = 1.0 / (2.0 * Sdev[i] * Sdev[i]);

      for (l = Distribution[i].Minlength(age); l < Distribution[i].Maxlength(age); l++) {
        length = LgrpDiv->Meanlength(l) - Meanlengths[i];
        if (Sdev[i] > verysmall)
          N = exp(-(length * length * tmpSdev));
        else
          N = 0.0;
        Distribution[i][age][l].N = N;
        sum += N;
      }

      for (l = Distribution[i].Minlength(age); l < Distribution[i].Maxlength(age); l++) {
        length = LgrpDiv->Meanlength(l);
        if (sum > rathersmall)
          Distribution[i][age][l].N *= 10000 / sum;
        else
          Distribution[i][age][l].N = 0.0;
        Distribution[i][age][l].W = Wcoeff1[i] * pow(length, Wcoeff2[i]);
      }
    }
  }
}

void RenewalData::AddRenewal(AgeBandMatrix& Alkeys, int area, const TimeClass* const TimeInfo) {

  if (RenewalTime.Size() == 0)
    return;

  int i, timeid, renewalid;
  double RenewalNumber = 0.0;
  timeid = TimeInfo->CurrentTime();
  renewalid = -1;

  for (i = 0; i < RenewalTime.Size(); i++)
    if ((RenewalTime[i] == timeid) && (RenewalArea[i] == area)) {
      renewalid = i;
      break;
    }

  //Add renewal to stock
  if (renewalid != -1) {
    if (isZero(Number[renewalid]))
      RenewalNumber = 0.0;
    else
      RenewalNumber = Number[renewalid];

    if (RenewalNumber < 0) {
      cerr << "Error in renewal - received illegal number of recruits (" << RenewalNumber << ")\n";
      RenewalNumber = -RenewalNumber;
    }

    if (RenewalNumber > verysmall) {
      assert(Alkeys.Minage() <= Distribution[renewalid].Minage());
      AgebandmAdd(Alkeys, Distribution[renewalid], *CI, RenewalNumber);
    }
  }
}
