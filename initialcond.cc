#include "initialcond.h"
#include "errorhandler.h"
#include "readfunc.h"
#include "print.h"
#include "readword.h"
#include "mathfunc.h"
#include "gadget.h"

/* JMB - I've removed all the code to deal with the basis functions
 * This code is still available from the tarfile for gadget verion 2.0.02
 * However, it will need some thinking to get it to work again though */

//
// A function to read initial stock data in normal distribution format
//
void InitialCond::ReadNormalData(CommentStream& infile, Keeper* const keeper,
  int noagegr, int minage, const AreaClass* const Area) {

  ErrorHandler handle;
  //Find start of data in datafile
  infile >> ws;
  if (infile.eof()) {
    handle.Message("Initial stock data file empty");
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
  Formula number;
  char tmpnumber[MaxStrLength];
  strncpy(tmpnumber, "", MaxStrLength);

  //Resize the matrices to hold the data
  AreaDist.AddRows(noareas, noagegr, number);
  AgeDist.AddRows(noareas, noagegr, number);
  Mean.AddRows(noareas, noagegr, number);
  Sdev.AddRows(noareas, noagegr, number);
  RelCondition.AddRows(noareas, noagegr, number);

  //Found the start of the data in the following format
  //age - area - agedist - areadist - meanlen - standdev - relcond
  if (countColumns(infile) != 7)
    handle.Message("Wrong number of columns in inputfile - should be 7");

  ageid = -1;
  tmparea = -1;
  keeper->AddString("meandata");
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
      infile >> AgeDist[areaid][ageid] >> ws;
      infile >> AreaDist[areaid][ageid] >> ws;
      infile >> Mean[areaid][ageid] >> ws;
      infile >> Sdev[areaid][ageid] >> ws;
      infile >> RelCondition[areaid][ageid] >> ws;

    } else {
      //initial data is not required, so read but ignore it
      infile >> tmpnumber >> ws;
      infile >> tmpnumber >> ws;
      infile >> tmpnumber >> ws;
      infile >> tmpnumber >> ws;
      infile >> tmpnumber >> ws;
    }
  }

  AreaDist.Inform(keeper);
  AgeDist.Inform(keeper);
  Mean.Inform(keeper);
  Sdev.Inform(keeper);
  RelCondition.Inform(keeper);
  keeper->ClearLast();
}

//
// A function to read initial stock data in number data format
//
void InitialCond::ReadNumberData(CommentStream& infile, Keeper* const keeper,
  int noagegr, int minage, const AreaClass* const Area) {

  ErrorHandler handle;
  //Find start of data in datafile
  infile >> ws;
  if (infile.eof()) {
    handle.Message("Initial stock number data file empty");
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
  int noareas = areas.Size();
  int nolengr = LgrpDiv->NoLengthGroups();

  if (countColumns(infile) != 4)
    handle.Message("Wrong number of columns in inputfile - should be 4");

  //Set the numbers in the AgeBandMatrixPtrVector to zero (in case some arent in the inputfile)
  for (areaid = 0; areaid < noareas; areaid++)
    for (ageid = minage; ageid < noagegr + minage; ageid++)
      for (lengthid = 0; lengthid < nolengr; lengthid++)
        AreaAgeLength[areaid][ageid][lengthid].N = 0.0;

  ageid = -1;
  areaid = -1;
  lengthid = -1;
  keeper->AddString("numberdata");
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
      if (length == LgrpDiv->Minlength(i))
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
      AreaAgeLength[areaid][ageid][lengthid].N = tmpnumber;
    }
  }
  keeper->ClearLast();
}

//
// Constructor for the initial conditions
//
InitialCond::InitialCond(CommentStream& infile, const IntVector& Areas,
  Keeper* const keeper, const char* refWeightFile, const AreaClass* const Area)
  : LivesOnAreas(Areas), LgrpDiv(0), CI(0) {

  ErrorHandler handle;
  ifstream subfile;
  CommentStream subcomment(subfile);
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);

  int i, j, k;
  int noareas = areas.Size();
  int minage, maxage;
  double minlength, maxlength, dl;

  keeper->AddString("initialcond");
  infile >> text >> ws;  //read in 'numbers'
  readWordAndVariable(infile, "minage", minage);
  readWordAndVariable(infile, "maxage", maxage);
  readWordAndVariable(infile, "minlength", minlength);
  readWordAndVariable(infile, "maxlength", maxlength);
  readWordAndVariable(infile, "dl", dl);

  LgrpDiv = new LengthGroupDivision(minlength, maxlength, dl);
  if (LgrpDiv->Error())
    printLengthGroupError(minlength, maxlength, dl, "length groups for initial condition of stock");

  int noagegr = maxage - minage + 1; //Number of age groups
  int nolengr = LgrpDiv->NoLengthGroups(); //Number of length groups

  //Read the standard deviation multiplier - default to 1.0
  keeper->AddString("sdevmultiplier");
  infile >> text >> ws;
  if ((strcasecmp(text, "sdev") == 0))
    infile >> SdevMult >> ws >> text >> ws;
  else
    SdevMult.setValue(1.0);
  SdevMult.Inform(keeper);
  keeper->ClearLast();

  //default values
  readNumbers = 0;

  //create the AreaAgeLength object of the correct size
  PopInfoMatrix tmpPop(noagegr, nolengr);
  for (i = 0; i < noagegr; i++) {
    for (j = 0; j < nolengr; j++) {
      //these values for tmpPop[i][j] must not be zero - set them to 1
      tmpPop[i][j].N = 1.0;
      tmpPop[i][j].W = 1.0;
    }
  }
  AreaAgeLength.resize(noareas, minage, 0, tmpPop);

  keeper->AddString("readinitialdata");
  if ((strcasecmp(text, "initstockfile") == 0)) {
    //read initial data in mean length format
    infile >> text >> ws;
    subfile.open(text, ios::in);
    checkIfFailure(subfile, text);
    handle.Open(text);
    this->ReadNormalData(subcomment, keeper, noagegr, minage, Area);
    handle.Close();
    subfile.close();
    subfile.clear();

  } else if ((strcasecmp(text, "numberfile") == 0)) {
    readNumbers = 1;
    //read initial data in number format
    infile >> text >> ws;
    subfile.open(text, ios::in);
    checkIfFailure(subfile, text);
    handle.Open(text);
    this->ReadNumberData(subcomment, keeper, noagegr, minage, Area);
    handle.Close();
    subfile.close();
    subfile.clear();

  } else {
    handle.Unexpected("type of datafile", text);
  }
  keeper->ClearLast();

  //Now that the initial conditions have been read from file
  //we have to set the initial weight of the stock
  double rCond, rWeight;
  DoubleMatrix tmpRefW;

  //Read information on reference weights.
  keeper->AddString("referenceweights");
  ifstream subweightfile;
  subweightfile.open(refWeightFile, ios::in);
  checkIfFailure(subweightfile, refWeightFile);
  handle.Open(refWeightFile);
  CommentStream subweightcomment(subweightfile);
  if (!read2ColVector(subweightcomment, tmpRefW))
    handle.Message("Wrong format for reference weights");
  handle.Close();
  subweightfile.close();
  subweightfile.clear();

  //Interpolate the reference weights. First there are some error checks.
  for (i = 0; i < tmpRefW.Nrow() - 1; i++)
    if ((tmpRefW[i + 1][0] - tmpRefW[i][0]) <= 0)
      handle.Message("Lengths for reference weights must be strictly increasing");

  if (LgrpDiv->Meanlength(0) < tmpRefW[0][0] ||
      LgrpDiv->Meanlength(LgrpDiv->NoLengthGroups() - 1) > tmpRefW[tmpRefW.Nrow() - 1][0])
    handle.Message("Lengths for reference weights must span the range of initial condition lengths");

  //Aggregate the reference weight data to be the same format
  DoubleVector Wref(nolengr);
  int pos = 0;
  for (j = 0; j < nolengr; j++) {
    for (i = pos; i < tmpRefW.Nrow() - 1; i++) {
      if (LgrpDiv->Meanlength(j) >= tmpRefW[i][0] && LgrpDiv->Meanlength(j) <= tmpRefW[i + 1][0]) {
        rWeight = (LgrpDiv->Meanlength(j) - tmpRefW[i][0]) / (tmpRefW[i + 1][0] - tmpRefW[i][0]);
        Wref[j] = tmpRefW[i][1] + rWeight * (tmpRefW[i + 1][1] - tmpRefW[i][1]);
        pos = i;
      }
    }
  }
  keeper->ClearLast();

  //finaly copy the information into AreaAgeLength
  keeper->AddString("weightdata");
  for (i = 0; i < noareas; i++) {
    for (j = 0; j < noagegr; j++) {
      if (readNumbers == 1)
        rCond = 1.0;
      else
        rCond = RelCondition[i][j];

      for (k = 0; k < nolengr; k++) {
        rWeight = (Wref[k] * rCond);

        if (isZero(rWeight)) {
          cerr << "Possible error: zero mean weight for the stock for the following\n"
            << "Area: " << i << " Age group: " << j << " Length group: " << k << endl;
        }

        //for AreaAgeLength the age is taken from the minimum age
        AreaAgeLength[i][j + minage][k].W = rWeight;
      }
    }
  }

  keeper->ClearLast();
  keeper->ClearLast();
}

InitialCond::~InitialCond() {
  delete LgrpDiv;
  delete CI;
}

void InitialCond::SetCI(const LengthGroupDivision* const GivenLDiv) {
  CI = new ConversionIndex(LgrpDiv, GivenLDiv);
}

void InitialCond::Print(ofstream& outfile) const {
  int i, j;

  outfile << "\nInitial conditions\n";
  for (i = 0; i < areas.Size(); i++) {
    outfile << "\tInternal area " << areas[i] << endl;
    Printagebandm(outfile, AreaAgeLength[i]);
  }
  outfile << "\tMean lengths\n";
  for (i = 0; i < Mean.Nrow(); i++) {
    for (j = 0; j < Mean.Ncol(i); j++)
      outfile << TAB << Mean[i][j];
    outfile << endl;
  }
  outfile << "\tStandard deviation\n";
  for (i = 0; i < Sdev.Nrow(); i++) {
    for (j = 0; j < Sdev.Ncol(i); j++)
      outfile << TAB << Sdev[i][j];
    outfile << endl;
  }
  outfile << endl;
  outfile.flush();
}

void InitialCond::Initialize(AgeBandMatrixPtrVector& Alkeys) {

  int area, age, l;
  int minage, maxage;
  double mult, scaler;

  if (readNumbers == 0) {
    for (area = 0; area < AreaAgeLength.Size(); area++) {
      minage = AreaAgeLength[area].Minage();
      maxage = AreaAgeLength[area].Maxage();
      for (age = minage; age <= maxage; age++) {
        scaler = 0.0;
        for (l = AreaAgeLength[area].Minlength(age);
            l < AreaAgeLength[area].Maxlength(age); l++) {
          AreaAgeLength[area][age][l].N =
            dnorm(LgrpDiv->Meanlength(l), Mean[area][age - minage], Sdev[area][age - minage] * SdevMult);
          scaler += AreaAgeLength[area][age][l].N;
        }

        scaler = (scaler > rathersmall ? 10000 / scaler : 0.0);
        for (l = AreaAgeLength[area].Minlength(age);
            l < AreaAgeLength[area].Maxlength(age); l++) {
          AreaAgeLength[area][age][l].N *= scaler;
        }
      }
    }
  }

  for (area = 0; area < areas.Size(); area++) {
    Alkeys[area].setToZero();
    minage = max(Alkeys[area].Minage(), AreaAgeLength[area].Minage());
    maxage = min(Alkeys[area].Maxage(), AreaAgeLength[area].Maxage());

    if (maxage < minage)
      return;

    for (age = minage; age <= maxage; age++) {
      if (readNumbers == 0)
        mult = AreaDist[area][age - minage] * AgeDist[area][age - minage];
      else
        mult = 1.0;

      if (mult < 0)
        mult = -mult;

      PopinfoAdd(Alkeys[area][age], AreaAgeLength[area][age], *CI, mult);
    }
  }
}
