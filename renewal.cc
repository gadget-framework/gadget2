#include "renewal.h"
#include "errorhandler.h"
#include "readfunc.h"
#include "keeper.h"
#include "readword.h"
#include "gadget.h"

extern ErrorHandler handle;

RenewalData::RenewalData(CommentStream& infile, const IntVector& Areas,
  const AreaClass* const Area, const TimeClass* const TimeInfo, Keeper* const keeper,
  double DL) : LivesOnAreas(Areas), CI(0), LgrpDiv(0) {

  keeper->addString("renewaldata");
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);

  int i = 0;
  timeindex = 0;
  int year, step, area, age, ind, no;
  double minlength, maxlength, dl;

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
  //JMB - changed to make the reading of dl optional
  //If it isnt specifed here, it will default to the dl value of the stock
  infile >> ws;
  char c = infile.peek();
  if ((c == 'd') || (c == 'D'))
    readWordAndVariable(infile, "dl", dl);
  else
    dl = DL;

  LgrpDiv = new LengthGroupDivision(minlength, maxlength, dl);
  if (LgrpDiv->Error())
    handle.Message("Error in renewal - failed to create length group");

  //We now expect to find:
  //year, step, area, age and then the renewal data
  infile >> year >> step >> area >> age >> ws;
  while (isdigit(infile.peek()) && !infile.eof()) {
    if (TimeInfo->isWithinPeriod(year, step)) {

      renewalTime.resize(1);
      renewalTime[i] = TimeInfo->calcSteps(year, step);

      if (!this->isInArea(Area->InnerArea(area)))
        handle.Message("Error in renewal - stock undefined on area");

      renewalArea.resize(1);
      renewalArea[i] = Area->InnerArea(area);

      renewalNumber.resize(1, keeper);

      if (readOption == 0) {
        infile >> minlength >> no >> ws;

        if (!(infile >> renewalNumber[i]))
          handle.Message("Error in renewal - wrong format for renewalmultiplier");
        renewalNumber[i].Inform(keeper);

        //Now we read first the length distribution pattern and then
        //the corresponding mean weights. Both are assumed to be vectors of
        //length no. We read them into the indexvectors numtmpindvec and
        //weighttmpindvec, create poptmp and then keep it in Distribution.
        ind = LgrpDiv->numLengthGroup(minlength);
        DoubleIndexVector* numtmpindvec = new DoubleIndexVector(no, ind);
        DoubleIndexVector* weighttmpindvec = new DoubleIndexVector(no, ind);
        PopInfoIndexVector poptmp(no, ind);

        if (!readIndexVector(infile, (*numtmpindvec)))
          handle.Message("Error in renewal - failure in numbers of recruits");
        if (!readIndexVector(infile, (*weighttmpindvec)))
          handle.Message("Error in renewal - failure in weights of recruits");

        for (ind = poptmp.minCol(); ind < poptmp.maxCol(); ind++) {
          poptmp[ind].N = (*numtmpindvec)[ind];
          poptmp[ind].W = (*weighttmpindvec)[ind];
          //Check if any (i.e. nonzero) part of the population has zero mean weight.
          if (poptmp[ind].N > 0 && isZero(poptmp[ind].W))
            handle.Message("Error in renewal - zero mean weight for nonzero number of recruits");
        }
        renewalDistribution.resize(1, new AgeBandMatrix(age, poptmp));
        delete numtmpindvec;
        delete weighttmpindvec;

      } else if (readOption == 1) { //use meanlengths.
        meanLength.resize(1, keeper);
        sdevLength.resize(1, keeper);
        coeff1.resize(1, keeper);
        coeff2.resize(1, keeper);

        PopInfoIndexVector poptmp(LgrpDiv->numLengthGroups(), 0);
        renewalDistribution.resize(1, new AgeBandMatrix(age, poptmp));
        if (!(infile >> renewalNumber[i]))
          handle.Message("Error in renewal - wrong format for renewal multiplier");
        renewalNumber[i].Inform(keeper);
        if (!(infile >> meanLength[i]))
          handle.Message("Error in renewal - wrong format for renewal mean length");
        meanLength[i].Inform(keeper);
        if (!(infile >> sdevLength[i]))
          handle.Message("Error in renewal - wrong format for renewal standard deviation");
        sdevLength[i].Inform(keeper);
        if (!(infile >> coeff1[i]))
          handle.Message("Error in renewal - wrong format for renewal wcoeff1");
        coeff1[i].Inform(keeper);
        if (!(infile >> coeff2[i]))
          handle.Message("Error in renewal - wrong format for renewal wcoeff2");
        coeff2[i].Inform(keeper);

      } else
        handle.Message("Error in renewal - unrecognised format for renewal data ");

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
  handle.logMessage("Read recruits data file - number of entries", renewalTime.Size());
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
  outfile << "\nRenewal data\n\t";
  LgrpDiv->Print(outfile);
  outfile << "\tInternal area " << renewalArea[timeindex]
    << " age " << renewalDistribution[timeindex].minAge()
    << " number " << renewalNumber[timeindex] << "\n\tNumbers\n";
  renewalDistribution[timeindex].printNumbers(outfile);
  outfile << "\tMean weights\n";
  renewalDistribution[timeindex].printWeights(outfile);
}

void RenewalData::Reset() {
  int i, age, l;
  double sum, length, N, tmpVariance;

  timeindex = 0;
  if (readOption == 1) {
    for (i = 0; i < renewalDistribution.Size(); i++) {
      age = renewalDistribution[i].minAge();
      sum = 0.0;
      N = 0.0;
      if (isZero(sdevLength[i]))
        tmpVariance = 0.0;
      else
        tmpVariance = 1.0 / (2.0 * sdevLength[i] * sdevLength[i]);

      for (l = renewalDistribution[i].minLength(age);
           l < renewalDistribution[i].maxLength(age); l++) {
        length = LgrpDiv->meanLength(l) - meanLength[i];
        if (sdevLength[i] > verysmall)
          N = exp(-(length * length * tmpVariance));
        else
          N = 0.0;
        renewalDistribution[i][age][l].N = N;
        sum += N;
      }

      for (l = renewalDistribution[i].minLength(age);
           l < renewalDistribution[i].maxLength(age); l++) {
        length = LgrpDiv->meanLength(l);
        if (sum > rathersmall)
          renewalDistribution[i][age][l].N *= 10000.0 / sum;
        else
          renewalDistribution[i][age][l].N = 0.0;
        renewalDistribution[i][age][l].W = coeff1[i] * pow(length, coeff2[i]);
      }
    }
  }
  handle.logMessage("Reset renewal data");
}

void RenewalData::addRenewal(AgeBandMatrix& Alkeys, int area, const TimeClass* const TimeInfo) {

  if (renewalTime.Size() == 0)
    return;

  int i;
  double renew = 0.0;
  int renewalid = -1;

  for (i = 0; i < renewalTime.Size(); i++)
    if ((renewalTime[i] == TimeInfo->CurrentTime()) && (renewalArea[i] == area)) {
      renewalid = i;
      break;
    }

  //Add renewal to stock
  if (renewalid != -1) {
    timeindex = renewalid;
    if (isZero(renewalNumber[renewalid]))
      renew = 0.0;
    else
      renew = renewalNumber[renewalid];

    if (renew < 0) {
      handle.logWarning("Warning in renewal - invalid number of recruits", renew);
      renew = -renew;
    }

    if (renew > verysmall)
      Alkeys.Add(renewalDistribution[renewalid], *CI, renew);

  }
}
