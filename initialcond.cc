#include "initialcond.h"
#include "errorhandler.h"
#include "readfunc.h"
#include "print.h"
#include "readword.h"
#include "conversion.h"
#include "extravector.h"
#include "gadget.h"

//
// A function to read initial stock data in mean data format
//
int InitialCond::ReadMeanData(CommentStream& infile, Keeper* const keeper,
  int noareas, int noagegr, int minage, const AreaClass* const Area) {

  ErrorHandler handle;
  //Find start of data in datafile
  infile >> ws;
  if (infile.eof()) {
    handle.Message("Initial stock data file empty");
    return 0;
  }

  char c = infile.peek();
  if (!isdigit(c)) {
    infile.get(c);
    while (c != '\n' && !infile.eof())
      infile.get(c);
  }

  int age, area, ageid, areaid, keepdata;
  Formula number;                       //initialised to zero
  char tmpnumber[MaxStrLength];
  strncpy(tmpnumber, "", MaxStrLength);

  //Resize the matrices to hold the data
  Distribution.AddRows(noareas, noagegr, number);
  Mean.AddRows(noareas, noagegr, number);
  Sdev.AddRows(noareas, noagegr, number);
  RelCondition.AddRows(noareas, noagegr, number);

  //Found the start of the data in the following format
  //age - area - distribution - meanlen - standdev - relcond
  if (!(CheckColumns(infile, 6)))
    handle.Message("Wrong number of columns in inputfile - should be 6");

  ageid = 0;
  areaid = 0;
  keeper->AddString("meandata");
  while (!infile.eof()) {
    keepdata = 0;
    infile >> age >> area >> ws;

    //Crude data check - perhaps there should be a better check?
    if (age > (noagegr + minage))
      keepdata = 1;
    else
      ageid = age - minage;

    if (keepdata == 0) {
      //initial data is required, so store it
      areaid = Area->InnerArea(area);
      if (areaid == -1)
        handle.UndefinedArea(area);

      infile >> Distribution[areaid][ageid] >> ws;
      infile >> Mean[areaid][ageid] >> ws;
      infile >> Sdev[areaid][ageid] >> ws;
      infile >> RelCondition[areaid][ageid] >> ws;

    } else {
      //initial data is not required, so read but ignore it
      infile >> tmpnumber >> ws;
      infile >> tmpnumber >> ws;
      infile >> tmpnumber >> ws;
      infile >> tmpnumber >> ws;
    }
  }

  Distribution.Inform(keeper);
  Mean.Inform(keeper);
  Sdev.Inform(keeper);
  RelCondition.Inform(keeper);
  keeper->ClearLast();
  return 1;
}

//
// A function to read initial stock distribution data
//
int InitialCond::ReadDistributionData(CommentStream& infile, Keeper* const keeper,
  int noareas, int noagegr, int minage, const AreaClass* const Area) {

  ErrorHandler handle;
  //Find start of data in datafile
  infile >> ws;
  if (infile.eof()) {
    handle.Message("Distribution stock data file empty");
    return 0;
  }

  char c = infile.peek();
  if (!isdigit(c)) {
    infile.get(c);
    while (c != '\n' && !infile.eof())
      infile.get(c);
  }

  int age, area, keepdata, ageid, areaid;
  Formula number;
  char tmpnumber[MaxStrLength];
  strncpy(tmpnumber, "", MaxStrLength);

  //Resize the matrices to hold the data
  Distribution.AddRows(noareas, noagegr, number);
  RelCondition.AddRows(noareas, noagegr, number);

  //Found the start of the data in the following format
  //age - area - distribution - relcond
  if (!(CheckColumns(infile, 4)))
    handle.Message("Wrong number of columns in inputfile - should be 4");

  ageid = 0;
  areaid = 0;
  keeper->AddString("distdata");
  while (!infile.eof()) {
    keepdata = 0;
    infile >> age >> area >> ws;

    //Crude data check - perhaps there should be a better check?
    if (age > (noagegr + minage))
      keepdata = 1;
    else
      ageid = age - minage;

    if (keepdata == 0) {
      //initial data is required, so store it
      areaid = Area->InnerArea(area);
      if (areaid == -1)
        handle.UndefinedArea(area);

      infile >> Distribution[areaid][ageid] >> ws;
      infile >> RelCondition[areaid][ageid] >> ws;

    } else {
      //initial data is not required, so read but ignore it
      infile >> tmpnumber >> ws;
      infile >> tmpnumber >> ws;
    }
  }

  Distribution.Inform(keeper);
  RelCondition.Inform(keeper);
  keeper->ClearLast();
  return 1;
}

//
// A function to read initial stock data in basis function format
// There is currently no data checking in this function - needs to be added
//
int InitialCond::ReadBasisData(CommentStream& infile, Keeper* const keeper,
  int numfunc, int noareas, int noagegr, const AreaClass* const Area) {

  ErrorHandler handle;
  int i, j;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);

  //Find start of data in datafile
  infile >> ws;
  if (infile.eof()) {
    handle.Message("Basis function data file empty");
    return 0;
  }

  infile >> text >> ws;
  if (strcasecmp(text, "[parameters]") != 0) {
    handle.Unexpected("[parameters]", text);
    return 0;
  }

  int age, area, basfn;
  Formula number;

  //Resize the matrices to hold the parameters
  MeanPar.AddRows(noareas, numfunc, number);
  SdevPar.AddRows(noareas, numfunc, number);
  MeanBasis.AddRows(noagegr, numfunc, 0.0);
  SdevBasis.AddRows(noagegr, numfunc, 0.0);

  //Found the start of the data in the following format
  //area - basisfn - mean - sdev
  if (!(CheckColumns(infile, 4)))
    handle.Message("Wrong number of columns in inputfile - should be 4");

  keeper->AddString("basisparam");
  for (i = 0; i < noareas; i++) {
    for (j = 0; j < numfunc; j++) {
      if (infile.eof()) {
        handle.Message("Unexpected end of basis function data file");
        return 0;
      }

      //Do we want to check the data here??
      infile >> area >> basfn >> MeanPar[i][j] >> SdevPar[i][j] >> ws;
    }
  }

  MeanPar.Inform(keeper);
  SdevPar.Inform(keeper);
  keeper->ClearLast();

  infile >> text >> ws;
  if (strcasecmp(text, "[basisfunction]") != 0) {
    handle.Unexpected("[basisfunction]", text);
    return 0;
  }

  //Found the start of the data in the following format
  //basisfn - age - mean - sdev
  if (!(CheckColumns(infile, 4)))
    handle.Message("Wrong number of columns in inputfile - should be 4");

  keeper->AddString("basisfunction");
  for (i = 0; i < noagegr; i++) {
    for (j = 0; j < numfunc; j++) {
      if (infile.eof()) {
        handle.Message("Unexpected end of basis function data file");
        return 0;
      }

      //Do we want to check the data here??
      infile >> basfn >> age >> MeanBasis[i][j] >> SdevBasis[i][j] >> ws;
    }
  }

  keeper->ClearLast();
  return 1;
}

//
// A function to read initial stock data in length basis function format
// There is currently no data checking in this function - needs to be added
//
int InitialCond::ReadLengthBasisData(CommentStream& infile, Keeper* const keeper,
  int numfunc, int noareas, int nolengr, const AreaClass* const Area) {

  ErrorHandler handle;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  int i, j;

  //Find start of data in datafile
  infile >> ws;
  if (infile.eof()) {
    handle.Message("Length basis function data file empty");
    return 0;
  }

  infile >> text >> ws;
  if (strcasecmp(text, "[parameters]") != 0) {
    handle.Unexpected("[parameters]", text);
    return 0;
  }

  int area, basfn;
  Formula number;
  double len;

  //Resize the matrices to hold the parameters
  LengthBasisPar.AddRows(noareas, numfunc, number);
  LengthBasis.AddRows(nolengr, numfunc, 0.0);

  //Found the start of the data in the following format
  //area - basisfn - lenbasispar
  if (!(CheckColumns(infile, 3)))
    handle.Message("Wrong number of columns in inputfile - should be 3");

  keeper->AddString("lenbasisparam");
  for (i = 0; i < noareas; i++) {
    for (j = 0; j < numfunc; j++) {
      if (infile.eof()) {
        handle.Message("Unexpected end of length basis function data file");
        return 0;
      }

      //Do we want to check the data here??
      infile >> area >> basfn >> LengthBasisPar[i][j] >> ws;
    }
  }

  LengthBasisPar.Inform(keeper);
  keeper->ClearLast();

  infile >> text >> ws;
  if (strcasecmp(text, "[basisfunction]") != 0) {
    handle.Unexpected("[basisfunction]", text);
    return 0;
  }

  //Found the start of the data in the following format
  //basisfn - age - lenbasis
  if (!(CheckColumns(infile, 3)))
    handle.Message("Wrong number of columns in inputfile - should be 3");

  keeper->AddString("lenbasisfunction");
  for (i = 0; i < nolengr; i++) {
    for (j = 0; j < numfunc; j++) {
      if (infile.eof()) {
        handle.Message("Unexpected end of length basis function data file");
        return 0;
      }

      //Do we want to check the data here??
      infile >> basfn >> len >> LengthBasis[i][j] >> ws;
    }
  }

  keeper->ClearLast();
  return 1;
}

//
// A function to read initial stock data in number data format
//
int InitialCond::ReadNumberData(CommentStream& infile, Keeper* const keeper,
  int noareas, int noagegr, int nolengr, int minage, const AreaClass* const Area) {

  ErrorHandler handle;
  //Find start of data in datafile
  infile >> ws;
  if (infile.eof()) {
    handle.Message("Initial stock number data file empty");
    return 0;
  }

  char c = infile.peek();
  if (!isdigit(c)) {
    infile.get(c);
    while (c != '\n' && !infile.eof())
      infile.get(c);
  }

  //Found the start of the data in the following format
  //area - age - length - number
  int age, area, length;
  int keepdata, ageid, areaid, lengthid;
  double tmpnumber;

  if (!(CheckColumns(infile, 4)))
    handle.Message("Wrong number of columns in inputfile - should be 4");

  ageid = 0;
  areaid = 0;
  lengthid = 0;
  keeper->AddString("numberdata");
  while (!infile.eof()) {
    keepdata = 0;
    infile >> area >> age >> length >> ws;

    //Crude data check - perhaps there should be a better check?
    if (age > noagegr)
      keepdata = 1;
    else
      ageid = minage + age - 1;  //age is counted from minage, not 0

    if (length > nolengr)
      keepdata = 1;
    else
      lengthid = length - 1;

    if (keepdata == 0) {
      //initial data is required, so store it
      areaid = Area->InnerArea(area);
      if (areaid == -1)
        handle.UndefinedArea(area);

      infile >> AreaAgeLength[areaid][ageid][lengthid].N >> ws;

    } else {
      //initial data is not required, so read but ignore it
      infile >> tmpnumber >> ws;
    }
  }

  keeper->ClearLast();
  return 1;
}

//
// Constructor for the initial conditions
//
InitialCond::InitialCond(CommentStream& infile, const intvector& area,
  Keeper* const keeper, const char* refWeightFile, const AreaClass* const Area)
  : LgrpDiv(0), CI(0), areas(area) {

  ErrorHandler handle;
  ifstream subfile;
  CommentStream subcomment(subfile);
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);

  keeper->AddString("initialcond");
  infile >> text;
  if (strcasecmp(text, "numbers") != 0)
    handle.Unexpected("numbers", text);

  int i, j, k;
  int noareas = area.Size();
  int minage = 0;
  int maxage = 0;
  double minlength = 0;
  double maxlength = 0;
  double dl = 0;
  ReadWordAndVariable(infile, "minage", minage);
  ReadWordAndVariable(infile, "maxage", maxage);
  ReadWordAndVariable(infile, "minlength", minlength);
  ReadWordAndVariable(infile, "maxlength", maxlength);
  ReadWordAndVariable(infile, "dl", dl);

  LgrpDiv = new LengthGroupDivision(minlength, maxlength, dl);
  if (LgrpDiv->Error())
    LengthGroupPrintError(minlength, maxlength, dl, keeper);

  int noagegr = maxage - minage + 1; //Number of age groups
  int nolengr = LgrpDiv->NoLengthGroups(); //Number of length groups

  //Now we have read age and length group division.
  infile >> text >> ws;
  keeper->AddString("agemultiple");
  agemultiple.resize(noagegr, minage, keeper);
  if ((strcasecmp(text, "agemultiple") == 0)) {
    if (!(infile >> agemultiple))
      handle.Message("Incorrect format of agemultiple");
    agemultiple.Inform(keeper);
  } else
    handle.Unexpected("agemultiple", text);
  keeper->ClearLast();

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
  use_lengthbasisfunctions = 0;
  readNumbers = 0;

  //create the AreaAgeLength object of the correct size
  popinfomatrix tmpPop(noagegr, nolengr);
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
    subfile.open(text);
    CheckIfFailure(subfile, text);
    handle.Open(text);
    this->ReadMeanData(subcomment, keeper, noareas, noagegr, minage, Area);
    handle.Close();
    subfile.close();
    subfile.clear();

  } else if ((strcasecmp(text, "distributionfile") == 0)) {
    //read initial distribution data
    infile >> text >> ws;
    subfile.open(text);
    CheckIfFailure(subfile, text);
    handle.Open(text);
    this->ReadDistributionData(subcomment, keeper, noareas, noagegr, minage, Area);
    handle.Close();
    subfile.close();
    subfile.clear();

    //Now have to read either basis function or lengthmultiple data
    //or the number data directly from a file
    infile >> text >> ws;
    if ((strcasecmp(text, "basisfunc") == 0)) {
      //read initial data in basis function format
      infile >> i >> ws >> text >> ws;
      subfile.open(text);
      CheckIfFailure(subfile, text);
      handle.Open(text);
      this->ReadBasisData(subcomment, keeper, i, noareas, noagegr, Area);
      handle.Close();
      subfile.close();
      subfile.clear();

    } else if ((strcasecmp(text, "lengthmultiplebasisfunc") == 0)) {
      //read initial data in length multiple basis function format
      infile >> use_lengthbasisfunctions >> ws;
      infile >> i >> ws >> text >> ws;
      subfile.open(text);
      CheckIfFailure(subfile, text);
      handle.Open(text);
      this->ReadLengthBasisData(subcomment, keeper, i, noareas, nolengr, Area);
      handle.Close();
      subfile.close();
      subfile.clear();

    } else if ((strcasecmp(text, "numberfile") == 0)) {
      readNumbers = 1;
      //read initial data in number format
      infile >> text >> ws;
      subfile.open(text);
      CheckIfFailure(subfile, text);
      handle.Open(text);
      this->ReadNumberData(subcomment, keeper, noareas, noagegr, nolengr, minage, Area);
      handle.Close();
      subfile.close();
      subfile.clear();

    } else {
      handle.Unexpected("type of datafile", text);
    }

  } else {
    handle.Unexpected("type of datafile", text);
  }
  keeper->ClearLast();

  //Now that the initial conditions have been read from file
  //we have to set the initial weight of the stock
  double rCond, rWeight;
  doublematrix tmpRefW;

  //Read information on reference weights.
  keeper->AddString("referenceweights");
  ifstream subweightfile(refWeightFile, ios::in);
  CheckIfFailure(subweightfile, refWeightFile);
  handle.Open(refWeightFile);
  CommentStream subweightcomment(subweightfile);
  if (!Read2ColVector(subweightcomment, tmpRefW))
    handle.Message("Wrong format for reference weights");
  handle.Close();
  subweightfile.close();
  subweightfile.clear();

  //Aggregate the reference weight data to be the same format
  doublevector Wref(nolengr);
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

        if (iszero(rWeight)) {
          cerr << "Possible error: zero mean weight for the following stock\n"
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
    outfile << "Inner Area " << areas[i] << endl;
    Printagebandm(outfile, AreaAgeLength[i]);
  }
  outfile << "Meanlengths\n";
  for (i = 0; i < Mean.Nrow(); i++) {
    for (j = 0; j < Mean.Ncol(i); j++)
      outfile << Mean[i][j] << TAB;
    outfile << endl;
  }
  outfile << "Sdev\n";
  for (i = 0; i < Sdev.Nrow(); i++) {
    for (j = 0; j < Sdev.Ncol(i); j++)
      outfile << Sdev[i][j] << TAB;
    outfile << endl;
  }
  outfile << "MeanPar\n";
  for (i = 0; i < MeanPar.Nrow(); i++) {
    for (j = 0; j < MeanPar.Ncol(i); j++)
      outfile << MeanPar[i][j] << TAB;
    outfile << endl;
  }
  outfile << "SdevPar\n";
  for (i = 0; i < SdevPar.Nrow(); i++) {
    for (j = 0; j < SdevPar.Ncol(i); j++)
      outfile << SdevPar[i][j] << TAB;
    outfile << endl;
  }
  outfile << "MeanBasis\n";
  for (i = 0; i < MeanBasis.Nrow(); i++) {
    for (j = 0; j < MeanBasis.Ncol(i); j++)
      outfile << MeanBasis[i][j] << TAB;
    outfile << endl;
  }
  outfile << "SdevBasis\n";
  for (i = 0; i < SdevBasis.Nrow(); i++) {
    for (j = 0; j < SdevBasis.Ncol(i); j++)
      outfile << SdevBasis[i][j] << TAB;
    outfile << endl;
  }
  outfile.flush();
}

//Area that is sent down to this function that resets the population
//in  all areas  where the stock is defined.
void InitialCond::Initialize(Agebandmatrixvector& Alkeys) { //const

  assert(Alkeys.Size() == areas.Size());
  //The following check should be replaced by something more descriptive!
  //This just means that if some sort of information on meanlengths was read
  //from file (instead of just numbers), the function Recalc should be called
  if (Mean.Nrow() > 0)
    Recalc();

  int area, age, length, i;
  int minage, maxage;
  double mult, lmult;

  for (area = 0; area < areas.Size(); area++) {
    Alkeys[area].SettoZero();
    minage = max(Alkeys[area].Minage(), AreaAgeLength[area].Minage());
    maxage = min(Alkeys[area].Maxage(), AreaAgeLength[area].Maxage());

    if (maxage < minage)
      return;

    for (age = minage; age <= maxage; age++) {
      mult = Distribution[area][age - minage] * agemultiple[age];
      if (mult < 0)
        mult = -mult;

      //Breytingar 17 - 5 og 19 - 12 96 HB.
      if (use_lengthbasisfunctions == 1 || use_lengthbasisfunctions == 2) {
        popinfoindexvector* NrAreaAge = new popinfoindexvector(AreaAgeLength[area][age]);
        for (length = NrAreaAge->Mincol(); length < NrAreaAge->Maxcol(); length++) {
          lmult = 0.0;
          for (i = 0; i < LengthBasisPar.Ncol(); i++)
            lmult += LengthBasisPar[area][i] * LengthBasis[length][i];

          if (use_lengthbasisfunctions == 1) {
            if (lmult < -50)
              lmult = -50;  //To avoid exception

            lmult = 1.0 / (1.0 + exp(-lmult));
          } else
            lmult = lmult * lmult;

          NrAreaAge->operator[](length) *= lmult;
        }
        PopinfoAdd(Alkeys[area][age], *NrAreaAge, *CI, mult);
        delete NrAreaAge; //30-11
      } else
        PopinfoAdd(Alkeys[area][age], AreaAgeLength[area][age], *CI, mult);

    }
  }
}

#ifdef INLINE_VECTORS
inline
#endif
double dnorm(double length, double mean, double sdev) {
  double t = (length - mean) / sdev;
  return exp(-(t * t) * 0.5);
}

void InitialCond::Recalc() {
  //Multiply the basis functions by the parameters.
  int i, j, age, area;
  double tempValue, scaler, length;
  int noareas = AreaAgeLength.Size(); //Number of areas
  int minage = AreaAgeLength[0].Minage();
  int maxage = AreaAgeLength[0].Maxage();
  int noagegr = maxage - minage + 1; //Number of age groups

  //This should be replaced by something more descriptive.  This just
  //means that if basis for meanlengths was read, then we replace the
  //contents of Mean by appropriate multiple of the basis functions
  if (MeanBasis.Nrow() > 0) {
    for (area = 0; area < noareas; area++) {
      for (i = 0; i < noagegr; i++) {
        Mean[area][i].setValue(0.0);
        tempValue = 0.0;
        for (j = 0; j < MeanBasis.Ncol(); j++) {
          assert(!Mean[area][i].isMultipleValue());
          tempValue += (MeanBasis[i][j] * MeanPar[area][j]);
        }
        Mean[area][i].setValue(tempValue);
      }
    }
  }

  //Do the same for standard deviation.
  if (SdevBasis.Nrow() > 0) {
    for (area = 0; area < noareas; area++) {
      for (i = 0; i < noagegr; i++) {
        Sdev[area][i].setValue(0.0);
        tempValue = 0.0;
        for (j = 0; j < SdevBasis.Ncol(); j++) {
          assert(!Sdev[area][i].isMultipleValue());
          tempValue += (SdevBasis[i][j] * SdevPar[area][j]);
        }
        Sdev[area][i].setValue(tempValue);
      }
    }
  }

  //Fill up the initial condition matrices, use normal distribution.
  //Multiplication by Distribution[area][age] is done in InitialCond::Initialize
  for (area = 0; area < noareas; area++) {
    for (age = minage; age <= maxage; age++) {
      scaler = 0.0;
      for (i = AreaAgeLength[area].Minlength(age);
          i < AreaAgeLength[area].Maxlength(age); i++) {
        length = LgrpDiv->Meanlength(i);
        AreaAgeLength[area][age][i].N =
          dnorm(length, Mean[area][age - minage], Sdev[area][age - minage] * SdevMult);
        scaler += AreaAgeLength[area][age][i].N;
      }
      scaler = (scaler > 1e-10 ? 10000/scaler : 0.0);
      for (i = AreaAgeLength[area].Minlength(age);
          i < AreaAgeLength[area].Maxlength(age); i++) {
        AreaAgeLength[area][age][i].N *= scaler;
      }
    }
  }
}
