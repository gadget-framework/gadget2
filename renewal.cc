#include "renewal.h"
#include "errorhandler.h"
#include "readfunc.h"
#include "keeper.h"
#include "readword.h"
#include "gadget.h"
#include "global.h"

RenewalData::RenewalData(CommentStream& infile, const IntVector& Areas,
  const AreaClass* const Area, const TimeClass* const TimeInfo, Keeper* const keeper,
  const char* refWeightFile, const char* givenname, int minage, int maxage, double DL)
  : HasName(givenname), LivesOnAreas(Areas), CI(0), LgrpDiv(0) {

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
    readRefWeights(subcomment, tmpRefW);
    handle.Close();
    subfile.close();
    subfile.clear();

    //Interpolate the reference weights. First there are some error checks.
    if (LgrpDiv->meanLength(0) < tmpRefW[0][0] ||
        LgrpDiv->meanLength(LgrpDiv->numLengthGroups() - 1) > tmpRefW[tmpRefW.Nrow() - 1][0])
      handle.logFileMessage(LOGFAIL, "lengths for reference weights must span the range of initial condition lengths");

    //Aggregate the reference weight data to be the same format
    double ratio, tmplen;
    refWeight.resize(LgrpDiv->numLengthGroups(), 0.0);
    int pos = 0;
    for (j = 0; j < LgrpDiv->numLengthGroups(); j++) {
      tmplen = LgrpDiv->meanLength(j);
      for (i = pos; i < tmpRefW.Nrow() - 1; i++) {
        if (((tmplen > tmpRefW[i][0]) || (isEqual(tmplen, tmpRefW[i][0]))) &&
            ((tmplen < tmpRefW[i + 1][0]) || (isEqual(tmplen, tmpRefW[i + 1][0])))) {

          ratio = (tmplen - tmpRefW[i][0]) / (tmpRefW[i + 1][0] - tmpRefW[i][0]);
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
    handle.logFileMessage(LOGFAIL, "unrecognised renewal data format", text);

  keeper->clearLast();
}

void RenewalData::readNormalParameterData(CommentStream& infile, Keeper* const keeper,
  const TimeClass* const TimeInfo, const AreaClass* const Area, int minage, int maxage) {

  char c;
  int year, step, area, age, keepdata, count, reject;
  PopInfoIndexVector poptmp(LgrpDiv->numLengthGroups(), 0);

  //We now expect to find the renewal data in the following format
  //year, step, area, age, number, mean, sdev, alpha, beta
  infile >> ws;
  if (countColumns(infile) != 9)
    handle.logFileMessage(LOGFAIL, "wrong number of columns in inputfile - should be 9");

  year = step = area = age = count = reject = 0;
  while (!infile.eof()) {
    //crude check to see if something has gone wrong and avoid infinite loops
    if (!(isdigit(infile.peek())))
      handle.logFileMessage(LOGFAIL, "failed to read data from file");

    keepdata = 1;
    infile >> year >> step >> area >> age >> ws;

    if (!(TimeInfo->isWithinPeriod(year, step)))
      keepdata = 0;

    if (!(this->isInArea(Area->getInnerArea(area))))
      keepdata = 0;

    if ((age < minage) || (age > maxage))
      keepdata = 0;

    if (keepdata == 1) {
      //renewal data is required, so store it
      renewalTime.resize(1, TimeInfo->calcSteps(year, step));
      renewalArea.resize(1, Area->getInnerArea(area));
      renewalAge.resize(1, age);

      renewalMult.resize(1, keeper);
      meanLength.resize(1, keeper);
      sdevLength.resize(1, keeper);
      alpha.resize(1, keeper);
      beta.resize(1, keeper);
      renewalDistribution.resize(1, new AgeBandMatrix(age, poptmp));

      infile >> renewalMult[count] >> ws;
      infile >> meanLength[count] >> ws;
      infile >> sdevLength[count] >> ws;
      infile >> alpha[count] >> ws;
      infile >> beta[count] >> ws;

      renewalMult[count].Inform(keeper);
      meanLength[count].Inform(keeper);
      sdevLength[count].Inform(keeper);
      alpha[count].Inform(keeper);
      beta[count].Inform(keeper);
      count++;

    } else { //renewal data not required - skip rest of line
      reject++;
      infile.get(c);
      while (c != '\n' && !infile.eof())
        infile.get(c);
      infile >> ws;
    }
  }

  if (count == 0)
    handle.logMessage(LOGWARN, "Warning in renewal - found no data in the data file");
  if (reject != 0)
    handle.logMessage(LOGMESSAGE, "Discarded invalid renewal data - number of invalid entries", reject);
  handle.logMessage(LOGMESSAGE, "Read renewal data file - number of entries", count);
}

void RenewalData::readNormalConditionData(CommentStream& infile, Keeper* const keeper,
  const TimeClass* const TimeInfo, const AreaClass* const Area, int minage, int maxage) {

  char c;
  int year, step, area, age, keepdata, count, reject;
  PopInfoIndexVector poptmp(LgrpDiv->numLengthGroups(), 0);

  //We now expect to find the renewal data in the following format
  //year, step, area, age, number, mean, sdev, cond
  infile >> ws;
  if (countColumns(infile) != 8)
    handle.logFileMessage(LOGFAIL, "wrong number of columns in inputfile - should be 8");

  year = step = area = age = count = reject = 0;
  while (!infile.eof()) {
    //crude check to see if something has gone wrong and avoid infinite loops
    if (!(isdigit(infile.peek())))
      handle.logFileMessage(LOGFAIL, "failed to read data from file");

    keepdata = 1;
    infile >> year >> step >> area >> age >> ws;

    if (!(TimeInfo->isWithinPeriod(year, step)))
      keepdata = 0;

    if (!(this->isInArea(Area->getInnerArea(area))))
      keepdata = 0;

    if ((age < minage) || (age > maxage))
      keepdata = 0;

    if (keepdata == 1) {
      //renewal data is required, so store it
      renewalTime.resize(1, TimeInfo->calcSteps(year, step));
      renewalArea.resize(1, Area->getInnerArea(area));
      renewalAge.resize(1, age);

      renewalMult.resize(1, keeper);
      meanLength.resize(1, keeper);
      sdevLength.resize(1, keeper);
      relCond.resize(1, keeper);
      renewalDistribution.resize(1, new AgeBandMatrix(age, poptmp));

      infile >> renewalMult[count] >> ws;
      infile >> meanLength[count] >> ws;
      infile >> sdevLength[count] >> ws;
      infile >> relCond[count] >> ws;

      renewalMult[count].Inform(keeper);
      meanLength[count].Inform(keeper);
      sdevLength[count].Inform(keeper);
      relCond[count].Inform(keeper);
      count++;

    } else { //renewal data not required - skip rest of line
      reject++;
      infile.get(c);
      while (c != '\n' && !infile.eof())
        infile.get(c);
      infile >> ws;
    }
  }

  if (count == 0)
    handle.logMessage(LOGWARN, "Warning in renewal - found no data in the data file");
  if (reject != 0)
    handle.logMessage(LOGMESSAGE, "Discarded invalid renewal data - number of invalid entries", reject);
  handle.logMessage(LOGMESSAGE, "Read renewal data file - number of entries", count);
}

void RenewalData::readNumberData(CommentStream& infile, Keeper* const keeper,
  const TimeClass* const TimeInfo, const AreaClass* const Area, int minage, int maxage) {

  char c;
  double length;
  int i, year, step, area, age, keepdata, count, id, lengthid, reject;
  int numlen = LgrpDiv->numLengthGroups();
  PopInfoIndexVector poptmp(numlen, 0);

  //We now expect to find the renewal data in the following format
  //year, step, area, age, length, number, mean weight
  infile >> ws;
  if (countColumns(infile) != 7)
    handle.logFileMessage(LOGFAIL, "wrong number of columns in inputfile - should be 7");

  year = step = area = age = count = reject = 0;
  while (!infile.eof()) {
    //crude check to see if something has gone wrong and avoid infinite loops
    if (!(isdigit(infile.peek())))
      handle.logFileMessage(LOGFAIL, "failed to read data from file");

    keepdata = 1;
    infile >> year >> step >> area >> age >> length >> ws;

    if (!(TimeInfo->isWithinPeriod(year, step)))
      keepdata = 0;

    if (!(this->isInArea(Area->getInnerArea(area))))
      keepdata = 0;

    if ((age < minage) || (age > maxage))
      keepdata = 0;

    lengthid = -1;
    for (i = 0; i < numlen; i++)
      if (isEqual(length, LgrpDiv->minLength(i)))
        lengthid = i;

    //OK the length doesnt match a minimum length so find the length group it is in
    if ((lengthid == -1) && (length > LgrpDiv->minLength()) && (length < LgrpDiv->maxLength())) {
      for (i = 1; i < numlen; i++) {
        if (length < LgrpDiv->minLength(i)) {
          lengthid = i - 1;
          break;
        }
      }
      if (lengthid == -1)
        lengthid = numlen - 1;  //then this must be the last length group
    }

    if (lengthid == -1)
      keepdata = 0;

    if (keepdata == 1) {
      //renewal data is required, so store it
      id = -1;
      for (i = 0; i < renewalTime.Size(); i++)
        if ((renewalTime[i] == TimeInfo->calcSteps(year, step))
            && (renewalArea[i] == Area->getInnerArea(area))
            && (renewalAge[i] == age))
          id = i;

      if (id == -1) {
        //this is a new timestep/area combination
        renewalTime.resize(1, TimeInfo->calcSteps(year, step));
        renewalArea.resize(1, Area->getInnerArea(area));
        renewalAge.resize(1, age);
        id = renewalTime.Size() - 1;

        renewalDistribution.resize(1, new AgeBandMatrix(age, poptmp));
        renewalNumber.resize(new FormulaMatrix(maxage - minage + 1, numlen, 0.0));
      }

      renewalDistribution[id][age][lengthid].N = 0.0;
      infile >> (*renewalNumber[id])[age - minage][lengthid] >> ws;
      infile >> renewalDistribution[id][age][lengthid].W >> ws;
      count++;

    } else { //renewal data not required - skip rest of line
      reject++;
      infile.get(c);
      while (c != '\n' && !infile.eof())
        infile.get(c);
      infile >> ws;
    }
  }

  for (i = 0; i < renewalNumber.Size(); i++)
    (*renewalNumber[i]).Inform(keeper);

  if (count == 0)
    handle.logMessage(LOGWARN, "Warning in renewal - found no data in the data file");
  if (reject != 0)
    handle.logMessage(LOGMESSAGE, "Discarded invalid renewal data - number of invalid entries", reject);
  handle.logMessage(LOGMESSAGE, "Read renewal data file - number of entries", count);
}

RenewalData::~RenewalData() {
  int i;
  delete LgrpDiv;
  delete CI;
  for (i = 0; i < renewalNumber.Size(); i++)
    delete renewalNumber[i];
}

void RenewalData::setCI(const LengthGroupDivision* const GivenLDiv) {
  if (!checkLengthGroupStructure(GivenLDiv, LgrpDiv))
    handle.logMessage(LOGFAIL, "Error in renewal - invalid length group structure for stock", this->getName());
  //check minimum and maximum lengths
  if (LgrpDiv->minLength() < GivenLDiv->minLength())
    handle.logMessage(LOGWARN, "Warning in renewal - minimum length less than stock length for stock", this->getName());
  if (LgrpDiv->maxLength() > GivenLDiv->maxLength())
    handle.logMessage(LOGWARN, "Warning in renewal - maximum length greater than stock length for stock", this->getName());
  CI = new ConversionIndex(LgrpDiv, GivenLDiv);
  if (CI->Error())
    handle.logMessage(LOGFAIL, "Error in renewal - error when checking length structure");
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
        outfile << " multiplier " << renewalMult[i] << "\n\tNumbers\n";

      renewalDistribution[i].printNumbers(outfile);
      outfile << "\tMean weights\n";
      renewalDistribution[i].printWeights(outfile);
    }
  }
  outfile.flush();
}

void RenewalData::Reset() {
  int i, age, l, minage;
  double sum, mult, dnorm;

  index = 0;
  if (readoption == 0) {
    for (i = 0; i < renewalTime.Size(); i++) {
      age = renewalAge[i];

      //JMB check that the length data is valid
      if (isZero(meanLength[i]) || sdevLength[i] < 0.04) {
        //JMB the limit has been set at 0.04 to keep the exponential calculation sane
        handle.logMessage(LOGWARN, "Warning in renewal - invalid length data");

        //JMB set the population to zero
        for (l = renewalDistribution[i].minLength(age); l < renewalDistribution[i].maxLength(age); l++)
          renewalDistribution[i][age][l].setToZero();

      } else {
        //JMB check that the mean length is within the length group range
        if (meanLength[i] < LgrpDiv->minLength())
          handle.logMessage(LOGWARN, "Warning in renewal - mean length is less than minimum length for stock", this->getName());
        if (meanLength[i] > LgrpDiv->maxLength())
          handle.logMessage(LOGWARN, "Warning in renewal - mean length is greater than maximum length for stock", this->getName());

        sum = 0.0;
        mult = 1.0 / sdevLength[i];
        for (l = renewalDistribution[i].minLength(age); l < renewalDistribution[i].maxLength(age); l++) {
          dnorm = (LgrpDiv->meanLength(l) - meanLength[i]) * mult;
          renewalDistribution[i][age][l].N = exp(-(dnorm * dnorm) * 0.5);
          sum += renewalDistribution[i][age][l].N;
        }

        if (isZero(sum)) {
          handle.logMessage(LOGWARN, "Warning in renewal - calculated zero recruits for stock", this->getName());
          for (l = renewalDistribution[i].minLength(age); l < renewalDistribution[i].maxLength(age); l++)
            renewalDistribution[i][age][l].setToZero();

        } else {
          sum = 10000.0 / sum;
          for (l = renewalDistribution[i].minLength(age); l < renewalDistribution[i].maxLength(age); l++) {
            renewalDistribution[i][age][l].N *= sum;
            renewalDistribution[i][age][l].W = refWeight[l] * relCond[i];
            if ((handle.getLogLevel() >= LOGWARN) && (isZero(renewalDistribution[i][age][l].W)) && (renewalDistribution[i][age][l].N > 0.0))
              handle.logMessage(LOGWARN, "Warning in renewal - zero mean weight for stock", this->getName());
          }
        }
      }
    }

  } else if (readoption == 1) {
    for (i = 0; i < renewalTime.Size(); i++) {
      age = renewalAge[i];

      //JMB check that the length data is valid
      if (isZero(meanLength[i]) || sdevLength[i] < 0.04) {
        //JMB the limit has been set at 0.04 to keep the exponential calculation sane
        handle.logMessage(LOGWARN, "Warning in renewal - invalid length data");

        //JMB set the population to zero
        for (l = renewalDistribution[i].minLength(age); l < renewalDistribution[i].maxLength(age); l++)
          renewalDistribution[i][age][l].setToZero();

      } else {
        //JMB check that the mean length is within the length group range
        if (meanLength[i] < LgrpDiv->minLength())
          handle.logMessage(LOGWARN, "Warning in renewal - mean length is less than minimum length for stock", this->getName());
        if (meanLength[i] > LgrpDiv->maxLength())
          handle.logMessage(LOGWARN, "Warning in renewal - mean length is greater than maximum length for stock", this->getName());

        sum = 0.0;
        mult = 1.0 / sdevLength[i];
        for (l = renewalDistribution[i].minLength(age); l < renewalDistribution[i].maxLength(age); l++) {
          dnorm = (LgrpDiv->meanLength(l) - meanLength[i]) * mult;
          renewalDistribution[i][age][l].N = exp(-(dnorm * dnorm) * 0.5);
          sum += renewalDistribution[i][age][l].N;
        }

        if (isZero(sum)) {
          handle.logMessage(LOGWARN, "Warning in renewal - calculated zero recruits for stock", this->getName());
          for (l = renewalDistribution[i].minLength(age); l < renewalDistribution[i].maxLength(age); l++)
            renewalDistribution[i][age][l].setToZero();

        } else {
          sum = 10000.0 / sum;
          for (l = renewalDistribution[i].minLength(age); l < renewalDistribution[i].maxLength(age); l++) {
            renewalDistribution[i][age][l].N *= sum;
            renewalDistribution[i][age][l].W = alpha[i] * pow(LgrpDiv->meanLength(l), beta[i]);
            if ((handle.getLogLevel() >= LOGWARN) && (isZero(renewalDistribution[i][age][l].W)) && (renewalDistribution[i][age][l].N > 0.0))
              handle.logMessage(LOGWARN, "Warning in renewal - zero mean weight for stock", this->getName());
          }
        }
      }
    }

  } else if (readoption == 2) {
    for (i = 0; i < renewalTime.Size(); i++) {
      age = renewalAge[i];
      minage = renewalDistribution[i].minAge();
      for (l = renewalDistribution[i].minLength(age); l < renewalDistribution[i].maxLength(age); l++) {
        renewalDistribution[i][age][l].N = (*renewalNumber[i])[age - minage][l];
        if (handle.getLogLevel() >= LOGWARN) {
          if (renewalDistribution[i][age][l].N < 0.0)
            handle.logMessage(LOGWARN, "Warning in renewal - negative number of recruits", renewalDistribution[i][age][l].N);
          if ((isZero(renewalDistribution[i][age][l].W)) && (renewalDistribution[i][age][l].N > 0.0))
            handle.logMessage(LOGWARN, "Warning in renewal - zero mean weight for stock", this->getName());
        }
      }
    }

  } else
    handle.logMessage(LOGFAIL, "Error in renewal - unrecognised data format");

  if (handle.getLogLevel() >= LOGMESSAGE)
    handle.logMessage(LOGMESSAGE, "Reset renewal data for stock", this->getName());
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
      else if (renewalMult[i] > verysmall)
        Alkeys.Add(renewalDistribution[i], *CI, renewalMult[i]);
    }
  }
}
