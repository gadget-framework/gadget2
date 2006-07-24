#include "initialcond.h"
#include "errorhandler.h"
#include "readfunc.h"
#include "readword.h"
#include "mathfunc.h"
#include "gadget.h"

extern ErrorHandler handle;

void InitialCond::readNormalConditionData(CommentStream& infile, Keeper* const keeper,
  int noagegr, int minage, const AreaClass* const Area) {

  int noareas = areas.Size();
  int i, age, area, ageid, areaid, tmparea, keepdata, count, reject;
  char c;

  //Resize the matrices to hold the data
  areaFactor.AddRows(noareas, noagegr, 0.0);
  ageFactor.AddRows(noareas, noagegr, 0.0);
  meanLength.AddRows(noareas, noagegr, 0.0);
  sdevLength.AddRows(noareas, noagegr, 0.0);
  relCond.AddRows(noareas, noagegr, 0.0);

  //Find the start of the data in the following format
  //age - area - agedist - areadist - meanlen - standdev - relcond
  infile >> ws;
  if (countColumns(infile) != 7)
    handle.logFileMessage(LOGFAIL, "wrong number of columns in inputfile - should be 7");

  area = age = ageid = count = reject = 0;
  keeper->addString("meandata");
  while (!infile.eof()) {
    keepdata = 1;
    infile >> age >> area >> ws;

    //crude age data check - perhaps there should be a better check?
    if ((age < minage) || (age >= (noagegr + minage)))
      keepdata = 0;
    else
      ageid = age - minage;

    //crude area data check
    areaid = -1;
    tmparea = Area->getInnerArea(area);
    for (i = 0; i < noareas; i++)
      if (areas[i] == tmparea)
        areaid = i;

    if (areaid == -1)
      keepdata = 0;

    if (keepdata == 1) {
      //initial data is required, so store it
      count++;
      infile >> ageFactor[areaid][ageid] >> ws;
      infile >> areaFactor[areaid][ageid] >> ws;
      infile >> meanLength[areaid][ageid] >> ws;
      infile >> sdevLength[areaid][ageid] >> ws;
      infile >> relCond[areaid][ageid] >> ws;

    } else { //initial data not required - skip rest of line
      reject++;
      infile.get(c);
      while (c != '\n' && !infile.eof())
        infile.get(c);
      infile >> ws;
    }
  }

  if (count == 0)
    handle.logMessage(LOGWARN, "Warning in initial conditions - found no data in the data file");
  else if (count < (noareas * noagegr))
    handle.logMessage(LOGWARN, "Warning in initial conditions - missing entries from data file");
  else if (count > (noareas * noagegr))
    handle.logMessage(LOGWARN, "Warning in initial conditions - repeated entries in data file");

  if (reject != 0)
    handle.logMessage(LOGMESSAGE, "Discarded invalid initial conditions data - number of invalid entries", reject);

  handle.logMessage(LOGMESSAGE, "Read initial conditions data file - number of entries", count);
  areaFactor.Inform(keeper);
  ageFactor.Inform(keeper);
  meanLength.Inform(keeper);
  sdevLength.Inform(keeper);
  relCond.Inform(keeper);
  keeper->clearLast();
}

void InitialCond::readNormalParameterData(CommentStream& infile, Keeper* const keeper,
  int noagegr, int minage, const AreaClass* const Area) {

  int noareas = areas.Size();
  int i, age, area, ageid, areaid, tmparea, keepdata, count, reject;
  char c;

  //Resize the matrices to hold the data
  areaFactor.AddRows(noareas, noagegr, 0.0);
  ageFactor.AddRows(noareas, noagegr, 0.0);
  meanLength.AddRows(noareas, noagegr, 0.0);
  sdevLength.AddRows(noareas, noagegr, 0.0);
  alpha.AddRows(noareas, noagegr, 0.0);
  beta.AddRows(noareas, noagegr, 0.0);

  //Find the start of the data in the following format
  //age - area - agedist - areadist - meanlen - standdev - alpha - beta
  infile >> ws;
  if (countColumns(infile) != 8)
    handle.logFileMessage(LOGFAIL, "wrong number of columns in inputfile - should be 8");

  area = age = ageid = count = reject = 0;
  keeper->addString("meandata");
  while (!infile.eof()) {
    keepdata = 1;
    infile >> age >> area >> ws;

    //crude age data check - perhaps there should be a better check?
    if ((age < minage) || (age >= (noagegr + minage)))
      keepdata = 0;
    else
      ageid = age - minage;

    //crude area data check
    areaid = -1;
    tmparea = Area->getInnerArea(area);
    for (i = 0; i < noareas; i++)
      if (areas[i] == tmparea)
        areaid = i;

    if (areaid == -1)
      keepdata = 0;

    if (keepdata == 1) {
      //initial data is required, so store it
      count++;
      infile >> ageFactor[areaid][ageid] >> ws;
      infile >> areaFactor[areaid][ageid] >> ws;
      infile >> meanLength[areaid][ageid] >> ws;
      infile >> sdevLength[areaid][ageid] >> ws;
      infile >> alpha[areaid][ageid] >> ws;
      infile >> beta[areaid][ageid] >> ws;

    } else { //initial data not required - skip rest of line
      reject++;
      infile.get(c);
      while (c != '\n' && !infile.eof())
        infile.get(c);
      infile >> ws;
    }
  }

  if (count == 0)
    handle.logMessage(LOGWARN, "Warning in initial conditions - found no data in the data file");
  else if (count < (noareas * noagegr))
    handle.logMessage(LOGWARN, "Warning in initial conditions - missing entries from data file");
  else if (count > (noareas * noagegr))
    handle.logMessage(LOGWARN, "Warning in initial conditions - repeated entries in data file");

  if (reject != 0)
    handle.logMessage(LOGMESSAGE, "Discarded invalid initial conditions data - number of invalid entries", reject);

  handle.logMessage(LOGMESSAGE, "Read initial conditions data file - number of entries", count);
  areaFactor.Inform(keeper);
  ageFactor.Inform(keeper);
  meanLength.Inform(keeper);
  sdevLength.Inform(keeper);
  alpha.Inform(keeper);
  beta.Inform(keeper);
  keeper->clearLast();
}

void InitialCond::readNumberData(CommentStream& infile, Keeper* const keeper,
  int noagegr, int minage, const AreaClass* const Area) {

  int i, age, area, tmparea, count, reject;
  int keepdata, ageid, areaid, lengthid;
  double length;
  char c;
  int noareas = areas.Size();
  int nolengr = LgrpDiv->numLengthGroups();

  //Find the start of the data in the following format
  //area - age - meanlength - number - weight
  infile >> ws;
  if (countColumns(infile) != 5)
    handle.logFileMessage(LOGFAIL, "wrong number of columns in inputfile - should be 5");

  //initialise things
  for (areaid = 0; areaid < noareas; areaid++) {
    initialNumber.resize(new FormulaMatrix(noagegr, nolengr, 0.0));
    for (ageid = minage; ageid < noagegr + minage; ageid++)
      for (lengthid = 0; lengthid < nolengr; lengthid++)
        initialPop[areaid][ageid][lengthid].setToZero();
  }

  area = age = ageid = count = reject = 0;
  keeper->addString("numberdata");
  while (!infile.eof()) {
    keepdata = 1;
    infile >> area >> age >> length >> ws;

    //crude age data check - perhaps there should be a better check?
    if ((age < minage) || (age >= (noagegr + minage)))
      keepdata = 0;
    else
      ageid = age;

    //crude length data check
    lengthid = -1;
    for (i = 0; i < nolengr; i++)
      if (length == LgrpDiv->minLength(i))
        lengthid = i;

    if (lengthid == -1)
      keepdata = 0;

    //crude area data check
    areaid = -1;
    tmparea = Area->getInnerArea(area);
    for (i = 0; i < noareas; i++)
      if (areas[i] == tmparea)
        areaid = i;

    if (areaid == -1)
      keepdata = 0;

    if (keepdata == 1) {
      //initial data is required, so store it
      infile >> (*initialNumber[areaid])[ageid - minage][lengthid] >> ws;
      infile >> initialPop[areaid][ageid][lengthid].W >> ws;
      count++;

    } else { //initial data not required - skip rest of line
      reject++;
      infile.get(c);
      while (c != '\n' && !infile.eof())
        infile.get(c);
      infile >> ws;
    }
  }

  for (i = 0; i < initialNumber.Size(); i++)
    (*initialNumber[i]).Inform(keeper);

  if (count == 0)
    handle.logMessage(LOGWARN, "Warning in initial conditions - found no data in the data file");
  else if (count < (noareas * noagegr * nolengr))
    handle.logMessage(LOGWARN, "Warning in initial conditions - missing entries from data file");
  else if (count > (noareas * noagegr * nolengr))
    handle.logMessage(LOGWARN, "Warning in initial conditions - repeated entries in data file");

  if (reject != 0)
    handle.logMessage(LOGMESSAGE, "Discarded invalid initial conditions data - number of invalid entries", reject);

  handle.logMessage(LOGMESSAGE, "Read initial conditions data file - number of entries", count);
  keeper->clearLast();
}

InitialCond::InitialCond(CommentStream& infile, const IntVector& Areas,
  Keeper* const keeper, const char* refWeightFile, const AreaClass* const Area, double DL)
  : LivesOnAreas(Areas), LgrpDiv(0), CI(0) {

  ifstream subfile;
  CommentStream subcomment(subfile);
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);

  int i, j, k;
  int minage, maxage;
  char c;
  double minlength, maxlength, dl;

  keeper->addString("initialcond");

  infile >> ws;
  c = infile.peek();
  if ((c == 'n') || (c == 'N'))
    infile >> text >> ws; //read in the word 'numbers' if it exists

  readWordAndVariable(infile, "minage", minage);
  readWordAndVariable(infile, "maxage", maxage);
  readWordAndVariable(infile, "minlength", minlength);
  readWordAndVariable(infile, "maxlength", maxlength);
  int noagegr = maxage - minage + 1;

  //JMB - changed to make the reading of dl optional
  //if it isnt specifed here, it will default to the dl value of the stock
  infile >> ws;
  c = infile.peek();
  if ((c == 'd') || (c == 'D'))
    readWordAndVariable(infile, "dl", dl);
  else
    dl = DL;

  LgrpDiv = new LengthGroupDivision(minlength, maxlength, dl);
  if (LgrpDiv->Error())
    handle.logMessage(LOGFAIL, "Error in initial conditions - failed to create length group");

  //read the standard deviation multiplier - default to 1.0
  keeper->addString("sdevmultiplier");
  infile >> ws;
  c = infile.peek();
  if ((c == 's') || (c == 'S'))
    readWordAndVariable(infile, "sdev", sdevMult);
  else
    sdevMult.setValue(1.0);
  sdevMult.Inform(keeper);
  keeper->clearLast();

  //create the initialPop object of the correct size
  PopInfo tmppop;
  tmppop.N = 1.0;
  PopInfoMatrix popmatrix(noagegr, LgrpDiv->numLengthGroups(), tmppop);
  initialPop.resize(areas.Size(), minage, 0, popmatrix);

  infile >> text >> ws;
  if ((strcasecmp(text, "initstockfile") == 0) || (strcasecmp(text, "normalcondfile") == 0)) {
    //read initial data in mean length format, using the reference weight file
    readoption = 0;
    infile >> text >> ws;
    subfile.open(text, ios::in);
    handle.checkIfFailure(subfile, text);
    handle.Open(text);
    this->readNormalConditionData(subcomment, keeper, noagegr, minage, Area);
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

  } else if ((strcasecmp(text, "normalparamfile") == 0)) {
    //read initial data in mean length format, using a length weight relationship
    readoption = 1;
    infile >> text >> ws;
    subfile.open(text, ios::in);
    handle.checkIfFailure(subfile, text);
    handle.Open(text);
    this->readNormalParameterData(subcomment, keeper, noagegr, minage, Area);
    handle.Close();
    subfile.close();
    subfile.clear();

  } else if ((strcasecmp(text, "numberfile") == 0)) {
    //read initial data in number format
    readoption = 2;
    infile >> text >> ws;
    subfile.open(text, ios::in);
    handle.checkIfFailure(subfile, text);
    handle.Open(text);
    this->readNumberData(subcomment, keeper, noagegr, minage, Area);
    handle.Close();
    subfile.close();
    subfile.clear();

  } else
    handle.logFileMessage(LOGFAIL, "unrecognised initial conditions format", text);

  keeper->clearLast();
}

InitialCond::~InitialCond() {
  int i;
  delete LgrpDiv;
  delete CI;
  for (i = 0; i < initialNumber.Size(); i++)
    delete initialNumber[i];
}

void InitialCond::setCI(const LengthGroupDivision* const GivenLDiv) {
  //check minimum and maximum lengths
  if (LgrpDiv->minLength() < GivenLDiv->minLength())
    handle.logMessage(LOGWARN, "Warning in initial conditions - minimum length less than stock length");
  if (LgrpDiv->maxLength() > GivenLDiv->maxLength())
    handle.logMessage(LOGWARN, "Warning in initial conditions - maximum length greater than stock length");
  CI = new ConversionIndex(LgrpDiv, GivenLDiv);
  if (CI->Error())
    handle.logMessage(LOGFAIL, "Error in initial conditions - error when checking length structure");
}

void InitialCond::Print(ofstream& outfile) const {
  int i, j;

  outfile << "\nInitial conditions\n";
  for (i = 0; i < areas.Size(); i++) {
    outfile << "\tInternal area " << areas[i] << endl;
    initialPop[i].printNumbers(outfile);
  }

  if ((readoption == 0) || (readoption == 1)) {
    //JMB print the multipliers that are used to scale the population
    outfile << "\tArea multipliers used to scale the population\n";
    for (i = 0; i < areaFactor.Nrow(); i++) {
      for (j = 0; j < areaFactor.Ncol(i); j++)
        outfile << TAB << areaFactor[i][j];
      outfile << endl;
    }
    outfile << "\tAge multipliers used to scale the population\n";
    for (i = 0; i < ageFactor.Nrow(); i++) {
      for (j = 0; j < ageFactor.Ncol(i); j++)
        outfile << TAB << ageFactor[i][j];
      outfile << endl;
    }
  }

  outfile << endl;
  outfile.flush();
}

void InitialCond::Initialise(AgeBandMatrixPtrVector& Alkeys) {

  int area, age, l;
  int minage, maxage;
  double mult, scaler, dnorm;

  if (readoption == 0) {
    if (isZero(sdevMult))  //JMB this should never happen ...
      handle.logMessage(LOGFAIL, "Error in initial conditions - multiplier for standard deviation is zero");

    for (area = 0; area < areas.Size(); area++) {
      minage = initialPop[area].minAge();
      maxage = initialPop[area].maxAge();
      for (age = minage; age <= maxage; age++) {

        //JMB check that the length data is valid
        if (isZero(meanLength[area][age - minage] ) || isZero(sdevLength[area][age - minage])) {
          handle.logMessage(LOGWARN, "Warning in initial conditions - invalid length data");

          //JMB set the population to zero
          for (l = initialPop[area].minLength(age); l < initialPop[area].maxLength(age); l++)
            initialPop[area][age][l].setToZero();

        } else {
          //JMB check that the mean length is greater than the minimum length
          if (meanLength[area][age - minage] < LgrpDiv->minLength())
            handle.logMessage(LOGWARN, "Warning in initial conditions - mean length is less than minimum length");

          scaler = 0.0;
          mult = 1.0 / (sdevLength[area][age - minage] * sdevMult);
          for (l = initialPop[area].minLength(age); l < initialPop[area].maxLength(age); l++) {
            dnorm = (LgrpDiv->meanLength(l) - meanLength[area][age - minage]) * mult;
            initialPop[area][age][l].N = exp(-(dnorm * dnorm) * 0.5);
            scaler += initialPop[area][age][l].N;
          }

          if (!isZero(scaler)) {
            scaler = 10000.0 / scaler;
            for (l = initialPop[area].minLength(age); l < initialPop[area].maxLength(age); l++) {
              initialPop[area][age][l].N *= scaler;
              initialPop[area][age][l].W = refWeight[l] * relCond[area][age - minage];
              if ((handle.getLogLevel() >= LOGWARN) && (isZero(initialPop[area][age][l].W)) && (initialPop[area][age][l].N > 0.0))
                handle.logMessage(LOGWARN, "Warning in initial conditions - zero mean weight");
            }
          }
        }
      }
    }

  } else if (readoption == 1) {
    if (isZero(sdevMult))  //JMB this should never happen ...
      handle.logMessage(LOGFAIL, "Error in initial conditions - multiplier for standard deviation is zero");

    for (area = 0; area < areas.Size(); area++) {
      minage = initialPop[area].minAge();
      maxage = initialPop[area].maxAge();
      for (age = minage; age <= maxage; age++) {

        //JMB check that the length data is valid
        if (isZero(meanLength[area][age - minage] ) || isZero(sdevLength[area][age - minage])) {
          handle.logMessage(LOGWARN, "Warning in initial conditions - invalid length data");

          //JMB set the population to zero
          for (l = initialPop[area].minLength(age); l < initialPop[area].maxLength(age); l++)
            initialPop[area][age][l].setToZero();

        } else {
          //JMB check that the mean length is greater than the minimum length
          if (meanLength[area][age - minage] < LgrpDiv->minLength())
            handle.logMessage(LOGWARN, "Warning in initial conditions - mean length is less than minimum length");

          scaler = 0.0;
          mult = 1.0 / (sdevLength[area][age - minage] * sdevMult);
          for (l = initialPop[area].minLength(age); l < initialPop[area].maxLength(age); l++) {
            dnorm = (LgrpDiv->meanLength(l) - meanLength[area][age - minage]) * mult;
            initialPop[area][age][l].N = exp(-(dnorm * dnorm) * 0.5);
            scaler += initialPop[area][age][l].N;
          }

          if (!isZero(scaler)) {
            scaler = 10000.0 / scaler;
            for (l = initialPop[area].minLength(age); l < initialPop[area].maxLength(age); l++) {
              initialPop[area][age][l].N *= scaler;
              initialPop[area][age][l].W = alpha[area][age - minage] * pow(LgrpDiv->meanLength(l), beta[area][age - minage]);
              if ((handle.getLogLevel() >= LOGWARN) && (isZero(initialPop[area][age][l].W)) && (initialPop[area][age][l].N > 0.0))
                handle.logMessage(LOGWARN, "Warning in initial conditions - zero mean weight");
            }
          }
        }
      }
    }

  } else if (readoption == 2) {
    for (area = 0; area < areas.Size(); area++) {
      minage = initialPop[area].minAge();
      maxage = initialPop[area].maxAge();
      for (age = minage; age <= maxage; age++) {
        for (l = initialPop[area].minLength(age); l < initialPop[area].maxLength(age); l++) {
          initialPop[area][age][l].N = (*initialNumber[area])[age - minage][l];
          if (handle.getLogLevel() >= LOGWARN) {
            if (initialPop[area][age][l].N < 0.0)
              handle.logMessage(LOGWARN, "Warning in initial conditions - negative initial population");
            if ((isZero(initialPop[area][age][l].W)) && (initialPop[area][age][l].N > 0.0))
              handle.logMessage(LOGWARN, "Warning in initial conditions - zero mean weight");
          }
        }
      }
    }

  } else
    handle.logMessage(LOGFAIL, "Error in initial conditions - unrecognised data format");

  mult = 1.0;
  for (area = 0; area < areas.Size(); area++) {
    Alkeys[area].setToZero();
    //check minimum and maximum ages
    if (handle.getLogLevel() >= LOGWARN) {
      if (initialPop[area].minAge() < Alkeys[area].minAge())
        handle.logMessage(LOGWARN, "Warning in initial conditions - minimum age less than stock age");
      if (initialPop[area].maxAge() > Alkeys[area].maxAge())
        handle.logMessage(LOGWARN, "Warning in initial conditions - maximum age greater than stock age");
    }

    minage = max(Alkeys[area].minAge(), initialPop[area].minAge());
    maxage = min(Alkeys[area].maxAge(), initialPop[area].maxAge());
    if (maxage < minage)
      handle.logMessage(LOGFAIL, "Error in initial conditions - maximum age less than minimum age");

    for (age = minage; age <= maxage; age++) {
      if ((readoption == 0) || (readoption == 1))
        mult = areaFactor[area][age - minage] * ageFactor[area][age - minage];

      if (mult < 0.0) {
        handle.logMessage(LOGWARN, "Warning in initial conditions - negative stock multiplier", mult);
        mult = -mult;
      }

      Alkeys[area][age].Add(initialPop[area][age], *CI, mult);
    }
  }
}
