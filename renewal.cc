#include "renewal.h"
#include "errorhandler.h"
#include "readfunc.h"
#include "keeper.h"
#include "readword.h"
#include "gadget.h"

extern ErrorHandler handle;

RenewalData::RenewalData(CommentStream& infile, const IntVector& Areas,
  const AreaClass* const Area, const TimeClass* const TimeInfo, Keeper* const keeper,
  const char* refWeightFile, int minage, int maxage, double DL)
  : LivesOnAreas(Areas), CI(0), LgrpDiv(0) {

  keeper->addString("renewaldata");
  ifstream subfile;
  CommentStream subcomment(subfile);
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);

  index = 0;
  int i, j;
  double minlength, maxlength, dl;
  readWordAndVariable(infile, "minlength", minlength);
  readWordAndVariable(infile, "maxlength", maxlength);

  //JMB - changed to make the reading of dl optional
  //if it isnt specifed here, it will default to the dl value of the stock
  infile >> ws;
  char c = infile.peek();
  if ((c == 'd') || (c == 'D'))
    readWordAndVariable(infile, "dl", dl);
  else
    dl = DL;

  LgrpDiv = new LengthGroupDivision(minlength, maxlength, dl);
  if (LgrpDiv->Error())
    handle.logMessage(LOGFAIL, "Error in renewal - failed to create length group");

  infile >> text >> ws;
  if ((strcasecmp(text, "normalcondfile") == 0)) {
    //read renewal data in mean length format, using the reference weight file
    readoption = 0;
    infile >> text >> ws;
    subfile.open(text, ios::in);
    handle.checkIfFailure(subfile, text);
    handle.Open(text);
    this->readNormalConditionData(subcomment, keeper, TimeInfo, Area, minage, maxage);
    handle.Close();
    subfile.close();
    subfile.clear();

    //read information on reference weights.
    DoubleMatrix tmpRefW;
    keeper->addString("referenceweights");
    subfile.open(refWeightFile, ios::in);
    handle.checkIfFailure(subfile, refWeightFile);
    handle.Open(refWeightFile);
    if (!readRefWeights(subcomment, tmpRefW))
      handle.logFileMessage(LOGFAIL, "Wrong format for reference weights");
    handle.Close();
    subfile.close();
    subfile.clear();

    //Interpolate the reference weights. First there are some error checks.
    if (LgrpDiv->meanLength(0) < tmpRefW[0][0] ||
        LgrpDiv->meanLength(LgrpDiv->numLengthGroups() - 1) > tmpRefW[tmpRefW.Nrow() - 1][0])
      handle.logFileMessage(LOGFAIL, "Lengths for reference weights must span the range of initial condition lengths");

    //Aggregate the reference weight data to be the same format
    double ratio;
    refWeight.resize(LgrpDiv->numLengthGroups());
    int pos = 0;
    for (j = 0; j < LgrpDiv->numLengthGroups(); j++) {
      for (i = pos; i < tmpRefW.Nrow() - 1; i++) {
        if ((LgrpDiv->meanLength(j) >= tmpRefW[i][0]) &&
            (LgrpDiv->meanLength(j) <= tmpRefW[i + 1][0])) {

          ratio = (LgrpDiv->meanLength(j) - tmpRefW[i][0]) / (tmpRefW[i + 1][0] - tmpRefW[i][0]);
          refWeight[j] = tmpRefW[i][1] + ratio * (tmpRefW[i + 1][1] - tmpRefW[i][1]);
          pos = i;
        }
      }
    }
    keeper->clearLast();

  } else if ((strcasecmp(text, "renewaldatafile") == 0) || (strcasecmp(text, "normalparamfile") == 0)) {
    //read renewal data in mean length format, using a length weight relationship
    readoption = 1;
    infile >> text >> ws;
    subfile.open(text, ios::in);
    handle.checkIfFailure(subfile, text);
    handle.Open(text);
    this->readNormalParameterData(subcomment, keeper, TimeInfo, Area, minage, maxage);
    handle.Close();
    subfile.close();
    subfile.clear();

  } else if ((strcasecmp(text, "numberfile") == 0)) {
    //read renewal data in number format
    readoption = 2;
    infile >> text >> ws;
    subfile.open(text, ios::in);
    handle.checkIfFailure(subfile, text);
    handle.Open(text);
    this->readNumberData(subcomment, keeper, TimeInfo, Area, minage, maxage);
    handle.Close();
    subfile.close();
    subfile.clear();

  } else
    handle.logFileMessage(LOGFAIL, "Error in renewal - unrecognised data format", text);

  keeper->clearLast();
}

void RenewalData::readNormalParameterData(CommentStream& infile, Keeper* const keeper,
  const TimeClass* const TimeInfo, const AreaClass* const Area, int minage, int maxage) {

  int count = 0;
  int year, step, area, age;
  int keepdata;

  //Find start of data in datafile
  infile >> ws;
  if (infile.eof()) {
    handle.logFileMessage(LOGFAIL, "Error in renewal - renewal data file empty");
    return;
  }

  char c = infile.peek();
  if (!isdigit(c)) {
    infile.get(c);
    while (c != '\n' && !infile.eof())
      infile.get(c);
  }

  //We now expect to find the renewal data in the following format
  //year, step, area, age, number, mean, sdev, alpha, beta
  if (countColumns(infile) != 9)
    handle.logFileMessage(LOGFAIL, "Wrong number of columns in inputfile - should be 9");

  while (!infile.eof()) {
    keepdata = 0;
    infile >> year >> step >> area >> age >> ws;

    if (!(TimeInfo->isWithinPeriod(year, step))) {
      handle.logFileMessage(LOGWARN, "Warning in renewal - ignoring data found outside time range");
      keepdata = 1;
    }

    if (!(this->isInArea(Area->InnerArea(area)))) {
      handle.logFileMessage(LOGWARN, "Warning in renewal - ignoring data found outside area range");
      keepdata = 1;
    }

    if ((age < minage) || (age > maxage)) {
      handle.logFileMessage(LOGWARN, "Warning in renewal - ignoring data found outside age range");
      keepdata = 1;
    }

    if (keepdata == 0) {
      //renewal data is required, so store it
      renewalTime.resize(1, TimeInfo->calcSteps(year, step));
      renewalArea.resize(1, Area->InnerArea(area));
      renewalAge.resize(1, age);

      renewalNumber.resize(1, keeper);
      meanLength.resize(1, keeper);
      sdevLength.resize(1, keeper);
      alpha.resize(1, keeper);
      beta.resize(1, keeper);

      PopInfoIndexVector poptmp(LgrpDiv->numLengthGroups(), 0);
      renewalDistribution.resize(1, new AgeBandMatrix(age, poptmp));

      infile >> renewalNumber[count] >> ws;
      infile >> meanLength[count] >> ws;
      infile >> sdevLength[count] >> ws;
      infile >> alpha[count] >> ws;
      infile >> beta[count] >> ws;

      renewalNumber[count].Inform(keeper);
      meanLength[count].Inform(keeper);
      sdevLength[count].Inform(keeper);
      alpha[count].Inform(keeper);
      beta[count].Inform(keeper);
      count++;

    } else { //renewal data not required - skip rest of line
      infile.get(c);
      while (c != '\n' && !infile.eof())
        infile.get(c);
      infile >> ws;
    }
  }

  if (handle.getLogLevel() >= LOGMESSAGE)
    handle.logMessage(LOGMESSAGE, "Read renewal data file - number of entries", count);
}

void RenewalData::readNormalConditionData(CommentStream& infile, Keeper* const keeper,
  const TimeClass* const TimeInfo, const AreaClass* const Area, int minage, int maxage) {

  int count = 0;
  int year, step, area, age;
  int keepdata;

  //Find start of data in datafile
  infile >> ws;
  if (infile.eof()) {
    handle.logFileMessage(LOGFAIL, "Error in renewal - renewal data file empty");
    return;
  }

  char c = infile.peek();
  if (!isdigit(c)) {
    infile.get(c);
    while (c != '\n' && !infile.eof())
      infile.get(c);
  }

  //We now expect to find the renewal data in the following format
  //year, step, area, age, number, mean, sdev, cond
  if (countColumns(infile) != 8)
    handle.logFileMessage(LOGFAIL, "Wrong number of columns in inputfile - should be 8");

  while (!infile.eof()) {
    keepdata = 0;
    infile >> year >> step >> area >> age >> ws;

    if (!(TimeInfo->isWithinPeriod(year, step))) {
      handle.logFileMessage(LOGWARN, "Warning in renewal - ignoring data found outside time range");
      keepdata = 1;
    }

    if (!(this->isInArea(Area->InnerArea(area)))) {
      handle.logFileMessage(LOGWARN, "Warning in renewal - ignoring data found outside area range");
      keepdata = 1;
    }

    if ((age < minage) || (age > maxage)) {
      handle.logFileMessage(LOGWARN, "Warning in renewal - ignoring data found outside age range");
      keepdata = 1;
    }

    if (keepdata == 0) {
      //renewal data is required, so store it
      renewalTime.resize(1, TimeInfo->calcSteps(year, step));
      renewalArea.resize(1, Area->InnerArea(area));
      renewalAge.resize(1, age);

      renewalNumber.resize(1, keeper);
      meanLength.resize(1, keeper);
      sdevLength.resize(1, keeper);
      relCond.resize(1, keeper);

      PopInfoIndexVector poptmp(LgrpDiv->numLengthGroups(), 0);
      renewalDistribution.resize(1, new AgeBandMatrix(age, poptmp));

      infile >> renewalNumber[count] >> ws;
      infile >> meanLength[count] >> ws;
      infile >> sdevLength[count] >> ws;
      infile >> relCond[count] >> ws;

      renewalNumber[count].Inform(keeper);
      meanLength[count].Inform(keeper);
      sdevLength[count].Inform(keeper);
      relCond[count].Inform(keeper);
      count++;

    } else { //renewal data not required - skip rest of line
      infile.get(c);
      while (c != '\n' && !infile.eof())
        infile.get(c);
      infile >> ws;
    }
  }

  if (handle.getLogLevel() >= LOGMESSAGE)
    handle.logMessage(LOGMESSAGE, "Read renewal data file - number of entries", count);
}

void RenewalData::readNumberData(CommentStream& infile, Keeper* const keeper,
  const TimeClass* const TimeInfo, const AreaClass* const Area, int minage, int maxage) {

  int count = 0;
  int i, year, step, area, age;
  double length, tmpnumber, tmpweight;
  int keepdata, id, lengthid;

  //Find start of data in datafile
  infile >> ws;
  if (infile.eof()) {
    handle.logFileMessage(LOGFAIL, "Error in renewal - renewal data file empty");
    return;
  }

  char c = infile.peek();
  if (!isdigit(c)) {
    infile.get(c);
    while (c != '\n' && !infile.eof())
      infile.get(c);
  }

  //We now expect to find the renewal data in the following format
  //year, step, area, age, length, number, mean weight
  if (countColumns(infile) != 7)
    handle.logFileMessage(LOGFAIL, "Wrong number of columns in inputfile - should be 7");

  while (!infile.eof()) {
    keepdata = 0;
    infile >> year >> step >> area >> age >> length >> tmpnumber >> tmpweight >> ws;

    if (!(TimeInfo->isWithinPeriod(year, step))) {
      handle.logFileMessage(LOGWARN, "Warning in renewal - ignoring data found outside time range");
      keepdata = 1;
    }

    if (!(this->isInArea(Area->InnerArea(area)))) {
      handle.logFileMessage(LOGWARN, "Warning in renewal - ignoring data found outside area range");
      keepdata = 1;
    }

    if ((age < minage) || (age > maxage)) {
      handle.logFileMessage(LOGWARN, "Warning in renewal - ignoring data found outside age range");
      keepdata = 1;
    }

    lengthid = -1;
    for (i = 0; i < LgrpDiv->numLengthGroups(); i++)
      if (length == LgrpDiv->minLength(i))
        lengthid = i;

    if (lengthid == -1) {
      handle.logFileMessage(LOGWARN, "Warning in renewal - ignoring data found outside length range");
      keepdata = 1;
    }

    if (keepdata == 0) {
      //renewal data is required, so store it
      id = -1;
      for (i = 0; i < renewalTime.Size(); i++)
        if ((renewalTime[i] == TimeInfo->calcSteps(year, step))
            && (renewalArea[i] == Area->InnerArea(area))
            && (renewalAge[i] == age))
          id = i;

      if (id == -1) {
        //this is a new timestep/area combination
        renewalTime.resize(1, TimeInfo->calcSteps(year, step));
        renewalArea.resize(1, Area->InnerArea(area));
        renewalAge.resize(1, age);
        id = renewalTime.Size() - 1;

        PopInfoIndexVector poptmp(LgrpDiv->numLengthGroups(), 0);
        renewalDistribution.resize(1, new AgeBandMatrix(age, poptmp));
      }

      if ((isZero(tmpweight)) && (tmpnumber > 0))
        handle.logFileMessage(LOGWARN, "Warning in renewal - zero mean weight");

      renewalDistribution[id][age][lengthid].N = tmpnumber;
      renewalDistribution[id][age][lengthid].W = tmpweight;
      count++;
    }
    infile >> ws;
  }

  if (handle.getLogLevel() >= LOGMESSAGE)
    handle.logMessage(LOGMESSAGE, "Read renewal data file - number of entries", count);
}

RenewalData::~RenewalData() {
  delete LgrpDiv;
  delete CI;
}

void RenewalData::setCI(const LengthGroupDivision* const GivenLDiv) {
  //check minimum and maximum lengths
  if (handle.getLogLevel() >= LOGWARN) {
    if (LgrpDiv->minLength() < GivenLDiv->minLength())
      handle.logMessage(LOGWARN, "Warning in renewal - minimum length less than stock length");
    if (LgrpDiv->maxLength() > GivenLDiv->maxLength())
      handle.logMessage(LOGWARN, "Warning in renewal - maximum length greater than stock length");
  }
  CI = new ConversionIndex(LgrpDiv, GivenLDiv);
}

void RenewalData::Print(ofstream& outfile) const {
  int i;

  outfile << "\nRenewal data\n\t";
  LgrpDiv->Print(outfile);
  //need to find all areas and ages for current index value ...
  for (i = 0; i < renewalTime.Size(); i++) {
    if (renewalTime[i] == renewalTime[index]) {
      outfile << "\tInternal area " << renewalArea[i] << " age " << renewalAge[i];

      if (readoption == 2)
        outfile << "\n\tNumbers\n";
      else
        outfile << " multiplier " << renewalNumber[i] << "\n\tNumbers\n";

      renewalDistribution[i].printNumbers(outfile);
      outfile << "\tMean weights\n";
      renewalDistribution[i].printWeights(outfile);
    }
  }
  outfile.flush();
}

void RenewalData::Reset() {
  int i, age, l;
  double sum, mult, dnorm;

  index = 0;
  if (readoption == 0) {
    for (i = 0; i < renewalDistribution.Size(); i++) {
      age = renewalAge[i];
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
        renewalDistribution[i][age][l].W = refWeight[l] * relCond[i];
        if ((handle.getLogLevel() >= LOGWARN) && (isZero(renewalDistribution[i][age][l].W)) && (renewalDistribution[i][age][l].N > 0))
          handle.logMessage(LOGWARN, "Warning in renewal - zero mean weight");
      }
    }

  } else if (readoption == 1) {
    for (i = 0; i < renewalDistribution.Size(); i++) {
      age = renewalAge[i];
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
        if ((handle.getLogLevel() >= LOGWARN) && (isZero(renewalDistribution[i][age][l].W)) && (renewalDistribution[i][age][l].N > 0))
          handle.logMessage(LOGWARN, "Warning in renewal - zero mean weight");
      }
    }

  } else if (readoption == 2) {
    //nothing to be done here

  } else
    handle.logMessage(LOGFAIL, "Error in renewal - unrecognised data format");

  if (handle.getLogLevel() >= LOGMESSAGE)
    handle.logMessage(LOGMESSAGE, "Reset renewal data");
}

int RenewalData::isRenewalStepArea(int area, const TimeClass* const TimeInfo) {
  int i;
  for (i = 0; i < renewalTime.Size(); i++)
    if ((renewalTime[i] == TimeInfo->getTime()) && (renewalArea[i] == area))
      return 1;
  return 0;
}

void RenewalData::addRenewal(AgeBandMatrix& Alkeys, int area, const TimeClass* const TimeInfo) {
  int i;
  for (i = 0; i < renewalTime.Size(); i++) {
    if ((renewalTime[i] == TimeInfo->getTime()) && (renewalArea[i] == area)) {
      index = i;
      if (readoption == 2)
        Alkeys.Add(renewalDistribution[i], *CI);
      else if (renewalNumber[i] > verysmall)
        Alkeys.Add(renewalDistribution[i], *CI, renewalNumber[i]);
    }
  }
}
