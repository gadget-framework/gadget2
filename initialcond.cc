#include "initialcond.h"
#include "errorhandler.h"
#include "readfunc.h"
#include "readword.h"
#include "mathfunc.h"
#include "gadget.h"

extern ErrorHandler handle;

void InitialCond::readNormalConditionData(CommentStream& infile, Keeper* const keeper,
  int noagegr, int minage, const AreaClass* const Area) {

  //Find start of data in datafile
  infile >> ws;
  if (infile.eof())
    handle.Message("Error in initial conditions - initial stock data file empty");

  char c = infile.peek();
  if (!isdigit(c)) {
    infile.get(c);
    while (c != '\n' && !infile.eof())
      infile.get(c);
  }

  int noareas = areas.Size();
  int i, age, area, ageid, areaid, tmparea, keepdata;
  int count = 0;
  Formula number;
  char tmpnumber[MaxStrLength];
  strncpy(tmpnumber, "", MaxStrLength);

  //Resize the matrices to hold the data
  areaFactor.AddRows(noareas, noagegr, number);
  ageFactor.AddRows(noareas, noagegr, number);
  meanLength.AddRows(noareas, noagegr, number);
  sdevLength.AddRows(noareas, noagegr, number);
  relCond.AddRows(noareas, noagegr, number);

  //Found the start of the data in the following format
  //age - area - agedist - areadist - meanlen - standdev - relcond
  if (countColumns(infile) != 7)
    handle.Message("Wrong number of columns in inputfile - should be 7");

  ageid = -1;
  tmparea = -1;
  keeper->addString("meandata");
  while (!infile.eof()) {
    keepdata = 0;
    infile >> age >> area >> ws;

    //crude age data check - perhaps there should be a better check?
    if ((age < minage) || (age >= (noagegr + minage))) {
      handle.Warning("Ignoring initial conditions data found outside age range");
      keepdata = 1;
    } else
      ageid = age - minage;

    //crude area data check
    areaid = -1;
    tmparea = Area->InnerArea(area);
    if (tmparea == -1)
      handle.UndefinedArea(area);
    for (i = 0; i < noareas; i++)
      if (areas[i] == tmparea)
        areaid = i;

    if (areaid == -1) {
      handle.Warning("Ignoring initial conditions data found outside area range");
      keepdata = 1;
    }

    if (keepdata == 0) {
      //initial data is required, so store it
      count++;
      infile >> ageFactor[areaid][ageid] >> ws;
      infile >> areaFactor[areaid][ageid] >> ws;
      infile >> meanLength[areaid][ageid] >> ws;
      infile >> sdevLength[areaid][ageid] >> ws;
      infile >> relCond[areaid][ageid] >> ws;

    } else {
      //initial data is not required, so read but ignore it
      infile >> tmpnumber >> ws;
      infile >> tmpnumber >> ws;
      infile >> tmpnumber >> ws;
      infile >> tmpnumber >> ws;
      infile >> tmpnumber >> ws;
    }
  }

  handle.logMessage("Read initial conditions data file - number of entries", count);
  areaFactor.Inform(keeper);
  ageFactor.Inform(keeper);
  meanLength.Inform(keeper);
  sdevLength.Inform(keeper);
  relCond.Inform(keeper);
  keeper->clearLast();
}

void InitialCond::readNormalParameterData(CommentStream& infile, Keeper* const keeper,
  int noagegr, int minage, const AreaClass* const Area) {

  //Find start of data in datafile
  infile >> ws;
  if (infile.eof())
    handle.Message("Error in initial conditions - initial stock data file empty");

  char c = infile.peek();
  if (!isdigit(c)) {
    infile.get(c);
    while (c != '\n' && !infile.eof())
      infile.get(c);
  }

  int noareas = areas.Size();
  int i, age, area, ageid, areaid, tmparea, keepdata;
  int count = 0;
  Formula number;
  char tmpnumber[MaxStrLength];
  strncpy(tmpnumber, "", MaxStrLength);

  //Resize the matrices to hold the data
  areaFactor.AddRows(noareas, noagegr, number);
  ageFactor.AddRows(noareas, noagegr, number);
  meanLength.AddRows(noareas, noagegr, number);
  sdevLength.AddRows(noareas, noagegr, number);
  alpha.AddRows(noareas, noagegr, number);
  beta.AddRows(noareas, noagegr, number);

  //Found the start of the data in the following format
  //age - area - agedist - areadist - meanlen - standdev - alpha - beta
  if (countColumns(infile) != 8)
    handle.Message("Wrong number of columns in inputfile - should be 8");

  ageid = -1;
  tmparea = -1;
  keeper->addString("meandata");
  while (!infile.eof()) {
    keepdata = 0;
    infile >> age >> area >> ws;

    //crude age data check - perhaps there should be a better check?
    if ((age < minage) || (age >= (noagegr + minage))) {
      handle.Warning("Ignoring initial conditions data found outside age range");
      keepdata = 1;
    } else
      ageid = age - minage;

    //crude area data check
    areaid = -1;
    tmparea = Area->InnerArea(area);
    if (tmparea == -1)
      handle.UndefinedArea(area);
    for (i = 0; i < noareas; i++)
      if (areas[i] == tmparea)
        areaid = i;

    if (areaid == -1) {
      handle.Warning("Ignoring initial conditions data found outside area range");
      keepdata = 1;
    }

    if (keepdata == 0) {
      //initial data is required, so store it
      count++;
      infile >> ageFactor[areaid][ageid] >> ws;
      infile >> areaFactor[areaid][ageid] >> ws;
      infile >> meanLength[areaid][ageid] >> ws;
      infile >> sdevLength[areaid][ageid] >> ws;
      infile >> alpha[areaid][ageid] >> ws;
      infile >> beta[areaid][ageid] >> ws;

    } else {
      //initial data is not required, so read but ignore it
      infile >> tmpnumber >> ws;
      infile >> tmpnumber >> ws;
      infile >> tmpnumber >> ws;
      infile >> tmpnumber >> ws;
      infile >> tmpnumber >> ws;
      infile >> tmpnumber >> ws;
    }
  }

  handle.logMessage("Read initial conditions data file - number of entries", count);
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

  //Find start of data in datafile
  infile >> ws;
  if (infile.eof())
    handle.Message("Error in initial conditions - initial stock data file empty");

  char c = infile.peek();
  if (!isdigit(c)) {
    infile.get(c);
    while (c != '\n' && !infile.eof())
      infile.get(c);
  }

  //Found the start of the data in the following format
  //area - age - meanlength - number - weight
  int i, age, area, tmparea;
  int keepdata, ageid, areaid, lengthid;
  double length, tmpnumber, tmpweight;
  int count = 0;
  int noareas = areas.Size();
  int nolengr = LgrpDiv->numLengthGroups();

  if (countColumns(infile) != 5)
    handle.Message("Wrong number of columns in inputfile - should be 5");

  //set the numbers in the AgeBandMatrixPtrVector to zero (in case some arent in the inputfile)
  for (areaid = 0; areaid < noareas; areaid++)
    for (ageid = minage; ageid < noagegr + minage; ageid++)
      for (lengthid = 0; lengthid < nolengr; lengthid++) {
        initialPop[areaid][ageid][lengthid].N = 0.0;
        initialPop[areaid][ageid][lengthid].W = 0.0;
      }

  ageid = -1;
  areaid = -1;
  lengthid = -1;
  keeper->addString("numberdata");
  while (!infile.eof()) {
    keepdata = 0;
    infile >> area >> age >> length >> tmpnumber >> tmpweight >> ws;

    //crude age data check - perhaps there should be a better check?
    if ((age < minage) || (age >= (noagegr + minage))) {
      handle.Warning("Ignoring initial conditions data found outside age range");
      keepdata = 1;
    } else
      ageid = age;

    //crude length data check
    lengthid = -1;
    for (i = 0; i < nolengr; i++)
      if (length == LgrpDiv->minLength(i))
        lengthid = i;

    if (lengthid == -1) {
      handle.Warning("Ignoring initial conditions data found outside length range");
      keepdata = 1;
    }

    //crude area data check
    areaid = -1;
    tmparea = Area->InnerArea(area);
    if (tmparea == -1)
      handle.UndefinedArea(area);
    for (i = 0; i < noareas; i++)
      if (areas[i] == tmparea)
        areaid = i;

    if (areaid == -1) {
      handle.Warning("Ignoring initial conditions data found outside area range");
      keepdata = 1;
    }

    if (keepdata == 0) {
      //initial data is required, so store it
      count++;
      if ((isZero(tmpweight)) && (!(isZero(tmpnumber))))
        handle.Warning("Warning in initial conditions - zero mean weight");
      initialPop[areaid][ageid][lengthid].N = tmpnumber;
      initialPop[areaid][ageid][lengthid].W = tmpweight;
    }
  }
  handle.logMessage("Read initial conditions data file - number of entries", count);
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
    handle.Message("Error in initial conditions - failed to create length group");

  //read the standard deviation multiplier - default to 1.0
  keeper->addString("sdevmultiplier");
  infile >> ws;
  c = infile.peek();
  if ((c == 's') || (c == 'S'))
    readWordAndFormula(infile, "sdev", sdevMult);
  else
    sdevMult.setValue(1.0);
  sdevMult.Inform(keeper);
  keeper->clearLast();

  //default values
  readoption = 0;

  //create the initialPop object of the correct size
  int noagegr = maxage - minage + 1;
  PopInfoMatrix tmpPop(noagegr, LgrpDiv->numLengthGroups());
  for (i = 0; i < noagegr; i++) {
    for (j = 0; j < LgrpDiv->numLengthGroups(); j++) {
      //these values for tmpPop[i][j] must not be zero - set them to 1
      tmpPop[i][j].N = 1.0;
      tmpPop[i][j].W = 1.0;
    }
  }
  initialPop.resize(areas.Size(), minage, 0, tmpPop);

  keeper->addString("readinitialdata");
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
    handle.Unexpected("normalcondfile, normalparamfile or numberfile", text);

  keeper->clearLast();

  if ((readoption == 0) || (readoption == 2)) {
    //read information on reference weights.
    DoubleMatrix tmpRefW;
    keeper->addString("referenceweights");
    ifstream subweightfile;
    subweightfile.open(refWeightFile, ios::in);
    handle.checkIfFailure(subweightfile, refWeightFile);
    handle.Open(refWeightFile);
    CommentStream subweightcomment(subweightfile);
    if (!readRefWeights(subweightcomment, tmpRefW))
      handle.Message("Wrong format for reference weights");
    handle.Close();
    subweightfile.close();
    subweightfile.clear();

    //Interpolate the reference weights. First there are some error checks.
    if (LgrpDiv->meanLength(0) < tmpRefW[0][0] ||
        LgrpDiv->meanLength(LgrpDiv->numLengthGroups() - 1) > tmpRefW[tmpRefW.Nrow() - 1][0])
      handle.Message("Lengths for reference weights must span the range of initial condition lengths");

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
  }

  keeper->clearLast();
}

InitialCond::~InitialCond() {
  delete LgrpDiv;
  delete CI;
}

void InitialCond::setCI(const LengthGroupDivision* const GivenLDiv) {
  CI = new ConversionIndex(LgrpDiv, GivenLDiv);
}

void InitialCond::Print(ofstream& outfile) const {
  int i, j;

  outfile << "\nInitial conditions\n";
  for (i = 0; i < areas.Size(); i++) {
    outfile << "\tInternal area " << areas[i] << endl;
    initialPop[i].printNumbers(outfile);
  }
  outfile << "\tMean lengths\n";
  for (i = 0; i < meanLength.Nrow(); i++) {
    for (j = 0; j < meanLength.Ncol(i); j++)
      outfile << TAB << meanLength[i][j];
    outfile << endl;
  }
  outfile << "\tStandard deviation\n";
  for (i = 0; i < sdevLength.Nrow(); i++) {
    for (j = 0; j < sdevLength.Ncol(i); j++)
      outfile << TAB << sdevLength[i][j];
    outfile << endl;
  }
  outfile << endl;
  outfile.flush();
}

void InitialCond::Initialise(AgeBandMatrixPtrVector& Alkeys) {

  int area, age, l;
  int minage, maxage;
  double mult, scaler, dnorm;

  if (readoption == 0) {
   for (area = 0; area < initialPop.Size(); area++) {
      minage = initialPop[area].minAge();
      maxage = initialPop[area].maxAge();
      for (age = minage; age <= maxage; age++) {
        scaler = 0.0;
        
        if (isZero(sdevLength[area][age - minage] * sdevMult))
          mult = 0.0;
        else        
          mult = 1.0 / (sdevLength[area][age - minage] * sdevMult);
  
        for (l = initialPop[area].minLength(age); l < initialPop[area].maxLength(age); l++) {
          dnorm = (LgrpDiv->meanLength(l) - meanLength[area][age - minage]) * mult;
          initialPop[area][age][l].N = exp(-(dnorm * dnorm) * 0.5);
          scaler += initialPop[area][age][l].N;
        }

        scaler = (scaler > rathersmall ? 10000.0 / scaler : 0.0);
        for (l = initialPop[area].minLength(age); l < initialPop[area].maxLength(age); l++) {
          initialPop[area][age][l].N *= scaler;
          initialPop[area][age][l].W = refWeight[l] * relCond[area][age - minage];
          if ((isZero(initialPop[area][age][l].W)) && (!(isZero(initialPop[area][age][l].N))))
            handle.logWarning("Warning in initial conditions - zero mean weight");
        }
      }
    }

  } else if (readoption == 1) {
    for (area = 0; area < initialPop.Size(); area++) {
      minage = initialPop[area].minAge();
      maxage = initialPop[area].maxAge();
      for (age = minage; age <= maxage; age++) {
        scaler = 0.0;
        
        if (isZero(sdevLength[area][age - minage] * sdevMult))
          mult = 0.0;
        else        
          mult = 1.0 / (sdevLength[area][age - minage] * sdevMult);
  
        for (l = initialPop[area].minLength(age); l < initialPop[area].maxLength(age); l++) {
          dnorm = (LgrpDiv->meanLength(l) - meanLength[area][age - minage]) * mult;
          initialPop[area][age][l].N = exp(-(dnorm * dnorm) * 0.5);
          scaler += initialPop[area][age][l].N;
        }

        scaler = (scaler > rathersmall ? 10000.0 / scaler : 0.0);
        for (l = initialPop[area].minLength(age); l < initialPop[area].maxLength(age); l++) {
          initialPop[area][age][l].N *= scaler;
          initialPop[area][age][l].W = alpha[area][age - minage] * pow(LgrpDiv->meanLength(l), beta[area][age - minage]);
          if ((isZero(initialPop[area][age][l].W)) && (!(isZero(initialPop[area][age][l].N))))
            handle.logWarning("Warning in initial conditions - zero mean weight");
        }
      }
    }

  } else if (readoption == 2) {
    //nothing to be done here

  } else
    handle.logFailure("Error in initial conditions - unrecognised data format");

  mult = 1.0;
  for (area = 0; area < areas.Size(); area++) {
    Alkeys[area].setToZero();
    minage = max(Alkeys[area].minAge(), initialPop[area].minAge());
    maxage = min(Alkeys[area].maxAge(), initialPop[area].maxAge());

    if (maxage < minage)
      return;

    for (age = minage; age <= maxage; age++) {
      if ((readoption == 0) || (readoption == 1))
        mult = areaFactor[area][age - minage] * ageFactor[area][age - minage];

      if (mult < 0) {
        handle.logWarning("Warning in initial conditions - negative stock multiplier", mult);
        mult = -mult;
      }

      Alkeys[area][age].Add(initialPop[area][age], *CI, mult);
    }
  }
}
