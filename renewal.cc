#include "renewal.h"
#include "errorhandler.h"
#include "readfunc.h"
#include "keeper.h"
#include "readword.h"
#include "gadget.h"

extern ErrorHandler handle;

RenewalData::RenewalData(CommentStream& infile, const IntVector& Areas,
  const AreaClass* const Area, const TimeClass* const TimeInfo, Keeper* const keeper)
  : LivesOnAreas(Areas), CI(0), LgrpDiv(0) {

  keeper->addString("renewaldata");
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);

  double minlength;
  double maxlength;
  double dl;
  infile >> text;
  if (strcasecmp(text, "normaldistribution") == 0) {
    readOption = 1;
    readWordAndVariable(infile, "minlength", minlength);
  } else if (strcasecmp(text, "minlength") == 0) {
    readOption = 0;
    infile >> minlength;
  } else
    handle.Unexpected("normaldistribution or minlength", text);

  readWordAndVariable(infile, "maxlength", maxlength);
  readWordAndVariable(infile, "dl", dl);

  LgrpDiv = new LengthGroupDivision(minlength, maxlength, dl);
  if (LgrpDiv->Error())
    handle.Message("Error in renewal - failed to create length group");

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

      if (readOption == 0) {
        infile >> minlength >> no >> ws;

        if (!(infile >> Number[i]))
          handle.Message("Wrong format for renewalmultiplier");
        Number[i].Inform(keeper);

        //Now we read first the length distribution pattern and then
        //the corresponding mean weights. Both are assumed to be vectors of
        //length no. We read them into the indexvectors numtmpindvec and
        //weighttmpindvec, create poptmp and then keep it in Distribution.
        ind = LgrpDiv->NoLengthGroup(minlength);
        DoubleIndexVector* numtmpindvec = new DoubleIndexVector(no, ind);
        DoubleIndexVector* weighttmpindvec = new DoubleIndexVector(no, ind);
        PopInfoIndexVector poptmp(no, ind);

        if (!readIndexVector(infile, (*numtmpindvec)))
          handle.Message("Failure in numbers of recruits");
        if (!readIndexVector(infile, (*weighttmpindvec)))
          handle.Message("Failure in weights of recruits");

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

      } else if (readOption == 1) { //use meanlengths.
        meanLength.resize(1, keeper);
        sdevLength.resize(1, keeper);
        coeff1.resize(1, keeper);
        coeff2.resize(1, keeper);

        PopInfoIndexVector poptmp(LgrpDiv->NoLengthGroups(), 0);
        Distribution.resize(1, new AgeBandMatrix(age, poptmp));
        if (!(infile >> Number[i]))
          handle.Message("Wrong format for renewalmultiplier");
        Number[i].Inform(keeper);
        if (!(infile >> meanLength[i]))
          handle.Message("Wrong format for renewalmeanlength");
        meanLength[i].Inform(keeper);
        if (!(infile >> sdevLength[i]))
          handle.Message("Wrong format for renewalsdev");
        sdevLength[i].Inform(keeper);
        if (!(infile >> coeff1[i]))
          handle.Message("Wrong format for renewalwcoeff1");
        coeff1[i].Inform(keeper);
        if (!(infile >> coeff2[i]))
          handle.Message("Wrong format for renewalwcoeff2");
        coeff2[i].Inform(keeper);

      } else
        handle.Message("Unknown data format for renewal data ");

      //Now we move on and check whether we find a digit or not
      infile >> ws;
      i++;

    } else { //This year and step is not required - skip rest of line
      infile.get(c);
      while (c != '\n' && !infile.eof())
        infile.get(c);
    }

    if (isdigit(infile.peek()) && !infile.eof())
      infile >> year >> step >> area >> age >> ws;
  }
  handle.logMessage("Read recruits data file - number of entries", RenewalTime.Size());
  keeper->clearLast();
}

RenewalData::~RenewalData() {
  delete LgrpDiv;
  delete CI;
}

void RenewalData::setCI(const LengthGroupDivision* const GivenLDiv) {
  CI = new ConversionIndex(LgrpDiv, GivenLDiv);
}

void RenewalData::Print(ofstream& outfile) const {
  outfile << "\nRenewal data\n\tDistribution\n";
  int i;
  for (i = 0; i < Distribution.Size(); i++) {
    outfile << "\tTime " << RenewalTime[i] << " internal area " << RenewalArea[i]
      << " age " << Distribution[i].minAge() << " number " << Number[i] << "\n\tNumbers\n";
    Distribution[i].printNumbers(outfile);
    outfile << "\tMean weights\n";
    Distribution[i].printWeights(outfile);
  }
}

void RenewalData::Reset() {
  int i, age, l;
  double sum, length, N, tmpVariance;

  if (readOption == 1) {
    for (i = 0; i < Distribution.Size(); i++) {
      age = Distribution[i].minAge();
      sum = 0.0;
      N = 0.0;
      if (isZero(sdevLength[i]))
        tmpVariance = 0.0;
      else
        tmpVariance = 1.0 / (2.0 * sdevLength[i] * sdevLength[i]);

      for (l = Distribution[i].minLength(age); l < Distribution[i].maxLength(age); l++) {
        length = LgrpDiv->meanLength(l) - meanLength[i];
        if (sdevLength[i] > verysmall)
          N = exp(-(length * length * tmpVariance));
        else
          N = 0.0;
        Distribution[i][age][l].N = N;
        sum += N;
      }

      for (l = Distribution[i].minLength(age); l < Distribution[i].maxLength(age); l++) {
        length = LgrpDiv->meanLength(l);
        if (sum > rathersmall)
          Distribution[i][age][l].N *= 10000.0 / sum;
        else
          Distribution[i][age][l].N = 0.0;
        Distribution[i][age][l].W = coeff1[i] * pow(length, coeff2[i]);
      }
    }
  }
  handle.logMessage("Reset renewal data");
}

void RenewalData::addRenewal(AgeBandMatrix& Alkeys, int area, const TimeClass* const TimeInfo) {

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
      handle.logWarning("Warning in renewal - illegal number of recruits", RenewalNumber);
      RenewalNumber = -RenewalNumber;
    }

    if (RenewalNumber > verysmall)
      Alkeys.Add(Distribution[renewalid], *CI, RenewalNumber);

  }
}
