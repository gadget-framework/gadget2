#include "initialcond.h"
#include "errorhandler.h"
#include "readfunc.h"
#include "readword.h"
#include "mathfunc.h"
#include "gadget.h"

extern ErrorHandler handle;

void InitialCond::readNormalData(CommentStream& infile, Keeper* const keeper,
  int noagegr, int minage, const AreaClass* const Area) {

  //Find start of data in datafile
  infile >> ws;
  if (infile.eof()) {
    handle.Message("Error in initial conditions - initial stock data file empty");
    return;
  }

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

    //crude data check - perhaps there should be a better check?
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

void InitialCond::readNumberData(CommentStream& infile, Keeper* const keeper,
  int noagegr, int minage, const AreaClass* const Area) {

  //Find start of data in datafile
  infile >> ws;
  if (infile.eof()) {
    handle.Message("Error in initial conditions - initial stock data file empty");
    return;
  }

  char c = infile.peek();
  if (!isdigit(c)) {
    infile.get(c);
    while (c != '\n' && !infile.eof())
      infile.get(c);
  }

  //Found the start of the data in the following format
  //area - age - meanlength - number
  int i, age, area, tmparea;
  int keepdata, ageid, areaid, lengthid;
  double length, tmpnumber;
  int count = 0;
  int noareas = areas.Size();
  int nolengr = LgrpDiv->numLengthGroups();

  if (countColumns(infile) != 4)
    handle.Message("Wrong number of columns in inputfile - should be 4");

  //set the numbers in the AgeBandMatrixPtrVector to zero (in case some arent in the inputfile)
  for (areaid = 0; areaid < noareas; areaid++)
    for (ageid = minage; ageid < noagegr + minage; ageid++)
      for (lengthid = 0; lengthid < nolengr; lengthid++)
        initialPop[areaid][ageid][lengthid].N = 0.0;

  ageid = -1;
  areaid = -1;
  lengthid = -1;
  keeper->addString("numberdata");
  while (!infile.eof()) {
    keepdata = 0;
    infile >> area >> age >> length >> tmpnumber >> ws;

    //crude data check - perhaps there should be a better check?
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
      initialPop[areaid][ageid][lengthid].N = tmpnumber;
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
    infile >> text >> ws; //read in 'numbers'

  readWordAndVariable(infile, "minage", minage);
  readWordAndVariable(infile, "maxage", maxage);
  readWordAndVariable(infile, "minlength", minlength);
  readWordAndVariable(infile, "maxlength", maxlength);

  //JMB - changed to make the reading of dl optional
  //If it isnt specifed here, it will default to the dl value of the stock
  infile >> ws;
  c = infile.peek();
  if ((c == 'd') || (c == 'D'))
    readWordAndVariable(infile, "dl", dl);
  else
    dl = DL;

  LgrpDiv = new LengthGroupDivision(minlength, maxlength, dl);
  if (LgrpDiv->Error())
    handle.Message("Error in initial conditions - failed to create length group");

  int noagegr = maxage - minage + 1; //Number of age groups
  int nolengr = LgrpDiv->numLengthGroups(); //Number of length groups

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
  readNumbers = 0;

  //create the initialPop object of the correct size
  PopInfoMatrix tmpPop(noagegr, nolengr);
  for (i = 0; i < noagegr; i++) {
    for (j = 0; j < nolengr; j++) {
      //these values for tmpPop[i][j] must not be zero - set them to 1
      tmpPop[i][j].N = 1.0;
      tmpPop[i][j].W = 1.0;
    }
  }
  initialPop.resize(areas.Size(), minage, 0, tmpPop);

  keeper->addString("readinitialdata");
  infile >> text >> ws;
  if ((strcasecmp(text, "initstockfile") == 0)) {
    //read initial data in mean length format
    infile >> text >> ws;
    subfile.open(text, ios::in);
    handle.checkIfFailure(subfile, text);
    handle.Open(text);
    this->readNormalData(subcomment, keeper, noagegr, minage, Area);
    handle.Close();
    subfile.close();
    subfile.clear();

  } else if ((strcasecmp(text, "numberfile") == 0)) {
    readNumbers = 1;
    //read initial data in number format
    infile >> text >> ws;
    subfile.open(text, ios::in);
    handle.checkIfFailure(subfile, text);
    handle.Open(text);
    this->readNumberData(subcomment, keeper, noagegr, minage, Area);
    handle.Close();
    subfile.close();
    subfile.clear();

  } else {
    handle.Unexpected("type of datafile", text);
  }
  keeper->clearLast();

  //Now that the initial conditions have been read from file
  //we have to set the initial weight of the stock
  double rCond, rWeight;
  DoubleMatrix tmpRefW;

  //read information on reference weights.
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
  DoubleVector Wref(nolengr);
  int pos = 0;
  for (j = 0; j < nolengr; j++) {
    for (i = pos; i < tmpRefW.Nrow() - 1; i++) {
      if (LgrpDiv->meanLength(j) >= tmpRefW[i][0] && LgrpDiv->meanLength(j) <= tmpRefW[i + 1][0]) {
        rWeight = (LgrpDiv->meanLength(j) - tmpRefW[i][0]) / (tmpRefW[i + 1][0] - tmpRefW[i][0]);
        Wref[j] = tmpRefW[i][1] + rWeight * (tmpRefW[i + 1][1] - tmpRefW[i][1]);
        pos = i;
      }
    }
  }
  keeper->clearLast();

  //finaly copy the information into initialPop
  for (i = 0; i < areas.Size(); i++) {
    for (j = 0; j < noagegr; j++) {
      if (readNumbers == 1)
        rCond = 1.0;
      else
        rCond = relCond[i][j];

      for (k = 0; k < nolengr; k++) {
        rWeight = (Wref[k] * rCond);

        if (isZero(rWeight))
          handle.logWarning("Warning in initial conditions - zero mean weight");

        //for initialPop the age is taken from the minimum age
        initialPop[i][j + minage][k].W = rWeight;
      }
    }
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

  if (readNumbers == 0) {
    for (area = 0; area < initialPop.Size(); area++) {
      minage = initialPop[area].minAge();
      maxage = initialPop[area].maxAge();
      for (age = minage; age <= maxage; age++) {
        scaler = 0.0;
        for (l = initialPop[area].minLength(age);
            l < initialPop[area].maxLength(age); l++) {
          dnorm = (LgrpDiv->meanLength(l) - meanLength[area][age - minage])
                  / (sdevLength[area][age - minage] * sdevMult);

          initialPop[area][age][l].N = exp(-(dnorm * dnorm) * 0.5);
          scaler += initialPop[area][age][l].N;
        }

        scaler = (scaler > rathersmall ? 10000 / scaler : 0.0);
        for (l = initialPop[area].minLength(age);
            l < initialPop[area].maxLength(age); l++) {
          initialPop[area][age][l].N *= scaler;
        }
      }
    }
  }

  for (area = 0; area < areas.Size(); area++) {
    Alkeys[area].setToZero();
    minage = max(Alkeys[area].minAge(), initialPop[area].minAge());
    maxage = min(Alkeys[area].maxAge(), initialPop[area].maxAge());

    if (maxage < minage)
      return;

    for (age = minage; age <= maxage; age++) {
      if (readNumbers == 0)
        mult = areaFactor[area][age - minage] * ageFactor[area][age - minage];
      else
        mult = 1.0;

      if (mult < 0) {
        handle.logWarning("Warning in initial conditions - negative stock multiplier", mult);
        mult = -mult;
      }

      Alkeys[area][age].Add(initialPop[area][age], *CI, mult);
    }
  }
}
