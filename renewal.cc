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

  index = 0;
  double minlength, maxlength, dl;

  infile >> text;
  if (strcasecmp(text, "normaldistribution") == 0) {
    readoption = 1;
    readWordAndVariable(infile, "minlength", minlength);
  } else if (strcasecmp(text, "minlength") == 0) {
    readoption = 0;
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

  if (readoption == 0) {
    this->readNumberData(infile, keeper, TimeInfo, Area);

  } else if (readoption == 1) {
    this->readNormalData(infile, keeper, TimeInfo, Area);

  } else
    handle.Message("Error in renewal - unrecognised format for renewal data");

  handle.logMessage("Read renewal data file - number of entries", renewalTime.Size());
  keeper->clearLast();
}

void RenewalData::readNormalData(CommentStream& infile, Keeper* const keeper,
  const TimeClass* const TimeInfo, const AreaClass* const Area) {

  int i = 0;
  int year, step, area, age;
  int nolengr = LgrpDiv->numLengthGroups();

  //Find start of data in datafile
  infile >> ws;
  if (infile.eof()) {
    handle.Message("Error in renewal - renewal data file empty");
    return;
  }

  char c = infile.peek();
  if (!isdigit(c)) {
    infile.get(c);
    while (c != '\n' && !infile.eof())
      infile.get(c);
  }

  if (countColumns(infile) != 9)
    handle.Message("Wrong number of columns in inputfile - should be 9");

  while (!infile.eof()) {
    //We now expect to find the renewal data in the following format
    //year, step, area, age, number, mean, sdev, alpha, beta
    infile >> year >> step >> area >> age >> ws;
    if (TimeInfo->isWithinPeriod(year, step)) {
      renewalTime.resize(1);
      renewalTime[i] = TimeInfo->calcSteps(year, step);

      if (!this->isInArea(Area->InnerArea(area)))
        handle.Message("Error in renewal - stock undefined on area");

      renewalArea.resize(1);
      renewalArea[i] = Area->InnerArea(area);
      renewalNumber.resize(1, keeper);
      meanLength.resize(1, keeper);
      sdevLength.resize(1, keeper);
      alpha.resize(1, keeper);
      beta.resize(1, keeper);

      PopInfoIndexVector poptmp(nolengr, 0);
      renewalDistribution.resize(1, new AgeBandMatrix(age, poptmp));
      if (!(infile >> renewalNumber[i]))
        handle.Message("Error in renewal - wrong format for renewal multiplier");
      if (!(infile >> meanLength[i]))
        handle.Message("Error in renewal - wrong format for renewal mean length");
      if (!(infile >> sdevLength[i]))
        handle.Message("Error in renewal - wrong format for renewal standard deviation");
      if (!(infile >> alpha[i]))
        handle.Message("Error in renewal - wrong format for renewal parameter alpha");
      if (!(infile >> beta[i]))
        handle.Message("Error in renewal - wrong format for renewal parameter beta");

      renewalNumber[i].Inform(keeper);
      meanLength[i].Inform(keeper);
      sdevLength[i].Inform(keeper);
      alpha[i].Inform(keeper);
      beta[i].Inform(keeper);

    } else { //This year and step is not required - skip rest of line
      infile.get(c);
      while (c != '\n' && !infile.eof())
        infile.get(c);
    }

    infile >> ws;
    i++;
  }
}

void RenewalData::readNumberData(CommentStream& infile, Keeper* const keeper,
  const TimeClass* const TimeInfo, const AreaClass* const Area) {

  int i = 0;
  int year, step, area, age, minlength, no, ind;

  //Find start of data in datafile
  infile >> ws;
  if (infile.eof()) {
    handle.Message("Error in renewal - renewal data file empty");
    return;
  }

  char c = infile.peek();
  if (!isdigit(c)) {
    infile.get(c);
    while (c != '\n' && !infile.eof())
      infile.get(c);
  }

  while (!infile.eof()) {
    //We now expect to find the renewal data in the following format
    //year, step, area, age and then the renewal data
    infile >> year >> step >> area >> age >> minlength >> no >> ws;

    if (TimeInfo->isWithinPeriod(year, step)) {
      renewalTime.resize(1);
      renewalTime[i] = TimeInfo->calcSteps(year, step);

      if (!this->isInArea(Area->InnerArea(area)))
        handle.Message("Error in renewal - stock undefined on area");

      renewalArea.resize(1);
      renewalArea[i] = Area->InnerArea(area);
      renewalNumber.resize(1, keeper);
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

    } else { //This year and step is not required - skip rest of line
      infile.get(c);
      while (c != '\n' && !infile.eof())
        infile.get(c);
    }

    infile >> ws;
    i++;
  }
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
  outfile << "\tInternal area " << renewalArea[index]
    << " age " << renewalDistribution[index].minAge()
    << " number " << renewalNumber[index] << "\n\tNumbers\n";
  renewalDistribution[index].printNumbers(outfile);
  outfile << "\tMean weights\n";
  renewalDistribution[index].printWeights(outfile);
}

void RenewalData::Reset() {
  int i, age, l;
  double sum, mult, dnorm;

  index = 0;
  if (readoption == 1) {
    for (i = 0; i < renewalDistribution.Size(); i++) {
      age = renewalDistribution[i].minAge();
      sum = 0.0;

      if (isZero(sdevLength[i]))
        mult = 0.0;
      else        
        mult = 1.0 / sdevLength[i];

      for (l = renewalDistribution[i].minLength(age);
           l < renewalDistribution[i].maxLength(age); l++) {

        dnorm = (LgrpDiv->meanLength(l) - meanLength[i]) * mult;
        renewalDistribution[i][age][l].N = exp(-(dnorm * dnorm) * 0.5);
        sum += renewalDistribution[i][age][l].N;
      }

      sum = (sum > rathersmall ? 10000.0 / sum : 0.0);
      for (l = renewalDistribution[i].minLength(age);
           l < renewalDistribution[i].maxLength(age); l++) {

        renewalDistribution[i][age][l].N *= sum;
        renewalDistribution[i][age][l].W = alpha[i] * pow(LgrpDiv->meanLength(l), beta[i]);
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
    index = renewalid;
    if (isZero(renewalNumber[index]))
      renew = 0.0;
    else
      renew = renewalNumber[index];

    if (renew < 0) {
      handle.logWarning("Warning in renewal - invalid number of recruits", renew);
      renew = -renew;
    }

    if (renew > verysmall)
      Alkeys.Add(renewalDistribution[index], *CI, renew);

  }
}
