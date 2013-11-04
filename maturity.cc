#include "maturity.h"
#include "stock.h"
#include "mathfunc.h"
#include "readfunc.h"
#include "readword.h"
#include "conversionindex.h"
#include "errorhandler.h"
#include "gadget.h"
#include "global.h"

// ********************************************************
// Functions for base Maturity
// ********************************************************
Maturity::Maturity(const IntVector& tmpareas, int minage, int numage,
  const LengthGroupDivision* const lgrpdiv, const char* givenname)
  : HasName(givenname), LivesOnAreas(tmpareas) {

  int i;
  istagged = 0;
  tmpratio = 1.0;
  ratioscale = 1.0; //JMB used to scale the ratios to ensure that they sum to 1
  LgrpDiv = new LengthGroupDivision(*lgrpdiv);
  if (LgrpDiv->Error())
    handle.logMessage(LOGFAIL, "Error in maturity - failed to create length group");

  IntVector lower(numage, 0);
  IntVector agesize(numage, LgrpDiv->numLengthGroups());
  Storage.resize(areas.Size(), minage, lower, agesize);
  for (i = 0; i < Storage.Size(); i++)
    Storage[i].setToZero();
}

Maturity::~Maturity() {
  int i;
  for (i = 0; i < matureStockNames.Size(); i++)
    delete[] matureStockNames[i];
  for (i = 0; i < CI.Size(); i++)
    delete CI[i];
  delete LgrpDiv;
}

void Maturity::setStock(StockPtrVector& stockvec) {
  int i, j, index;

  for (i = 0; i < matureStockNames.Size(); i++)
    for (j = 0; j < matureStockNames.Size(); j++)
      if ((strcasecmp(matureStockNames[i], matureStockNames[j]) == 0) && (i != j))
        handle.logMessage(LOGFAIL, "Error in maturity - repeated stock", matureStockNames[i]);

  for (i = 0; i < stockvec.Size(); i++)
    for (j = 0; j < matureStockNames.Size(); j++)
      if (strcasecmp(stockvec[i]->getName(), matureStockNames[j]) == 0)
        matureStocks.resize(stockvec[i]);

  if (matureStocks.Size() != matureStockNames.Size()) {
    handle.logMessage(LOGWARN, "Error in maturity - failed to match mature stocks");
    for (i = 0; i < stockvec.Size(); i++)
      handle.logMessage(LOGWARN, "Error in maturity - found stock", stockvec[i]->getName());
    for (i = 0; i < matureStockNames.Size(); i++)
      handle.logMessage(LOGWARN, "Error in maturity - looking for stock", matureStockNames[i]);
    handle.logMessage(LOGFAIL, ""); //JMB this will exit gadget
  }

  //JMB ensure that the ratio vector is indexed in the right order
  ratioindex.resize(matureStocks.Size(), 0);
  for (i = 0; i < matureStocks.Size(); i++)
    for (j = 0; j < matureStockNames.Size(); j++)
      if (strcasecmp(matureStocks[i]->getName(), matureStockNames[j]) == 0)
        ratioindex[i] = j;

  for (i = 0; i < matureStocks.Size(); i++) {
    CI.resize(new ConversionIndex(LgrpDiv, matureStocks[i]->getLengthGroupDiv()));
    if (CI[i]->Error())
      handle.logMessage(LOGFAIL, "Error in maturity - error when checking length structure");

    index = 0;
    for (j = 0; j < areas.Size(); j++)
      if (!matureStocks[i]->isInArea(areas[j]))
        index++;

    if (index != 0)
      handle.logMessage(LOGWARN, "Warning in maturity - mature stock isnt defined on all areas");
  }
}

void Maturity::Print(ofstream& outfile) const {
  int i;
  outfile << "\nMaturity\n\tNames of mature stocks:";
   for (i = 0; i < matureStockNames.Size(); i++)
    outfile << sep << matureStockNames[i];
  outfile << "\n\tRatio maturing into each stock:";
  for (i = 0; i < matureRatio.Size(); i++)
    outfile << sep << (matureRatio[ratioindex[i]] * ratioscale);
  outfile << "\n\tStored numbers:\n";
  for (i = 0; i < areas.Size(); i++) {
    outfile << "\tInternal area " << areas[i] << endl;
    Storage[i].printNumbers(outfile);
  }
}

void Maturity::Reset(const TimeClass* const TimeInfo) {
  //JMB check that the sum of the ratios is 1
  if (TimeInfo->getTime() == 1) {
    int i;
    ratioscale = 0.0;
    for (i = 0; i < matureRatio.Size(); i++ )
      ratioscale += matureRatio[i];

    if (isZero(ratioscale)) {
      handle.logMessage(LOGWARN, "Warning in maturity - specified ratios are zero");
      ratioscale = 1.0;
    } else if (isEqual(ratioscale, 1.0)) {
      // do nothing
    } else {
      handle.logMessage(LOGWARN, "Warning in maturity - scaling ratios using", ratioscale);
      ratioscale = 1.0 / ratioscale;
    }
  }
}

void Maturity::Move(int area, const TimeClass* const TimeInfo) {
  if (!(this->isMaturationStep(TimeInfo)))
    handle.logMessage(LOGFAIL, "Error in maturity - maturity requested on wrong timestep");
  int i, inarea = this->areaNum(area);
  for (i = 0; i < matureStocks.Size(); i++) {
    if (!matureStocks[i]->isInArea(area))
      handle.logMessage(LOGFAIL, "Error in maturity - mature stock doesnt live on area", area);

    tmpratio = matureRatio[ratioindex[i]] * ratioscale;
    matureStocks[i]->Add(Storage[inarea], CI[i], area, tmpratio);
    if (istagged && tagStorage.numTagExperiments() > 0)
      matureStocks[i]->Add(tagStorage, CI[i], area, tmpratio);
  }

  Storage[inarea].setToZero();
  if (istagged && tagStorage.numTagExperiments() > 0)
    tagStorage[inarea].setToZero();
}

void Maturity::storeMatureStock(int area, int age, int length, double number, double weight) {
  if (isZero(number) || isZero(weight)) {
    Storage[this->areaNum(area)][age][length].setToZero();
  } else {
    Storage[this->areaNum(area)][age][length].N = number;
    Storage[this->areaNum(area)][age][length].W = weight;
  }
}

void Maturity::storeMatureTagStock(int area, int age, int length, double number, int id) {
  if (!istagged)
    handle.logMessage(LOGFAIL, "Error in maturity - invalid tagging experiment");
  if ((id >= tagStorage.numTagExperiments()) || (id < 0))
    handle.logMessage(LOGFAIL, "Error in maturity - invalid tagging experiment");
  if (isZero(number))
    *(tagStorage[this->areaNum(area)][age][length][id].N) = 0.0;
  else
    *(tagStorage[this->areaNum(area)][age][length][id].N) = number;
}

const StockPtrVector& Maturity::getMatureStocks() {
  return matureStocks;
}

void Maturity::setTagged() {
  istagged = 1;
  //resize tagStorage to be the same size as Storage
  int i, minage, maxage;
  minage = Storage[0].minAge();
  maxage = Storage[0].maxAge();
  IntVector lower(maxage - minage + 1, 0);
  IntVector size(maxage - minage + 1, LgrpDiv->numLengthGroups());
  tagStorage.resize(areas.Size(), minage, lower, size);
  for (i = 0; i < tagStorage.Size(); i++)
    tagStorage[i].setToZero();
}

void Maturity::addMaturityTag(const char* tagname) {
  if (!istagged)
    handle.logMessage(LOGFAIL, "Error in maturity - invalid tagging experiment", tagname);
  tagStorage.addTag(tagname);
}

void Maturity::deleteMaturityTag(const char* tagname) {
  if (!istagged)
    handle.logMessage(LOGFAIL, "Error in maturity - invalid tagging experiment", tagname);

  int minage, maxage, age, len, a;
  int id = tagStorage.getTagID(tagname);

  if (id >= 0) {
    minage = tagStorage[0].minAge();
    maxage = tagStorage[0].maxAge();
    //free memory allocated for tagging experiment
    for (a = 0; a < tagStorage.Size(); a++) {
      for (age = minage; age <= maxage; age++) {
        for (len = tagStorage[a].minLength(age); len < tagStorage[a].maxLength(age); len++) {
          delete[] (tagStorage[a][age][len][id].N);
          (tagStorage[a][age][len][id].N) = NULL;
        }
      }
    }
    tagStorage.deleteTag(tagname);

  } else
    handle.logMessage(LOGWARN, "Warning in maturity - failed to delete tagging experiment", tagname);
}

// ********************************************************
// Functions for MaturityA
// ********************************************************
MaturityA::MaturityA(CommentStream& infile, const TimeClass* const TimeInfo,
  Keeper* const keeper, int minage, int numage, const IntVector& tmpareas,
  const char* givenname, const LengthGroupDivision* const lgrpdiv)
  : Maturity(tmpareas, minage, numage, lgrpdiv, givenname), minStockAge(minage) {

  //JMB store the length of the timestep
  timesteplength = TimeInfo->getTimeStepSize();

  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  int i;

  keeper->addString("maturity");
  infile >> text >> ws;
  if ((strcasecmp(text, "nameofmaturestocksandratio") != 0) && (strcasecmp(text, "maturestocksandratios") != 0))
    handle.logFileUnexpected(LOGFAIL, "maturestocksandratios", text);

  i = 0;
  infile >> text >> ws;
  while (strcasecmp(text, "coefficients") != 0 && !infile.eof()) {
    matureStockNames.resize(new char[strlen(text) + 1]);
    strcpy(matureStockNames[i], text);
    matureRatio.resize(1, keeper);
    if (!(infile >> matureRatio[i]))
      handle.logFileMessage(LOGFAIL, "invalid format for mature ratio");
    matureRatio[i].Inform(keeper);

    infile >> text >> ws;
    i++;
  }

  if (infile.eof())
    handle.logFileEOFMessage(LOGFAIL);
  maturityParameters.resize(4, keeper);
  maturityParameters.read(infile, TimeInfo, keeper);
  preCalcMaturation.AddRows(numage, LgrpDiv->numLengthGroups(), 0.0);

  infile >> ws;
  if (!infile.eof()) {
    infile >> text >> ws;
    handle.logFileUnexpected(LOGFAIL, "<end of file>", text);
  }
  handle.logMessage(LOGMESSAGE, "Read maturity data file");
  keeper->clearLast();
}

void MaturityA::Reset(const TimeClass* const TimeInfo) {
  Maturity::Reset(TimeInfo);

  if (TimeInfo->didStepSizeChange())
    timesteplength = TimeInfo->getTimeStepSize();

  maturityParameters.Update(TimeInfo);
  if (maturityParameters.didChange(TimeInfo)) {
    if (maturityParameters[1] < LgrpDiv->minLength())
      handle.logMessage(LOGWARN, "Warning in maturity calculation - l50 less than minimum length for stock", this->getName());
    if (maturityParameters[1] > LgrpDiv->maxLength())
      handle.logMessage(LOGWARN, "Warning in maturity calculation - l50 greater than maximum length for stock", this->getName());

    int age, len;
    for (age = 0; age < preCalcMaturation.Nrow(); age++) {
      for (len = 0; len < LgrpDiv->numLengthGroups(); len++) {
        tmpratio = exp(-maturityParameters[0] * (LgrpDiv->meanLength(len) - maturityParameters[1]) -
             maturityParameters[2] * (age + minStockAge - maturityParameters[3]));
        preCalcMaturation[age][len] = 1.0 / (1.0 + tmpratio);
      }
    }

    if (handle.getLogLevel() >= LOGMESSAGE)
      handle.logMessage(LOGMESSAGE, "Reset maturity data for stock", this->getName());
  }
}

void MaturityA::setStock(StockPtrVector& stockvec) {
  Maturity::setStock(stockvec);

  int i;
  minMatureAge = 9999;
  double minlength = 9999.0;
  for (i = 0; i < matureStocks.Size(); i++) {
    minMatureAge = min(matureStocks[i]->minAge(), minMatureAge);
    minlength = min(matureStocks[i]->getLengthGroupDiv()->minLength(), minlength);
  }
  minMatureLength = LgrpDiv->numLengthGroup(minlength);
  if (minMatureAge < minStockAge)
    handle.logMessage(LOGFAIL, "Error in maturity - minimum mature age is less than stock age for stock", this->getName());
}

double MaturityA::calcMaturation(int age, int length, int growth, double weight) {

  if ((age >= minMatureAge) && ((length + growth) >= minMatureLength)) {
    tmpratio = preCalcMaturation[age - minStockAge][length] *
       (maturityParameters[0] * growth * LgrpDiv->dl() + maturityParameters[2] * timesteplength);
    return (min(max(0.0, tmpratio), 1.0));
  }
  return 0.0;
}

void MaturityA::Print(ofstream& outfile) const {
  Maturity::Print(outfile);
  outfile << "\tPrecalculated maturity:\n";
  preCalcMaturation.Print(outfile);
}

int MaturityA::isMaturationStep(const TimeClass* const TimeInfo) {
  return 1;
}

// ********************************************************
// Functions for MaturityB
// ********************************************************
MaturityB::MaturityB(CommentStream& infile, const TimeClass* const TimeInfo,
  Keeper* const keeper, int minage, int numage, const IntVector& tmpareas,
  const char* givenname, const LengthGroupDivision* const lgrpdiv)
  : Maturity(tmpareas, minage, numage, lgrpdiv, givenname) {

  //JMB set the default value for currenttimestep
  currentmaturitystep = 0;

  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  int i, tmpint = 0;

  keeper->addString("maturity");
  infile >> text >> ws;
  if ((strcasecmp(text, "nameofmaturestocksandratio") != 0) && (strcasecmp(text, "maturestocksandratios") != 0))
    handle.logFileUnexpected(LOGFAIL, "maturestocksandratios", text);

  i = 0;
  infile >> text >> ws;
  while (strcasecmp(text, "maturitysteps") != 0 && !infile.eof()) {
    matureStockNames.resize(new char[strlen(text) + 1]);
    strcpy(matureStockNames[i], text);
    matureRatio.resize(1, keeper);
    if (!(infile >> matureRatio[i]))
      handle.logFileMessage(LOGFAIL, "invalid format for mature ratio");
    matureRatio[i].Inform(keeper);

    infile >> text >> ws;
    i++;
  }

  if (infile.eof())
    handle.logFileEOFMessage(LOGFAIL);

  infile >> ws;
  while (isdigit(infile.peek()) && !infile.eof()) {
    infile >> tmpint >> ws;
    maturitystep.resize(1, tmpint);
  }
  if (infile.eof())
    handle.logFileEOFMessage(LOGFAIL);

  i = 0;
  infile >> text;
  if (strcasecmp(text, "maturitylengths") != 0)
    handle.logFileUnexpected(LOGFAIL, "maturitylengths", text);
  while (i < maturitystep.Size() && !infile.eof()) {
    maturitylength.resize(1, keeper);
    maturitylength[i++].read(infile, TimeInfo, keeper);
  }

  for (i = 0; i < maturitystep.Size(); i++)
    if (maturitystep[i] < 1 || maturitystep[i] > TimeInfo->numSteps())
      handle.logFileMessage(LOGFAIL, "invalid maturity step", maturitystep[i]);

  if (maturitylength.Size() != maturitystep.Size())
    handle.logFileMessage(LOGFAIL, "number of maturitysteps does not equal number of maturitylengths");

  infile >> ws;
  if (!infile.eof()) {
    infile >> text >> ws;
    handle.logFileUnexpected(LOGFAIL, "<end of file>", text);
  }
  handle.logMessage(LOGMESSAGE, "Read maturity data file");
  keeper->clearLast();
}

void MaturityB::Print(ofstream& outfile) const {
  int i;
  Maturity::Print(outfile);
  outfile << "\tMaturity timesteps";
  for (i = 0; i < maturitystep.Size(); i++)
    outfile << sep << maturitystep[i];
  outfile << "\n\tMaturity lengths";
  for (i = 0; i < maturitylength.Size(); i++)
    outfile << sep << maturitylength[i];
  outfile << endl;
}

void MaturityB::setStock(StockPtrVector& stockvec) {
  Maturity::setStock(stockvec);
}

void MaturityB::Reset(const TimeClass* const TimeInfo) {
  Maturity::Reset(TimeInfo);

  int i;
  maturitylength.Update(TimeInfo);
  if (maturitylength.didChange(TimeInfo)) {
    for (i = 0; i < maturitylength.Size(); i++) {
      if (maturitylength[i] < LgrpDiv->minLength())
        handle.logMessage(LOGWARN, "Warning in maturity calculation - length less than minimum stock length for stock", this->getName());
      if (maturitylength[i] > LgrpDiv->maxLength())
        handle.logMessage(LOGWARN, "Warning in maturity calculation - length greater than maximum stock length for stock", this->getName());
    }

    if (handle.getLogLevel() >= LOGMESSAGE)
      handle.logMessage(LOGMESSAGE, "Reset maturity data for stock", this->getName());
  }
}

double MaturityB::calcMaturation(int age, int length, int growth, double weight) {
  if ((LgrpDiv->meanLength(length) > maturitylength[currentmaturitystep]) ||
      (isEqual(LgrpDiv->meanLength(length), maturitylength[currentmaturitystep])))
    return 1.0;
  return 0.0;
}

int MaturityB::isMaturationStep(const TimeClass* const TimeInfo) {
  int i;
  for (i = 0; i < maturitystep.Size(); i++) {
    if (maturitystep[i] == TimeInfo->getStep()) {
      currentmaturitystep = i;
      return 1;
    }
  }
  return 0;
}

// ********************************************************
// Functions for MaturityC
// ********************************************************
MaturityC::MaturityC(CommentStream& infile, const TimeClass* const TimeInfo,
  Keeper* const keeper, int minage, int numage, const IntVector& tmpareas,
  const char* givenname, const LengthGroupDivision* const lgrpdiv, int numMatConst)
  : Maturity(tmpareas, minage, numage, lgrpdiv, givenname), minStockAge(minage) {

  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  int i, tmpint = 0;

  keeper->addString("maturity");
  infile >> text >> ws;
  if ((strcasecmp(text, "nameofmaturestocksandratio") != 0) && (strcasecmp(text, "maturestocksandratios") != 0))
    handle.logFileUnexpected(LOGFAIL, "maturestocksandratios", text);

  i = 0;
  infile >> text >> ws;
  while (strcasecmp(text, "coefficients") != 0 && !infile.eof()) {
    matureStockNames.resize(new char[strlen(text) + 1]);
    strcpy(matureStockNames[i], text);
    matureRatio.resize(1, keeper);
    if (!(infile >> matureRatio[i]))
      handle.logFileMessage(LOGFAIL, "invalid format for mature ratio");
    matureRatio[i].Inform(keeper);

    infile >> text >> ws;
    i++;
  }

  if (infile.eof())
    handle.logFileEOFMessage(LOGFAIL);
  maturityParameters.resize(numMatConst, keeper);
  maturityParameters.read(infile, TimeInfo, keeper);
  preCalcMaturation.AddRows(numage, LgrpDiv->numLengthGroups(), 0.0);

  infile >> text >> ws;
  if ((strcasecmp(text, "maturitystep") != 0) && (strcasecmp(text, "maturitysteps") != 0))
    handle.logFileUnexpected(LOGFAIL, "maturitysteps", text);

  while (isdigit(infile.peek()) && !infile.eof()) {
    infile >> tmpint >> ws;
    maturitystep.resize(1, tmpint);
    i++;
  }

  for (i = 0; i < maturitystep.Size(); i++)
    if (maturitystep[i] < 1 || maturitystep[i] > TimeInfo->numSteps())
      handle.logFileMessage(LOGFAIL, "invalid maturity step", maturitystep[i]);

  infile >> ws;
  if (!infile.eof()) {
    infile >> text >> ws;
    handle.logFileUnexpected(LOGFAIL, "<end of file>", text);
  }

  handle.logMessage(LOGMESSAGE, "Read maturity data file");
  keeper->clearLast();
}

void MaturityC::setStock(StockPtrVector& stockvec) {
  Maturity::setStock(stockvec);

  int i;
  minMatureAge = 9999;
  double minlength = 9999.0;
  for (i = 0; i < matureStocks.Size(); i++) {
    minMatureAge = min(matureStocks[i]->minAge(), minMatureAge);
    minlength = min(matureStocks[i]->getLengthGroupDiv()->minLength(), minlength);
  }
  minMatureLength = LgrpDiv->numLengthGroup(minlength);
  if (minMatureAge < minStockAge)
    handle.logMessage(LOGFAIL, "Error in maturity - minimum mature age is less than stock age for stock", this->getName());
}

void MaturityC::Reset(const TimeClass* const TimeInfo) {
  Maturity::Reset(TimeInfo);

  maturityParameters.Update(TimeInfo);
  if (maturityParameters.didChange(TimeInfo)) {
    if (maturityParameters[1] < LgrpDiv->minLength())
      handle.logMessage(LOGWARN, "Warning in maturity calculation - l50 less than minimum length for stock", this->getName());
    if (maturityParameters[1] > LgrpDiv->maxLength())
      handle.logMessage(LOGWARN, "Warning in maturity calculation - l50 greater than maximum length for stock", this->getName());

    int age, len;
    for (age = 0; age < preCalcMaturation.Nrow(); age++) {
      for (len = 0; len < LgrpDiv->numLengthGroups(); len++) {
        if ((age + minStockAge >= minMatureAge) && (len >= minMatureLength)) {
          tmpratio = exp(-1.0 * maturityParameters[0] * (LgrpDiv->meanLength(len) - maturityParameters[1])
                 - maturityParameters[2] * (age + minStockAge - maturityParameters[3]));
          preCalcMaturation[age][len] = 1.0 / (1.0 + tmpratio);
          if (preCalcMaturation[age][len] < 0.0)
            preCalcMaturation[age][len] = 0.0;
          if (preCalcMaturation[age][len] > 1.0)
            preCalcMaturation[age][len] = 1.0;

        } else
          preCalcMaturation[age][len] = 0.0;
      }
    }

    if (handle.getLogLevel() >= LOGMESSAGE)
      handle.logMessage(LOGMESSAGE, "Reset maturity data for stock", this->getName());
  }
}

double MaturityC::calcMaturation(int age, int length, int growth, double weight) {
  if (age >= minStockAge)
    return preCalcMaturation[age - minStockAge][length];
  return 0.0;
}

int MaturityC::isMaturationStep(const TimeClass* const TimeInfo) {
  int i;
  for (i = 0; i < maturitystep.Size(); i++)
    if (maturitystep[i] == TimeInfo->getStep())
      return 1;
  return 0;
}

void MaturityC::Print(ofstream& outfile) const {
  int i;
  Maturity::Print(outfile);
  outfile << "\tPrecalculated maturity:\n";
  preCalcMaturation.Print(outfile);
  outfile << "\tMaturity timesteps:";
  for (i = 0; i < maturitystep.Size(); i++)
    outfile << sep << maturitystep[i];
  outfile << endl;
}

// ********************************************************
// Functions for MaturityD
// ********************************************************
MaturityD::MaturityD(CommentStream& infile, const TimeClass* const TimeInfo,
  Keeper* const keeper, int minage, int numage, const IntVector& tmpareas,
  const char* givenname, const LengthGroupDivision* const lgrpdiv, int numMatConst, const char* refWeightFile)
  : MaturityC(infile, TimeInfo, keeper, minage, numage, tmpareas, givenname, lgrpdiv, numMatConst) {

  //read information on reference weights.
  ifstream subweightfile;
  subweightfile.open(refWeightFile, ios::in);
  handle.checkIfFailure(subweightfile, refWeightFile);
  handle.Open(refWeightFile);
  CommentStream subweightcomment(subweightfile);
  DoubleMatrix tmpRefW;
  readRefWeights(subweightcomment, tmpRefW);
  handle.Close();
  subweightfile.close();
  subweightfile.clear();

  //aggregate the reference weight data to be the same length groups
  if (LgrpDiv->meanLength(0) < tmpRefW[0][0] ||
      LgrpDiv->meanLength(LgrpDiv->numLengthGroups() - 1) > tmpRefW[tmpRefW.Nrow() - 1][0])
    handle.logFileMessage(LOGFAIL, "lengths for reference weights must span the range of growth lengths");

  refWeight.resize(LgrpDiv->numLengthGroups(), 0.0);
  int i, j, pos = 0;
  double tmplen;
  for (j = 0; j < LgrpDiv->numLengthGroups(); j++) {
    tmplen = LgrpDiv->meanLength(j);
    for (i = pos; i < tmpRefW.Nrow() - 1; i++) {
      if (((tmplen > tmpRefW[i][0]) || (isEqual(tmplen, tmpRefW[i][0]))) &&
          ((tmplen < tmpRefW[i + 1][0]) || (isEqual(tmplen, tmpRefW[i + 1][0])))) {

        tmpratio = (tmplen - tmpRefW[i][0]) / (tmpRefW[i + 1][0] - tmpRefW[i][0]);
        refWeight[j] = tmpRefW[i][1] + tmpratio * (tmpRefW[i + 1][1] - tmpRefW[i][1]);
        pos = i;
      }
    }
  }
}

void MaturityD::setStock(StockPtrVector& stockvec) {
  Maturity::setStock(stockvec);

  int i;
  minMatureAge = 9999;
  double minlength = 9999.0;
  for (i = 0; i < matureStocks.Size(); i++) {
    minMatureAge = min(matureStocks[i]->minAge(), minMatureAge);
    minlength = min(matureStocks[i]->getLengthGroupDiv()->minLength(), minlength);
  }
  minMatureLength = LgrpDiv->numLengthGroup(minlength);
  if (minMatureAge < minStockAge)
    handle.logMessage(LOGFAIL, "Error in maturity - minimum mature age is less than stock age for stock", this->getName());
}

void MaturityD::Reset(const TimeClass* const TimeInfo) {
  Maturity::Reset(TimeInfo);

  maturityParameters.Update(TimeInfo);
  if (maturityParameters.didChange(TimeInfo)) {
    if (maturityParameters[1] < LgrpDiv->minLength())
      handle.logMessage(LOGWARN, "Warning in maturity calculation - l50 less than minimum length for stock", this->getName());
    if (maturityParameters[1] > LgrpDiv->maxLength())
      handle.logMessage(LOGWARN, "Warning in maturity calculation - l50 greater than maximum length for stock", this->getName());

   if (handle.getLogLevel() >= LOGMESSAGE)
      handle.logMessage(LOGMESSAGE, "Reset maturity data for stock", this->getName());
  }
}

double MaturityD::calcMaturation(int age, int length, int growth, double weight) {

  if ((age >= minMatureAge) && (length >= minMatureLength)) {
    double tmpweight, my;

    if ((length >= refWeight.Size()) || (isZero(refWeight[length])))
      tmpweight = maturityParameters[5];
    else
      tmpweight = weight / refWeight[length];

    my = exp(-1.0 * maturityParameters[0] * (LgrpDiv->meanLength(length) - maturityParameters[1])
           - maturityParameters[2] * (age + minStockAge - maturityParameters[3])
           - maturityParameters[4] * (tmpweight - maturityParameters[5]));
    tmpratio = 1.0 / (1.0 + my);
    return (min(max(0.0, tmpratio), 1.0));
  }
  return 0.0;
}

int MaturityD::isMaturationStep(const TimeClass* const TimeInfo) {
  int i;
  for (i = 0; i < maturitystep.Size(); i++)
    if (maturitystep[i] == TimeInfo->getStep())
      return 1;
  return 0;
}

void MaturityD::Print(ofstream& outfile) const {
  int i;
  Maturity::Print(outfile);
  outfile << "\tMaturity timesteps:";
  for (i = 0; i < maturitystep.Size(); i++)
    outfile << sep << maturitystep[i];
  outfile << endl;
}
