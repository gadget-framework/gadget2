#include "maturity.h"
#include "stock.h"
#include "mathfunc.h"
#include "readfunc.h"
#include "readword.h"
#include "conversionindex.h"
#include "errorhandler.h"
#include "gadget.h"

extern ErrorHandler handle;

// ********************************************************
// Functions for base Maturity
// ********************************************************
Maturity::Maturity(const IntVector& tmpareas, int minage, const IntVector& minlength,
  const IntVector& size, const LengthGroupDivision* const lgrpdiv)
  : LivesOnAreas(tmpareas), LgrpDiv(new LengthGroupDivision(*lgrpdiv)),
    Storage(tmpareas.Size(), minage, minlength, size),
    tagStorage(tmpareas.Size(), minage, minlength, size) {

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
  int index = 0;
  int i, j;
  DoubleVector tmpratio;

  for (i = 0; i < stockvec.Size(); i++)
    for (j = 0; j < matureStockNames.Size(); j++)
      if (strcasecmp(stockvec[i]->getName(), matureStockNames[j]) == 0) {
        matureStocks.resize(1);
        tmpratio.resize(1);
        matureStocks[index] = stockvec[i];
        tmpratio[index] = Ratio[j];
        index++;
      }

  if (index != matureStockNames.Size()) {
    handle.logMessage(LOGWARN, "Error in maturity - failed to match mature stocks");
    for (i = 0; i < stockvec.Size(); i++)
      handle.logMessage(LOGWARN, "Error in maturity - found stock", stockvec[i]->getName());
    for (i = 0; i < matureStockNames.Size(); i++)
      handle.logMessage(LOGWARN, "Error in maturity - looking for stock", matureStockNames[i]);
    exit(EXIT_FAILURE);
  }

  for (i = Ratio.Size(); i > 0; i--)
    Ratio.Delete(0);
  Ratio.resize(tmpratio.Size());
  for (i = 0; i < tmpratio.Size(); i++)
    Ratio[i] = tmpratio[i];

  CI.resize(matureStocks.Size(), 0);
  for (i = 0; i < matureStocks.Size(); i++)
    CI[i] = new ConversionIndex(LgrpDiv, matureStocks[i]->returnLengthGroupDiv());

  for (i = 0; i < matureStocks.Size(); i++) {
    index = 0;
    for (j = 0; j < areas.Size(); j++)
      if (!matureStocks[i]->isInArea(areas[j]))
        index++;

    if ((handle.getLogLevel() >= LOGWARN) && (index != 0))
      handle.logMessage(LOGWARN, "Warning in maturity - mature stock isnt defined on all areas");
  }

  for (i = 0; i < areas.Size(); i++) {
    Storage[i].setToZero();
    tagStorage[i].setToZero();
  }
}

void Maturity::Print(ofstream& outfile) const {
  int i;
  outfile << "\nMaturity\n\tNames of mature stocks:";
   for (i = 0; i < matureStockNames.Size(); i++)
    outfile << sep << matureStockNames[i];
  outfile << "\n\tStored numbers:\n";
  for (i = 0; i < areas.Size(); i++) {
    outfile << "\tInternal area " << areas[i] << endl;
    Storage[i].printNumbers(outfile);
  }
}

void Maturity::Move(int area, const TimeClass* const TimeInfo) {
  if (!(this->isMaturationStep(area, TimeInfo)))
    handle.logMessage(LOGFAIL, "Error in maturity - maturity requested on wrong timestep");
  int i, inarea = this->areaNum(area);
  for (i = 0; i < matureStocks.Size(); i++) {
    if (!matureStocks[i]->isInArea(area))
      handle.logMessage(LOGFAIL, "Error in maturity - mature stock doesnt live on area", area);

    matureStocks[i]->Add(Storage[inarea], CI[i], area, Ratio[i],
      Storage[inarea].minAge(), Storage[inarea].maxAge());

    if (tagStorage.numTagExperiments() > 0)
      matureStocks[i]->Add(tagStorage, inarea, CI[i], area, Ratio[i],
        tagStorage[inarea].minAge(), tagStorage[inarea].maxAge());
  }

  Storage[inarea].setToZero();
  if (tagStorage.numTagExperiments() > 0)
    tagStorage[inarea].setToZero();
}

void Maturity::PutInStorage(int area, int age, int length, double number,
  double weight, const TimeClass* const TimeInfo) {

  if (!(this->isMaturationStep(area, TimeInfo)))
    handle.logMessage(LOGFAIL, "Error in maturity - maturity requested on wrong timestep");
  int inarea = this->areaNum(area);
  if (isZero(number) || isZero(weight)) {
    Storage[inarea][age][length].N = 0.0;
    Storage[inarea][age][length].W = 0.0;
  } else {
    Storage[inarea][age][length].N = number;
    Storage[inarea][age][length].W = weight;
  }
}

void Maturity::PutInStorage(int area, int age, int length, double number,
  const TimeClass* const TimeInfo, int id) {

  if (!(this->isMaturationStep(area, TimeInfo)))
    handle.logMessage(LOGFAIL, "Error in maturity - maturity requested on wrong timestep");
  if (tagStorage.numTagExperiments() <= 0)
    handle.logMessage(LOGFAIL, "Error in maturity - no tagging experiment");
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

void Maturity::addMaturityTag(const char* tagname) {
  tagStorage.addTag(tagname);
}

void Maturity::deleteMaturityTag(const char* tagname) {
  int minage, maxage, minlen, maxlen, age, length, i;
  int id = tagStorage.getTagID(tagname);

  if (id >= 0) {
    minage = tagStorage[0].minAge();
    maxage = tagStorage[0].maxAge();
    // Remove allocated memory
    for (i = 0; i < tagStorage.Size(); i++) {
      for (age = minage; age <= maxage; age++) {
        minlen = tagStorage[i].minLength(age);
        maxlen = tagStorage[i].maxLength(age);
        for (length = minlen; length < maxlen; length++) {
          delete[] (tagStorage[i][age][length][id].N);
          (tagStorage[i][age][length][id].N) = NULL;
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
  Keeper* const keeper, int minage, const IntVector& minabslength, const IntVector& size,
  const IntVector& tmpareas, const LengthGroupDivision* const lgrpdiv)
  : Maturity(tmpareas, minage, minabslength, size, lgrpdiv), preCalcMaturation(minabslength, size, minage) {

  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  int i;
  keeper->addString("maturity");
  infile >> text;
  if ((strcasecmp(text, "nameofmaturestocksandratio") == 0) || (strcasecmp(text, "maturestocksandratios") == 0)) {
    i = 0;
    infile >> text >> ws;
    while (strcasecmp(text, "coefficients") != 0 && !infile.eof()) {
      matureStockNames.resize(1);
      matureStockNames[i] = new char[strlen(text) + 1];
      strcpy(matureStockNames[i], text);
      Ratio.resize(1);
      infile >> Ratio[i] >> text >> ws;
      i++;
    }
  } else
    handle.logFileUnexpected(LOGFAIL, "maturestocksandratios", text);

  if (infile.eof())
    handle.logFileEOFMessage(LOGFAIL);
  maturityParameters.resize(4, keeper);
  maturityParameters.read(infile, TimeInfo, keeper);

  infile >> ws;
  if (!infile.eof()) {
    infile >> text >> ws;
    handle.logFileUnexpected(LOGFAIL, "<end of file>", text);
  }
  if (handle.getLogLevel() >= LOGMESSAGE)
    handle.logMessage(LOGMESSAGE, "Read maturity data file");
  keeper->clearLast();
}

void MaturityA::Reset(const TimeClass* const TimeInfo) {
  maturityParameters.Update(TimeInfo);
  double my;
  int age, len;
  if (maturityParameters.didChange(TimeInfo)) {
    if ((handle.getLogLevel() >= LOGWARN) && (maturityParameters[1] > LgrpDiv->maxLength()))
      handle.logMessage(LOGWARN, "Warning in maturity calculation - l50 greater than maximum length");

    for (age = preCalcMaturation.minRow(); age <= preCalcMaturation.maxRow(); age++) {
      for (len = preCalcMaturation.minCol(age); len < preCalcMaturation.maxCol(age); len++) {
        my = exp(-maturityParameters[0] * (LgrpDiv->meanLength(len) - maturityParameters[1])
               - maturityParameters[2] * (age - maturityParameters[3]));
        preCalcMaturation[age][len] = 1.0 / (1.0 + my);
      }
    }
    if (handle.getLogLevel() >= LOGMESSAGE)
      handle.logMessage(LOGMESSAGE, "Reset maturity data");
  }
}

void MaturityA::setStock(StockPtrVector& stockvec) {
  Maturity::setStock(stockvec);

  int i;
  minMatureAge = 9999;
  double minlength = 9999.0;
  for (i = 0; i < matureStocks.Size(); i++) {
    minMatureAge = min(matureStocks[i]->minAge(), minMatureAge);
    minlength = min(matureStocks[i]->returnLengthGroupDiv()->minLength(), minlength);
  }
  minMatureLength = LgrpDiv->numLengthGroup(minlength);
}

double MaturityA::MaturationProbability(int age, int length, int growth,
  const TimeClass* const TimeInfo, int area, double weight) {

  if ((age >= minMatureAge) && ((length + growth) >= minMatureLength)) {
    double prob = preCalcMaturation[age][length] * (maturityParameters[0] * growth * LgrpDiv->dl()
                  + maturityParameters[2] * TimeInfo->getTimeStepSize());
    return (min(max(0.0, prob), 1.0));
  }
  return 0.0;
}

void MaturityA::Print(ofstream& outfile) const {
  Maturity::Print(outfile);
  outfile << "\tPrecalculated maturity:\n";
  preCalcMaturation.Print(outfile);
}

int MaturityA::isMaturationStep(int area, const TimeClass* const TimeInfo) {
  return 1;
}

// ********************************************************
// Functions for MaturityB
// ********************************************************
MaturityB::MaturityB(CommentStream& infile, const TimeClass* const TimeInfo,
  Keeper* const keeper, int minage, const IntVector& minabslength,
  const IntVector& size, const IntVector& tmpareas, const LengthGroupDivision* const lgrpdiv)
  : Maturity(tmpareas, minage, minabslength, size, lgrpdiv) {

  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  int i;
  infile >> text;
  keeper->addString("maturity");
  if ((strcasecmp(text, "nameofmaturestocksandratio") == 0) || (strcasecmp(text, "maturestocksandratios") == 0)) {
    i = 0;
    infile >> text >> ws;
    while (!(strcasecmp(text, "maturitysteps") == 0) && !infile.eof()) {
      matureStockNames.resize(1);
      matureStockNames[i] = new char[strlen(text) + 1];
      strcpy(matureStockNames[i], text);
      Ratio.resize(1);
      infile >> Ratio[i] >> text >> ws;
      i++;
    }
  } else
    handle.logFileUnexpected(LOGFAIL, "maturestocksandratios", text);

  if (infile.eof())
    handle.logFileEOFMessage(LOGFAIL);

  infile >> ws;
  while (isdigit(infile.peek()) && !infile.eof()) {
    maturitystep.resize(1);
    infile >> maturitystep[maturitystep.Size() - 1] >> ws;
  }
  if (infile.eof())
    handle.logFileEOFMessage(LOGFAIL);

  infile >> text;
  if (!(strcasecmp(text, "maturitylengths") == 0))
    handle.logFileUnexpected(LOGFAIL, "maturitylengths", text);
  while (maturitylength.Size() < maturitystep.Size() && !infile.eof()) {
    maturitylength.resize(1, keeper);
    maturitylength[maturitylength.Size() - 1].read(infile, TimeInfo, keeper);
  }

  if (maturitylength.Size() != maturitystep.Size())
    handle.logFileMessage(LOGFAIL, "Error in maturity - number of maturitysteps does not equal number of maturitylengths");

  infile >> ws;
  if (!infile.eof()) {
    infile >> text >> ws;
    handle.logFileUnexpected(LOGFAIL, "<end of file>", text);
  }
  if (handle.getLogLevel() >= LOGMESSAGE)
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
  maturitylength.Update(TimeInfo);
  if (maturitylength.didChange(TimeInfo))
    if (handle.getLogLevel() >= LOGMESSAGE)
      handle.logMessage(LOGMESSAGE, "Reset maturity data");
}

double MaturityB::MaturationProbability(int age, int length, int growth,
  const TimeClass* const TimeInfo, int area, double weight) {

  int i;
  for (i = 0; i < maturitylength.Size(); i++)
    if (TimeInfo->getStep() == maturitystep[i])
      if (LgrpDiv->meanLength(length) >= maturitylength[i])
        return 1.0;

  return 0.0;
}

int MaturityB::isMaturationStep(int area, const TimeClass* const TimeInfo) {
  int i;
  for (i = 0; i < maturitystep.Size(); i++)
    if (maturitystep[i] == TimeInfo->getStep())
      return 1;
  return 0;
}

// ********************************************************
// Functions for MaturityC
// ********************************************************
MaturityC::MaturityC(CommentStream& infile, const TimeClass* const TimeInfo,
  Keeper* const keeper, int minage, const IntVector& minabslength, const IntVector& size,
  const IntVector& tmpareas, const LengthGroupDivision* const lgrpdiv, int numMatConst)
  : Maturity(tmpareas, minage, minabslength, size, lgrpdiv), preCalcMaturation(minabslength, size, minage) {

  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  int i = 0;
  keeper->addString("maturity");
  infile >> text;
  if ((strcasecmp(text, "nameofmaturestocksandratio") == 0) || (strcasecmp(text, "maturestocksandratios") == 0)) {
    i = 0;
    infile >> text >> ws;
    while (strcasecmp(text, "coefficients") != 0 && !infile.eof()) {
      matureStockNames.resize(1);
      matureStockNames[i] = new char[strlen(text) + 1];
      strcpy(matureStockNames[i], text);
      Ratio.resize(1);
      infile >> Ratio[i] >> text >> ws;
      i++;
    }
  } else
    handle.logFileUnexpected(LOGFAIL, "maturestocksandratios", text);

  if (infile.eof())
    handle.logFileEOFMessage(LOGFAIL);
  maturityParameters.resize(numMatConst, keeper);
  maturityParameters.read(infile, TimeInfo, keeper);

  infile >> text >> ws;
  if (!((strcasecmp(text, "maturitystep") == 0) || (strcasecmp(text, "maturitysteps") == 0)))
    handle.logFileUnexpected(LOGFAIL, "maturitysteps", text);

  i = 0;
  while (isdigit(infile.peek()) && !infile.eof()) {
    maturitystep.resize(1);
    infile >> maturitystep[i] >> ws;
    i++;
  }

  for (i = 0; i < maturitystep.Size(); i++)
    if (maturitystep[i] < 1 || maturitystep[i] > TimeInfo->numSteps())
      handle.logFileMessage(LOGFAIL, "Error in maturity - invalid maturity step");

  infile >> ws;
  if (!infile.eof()) {
    infile >> text >> ws;
    handle.logFileUnexpected(LOGFAIL, "<end of file>", text);
  }
  if (handle.getLogLevel() >= LOGMESSAGE)
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
    minlength = min(matureStocks[i]->returnLengthGroupDiv()->minLength(), minlength);
  }
  minMatureLength = LgrpDiv->numLengthGroup(minlength);
}

void MaturityC::Reset(const TimeClass* const TimeInfo) {
  maturityParameters.Update(TimeInfo);
  double my;
  int age, len;
  if (maturityParameters.didChange(TimeInfo)) {
    if ((handle.getLogLevel() >= LOGWARN) && (maturityParameters[1] > LgrpDiv->maxLength()))
      handle.logMessage(LOGWARN, "Warning in maturity calculation - l50 greater than maximum length");

    for (age = preCalcMaturation.minRow(); age <= preCalcMaturation.maxRow(); age++) {
      for (len = preCalcMaturation.minCol(age); len < preCalcMaturation.maxCol(age); len++) {
        if ((age >= minMatureAge) && (len >= minMatureLength)) {
          my = exp(-4.0 * maturityParameters[0] * (LgrpDiv->meanLength(len) - maturityParameters[1])
                 - 4.0 * maturityParameters[2] * (age - maturityParameters[3]));
          preCalcMaturation[age][len] = 1.0 / (1.0 + my);
        } else
          preCalcMaturation[age][len] = 0.0;
      }
    }
    if (handle.getLogLevel() >= LOGMESSAGE)
      handle.logMessage(LOGMESSAGE, "Reset maturity data");
  }
}

double MaturityC::MaturationProbability(int age, int length, int growth,
  const TimeClass* const TimeInfo, int area, double weight) {

  if (this->isMaturationStep(area, TimeInfo))
    return (min(max(0.0, preCalcMaturation[age][length]), 1.0));
  return 0.0;
}

int MaturityC::isMaturationStep(int area, const TimeClass* const TimeInfo) {
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
  Keeper* const keeper, int minage, const IntVector& minabslength, const IntVector& size,
  const IntVector& tmpareas, const LengthGroupDivision* const lgrpdiv, int numMatConst, const char* refWeightFile)
  : MaturityC(infile, TimeInfo, keeper, minage, minabslength, size, tmpareas, lgrpdiv, numMatConst) {

  //read information on reference weights.
  ifstream subweightfile;
  subweightfile.open(refWeightFile, ios::in);
  handle.checkIfFailure(subweightfile, refWeightFile);
  handle.Open(refWeightFile);
  CommentStream subweightcomment(subweightfile);
  DoubleMatrix tmpRefW;
  if (!readRefWeights(subweightcomment, tmpRefW))
    handle.logFileMessage(LOGFAIL, "Wrong format for reference weights");
  handle.Close();
  subweightfile.close();
  subweightfile.clear();

  //Aggregate the reference weight data to be the same length groups
  if (LgrpDiv->meanLength(0) < tmpRefW[0][0] ||
      LgrpDiv->meanLength(LgrpDiv->numLengthGroups() - 1) > tmpRefW[tmpRefW.Nrow() - 1][0])
    handle.logFileMessage(LOGFAIL, "Lengths for reference weights must span the range of growth lengths");

  refWeight.resize(LgrpDiv->numLengthGroups(), 0.0);
  int i, j, pos = 0;
  double tmp;
  for (j = 0; j < LgrpDiv->numLengthGroups(); j++) {
    for (i = pos; i < tmpRefW.Nrow() - 1; i++) {
      if (LgrpDiv->meanLength(j) >= tmpRefW[i][0] && LgrpDiv->meanLength(j) <= tmpRefW[i + 1][0]) {
        tmp = (LgrpDiv->meanLength(j) - tmpRefW[i][0]) / (tmpRefW[i + 1][0] - tmpRefW[i][0]);
        refWeight[j] = tmpRefW[i][1] + tmp * (tmpRefW[i + 1][1] - tmpRefW[i][1]);
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
    minlength = min(matureStocks[i]->returnLengthGroupDiv()->minLength(), minlength);
  }
  minMatureLength = LgrpDiv->numLengthGroup(minlength);
}

void MaturityD::Reset(const TimeClass* const TimeInfo) {
  maturityParameters.Update(TimeInfo);
  if (maturityParameters.didChange(TimeInfo)) {
    if ((handle.getLogLevel() >= LOGWARN) && (maturityParameters[1] > LgrpDiv->maxLength()))
      handle.logMessage(LOGWARN, "Warning in maturity calculation - l50 greater than maximum length");
    if (handle.getLogLevel() >= LOGMESSAGE)
      handle.logMessage(LOGMESSAGE, "Reset maturity data");
  }
}

double MaturityD::MaturationProbability(int age, int length, int growth,
  const TimeClass* const TimeInfo, int area, double weight) {

  if ((this->isMaturationStep(area, TimeInfo)) && (age >= minMatureAge) && (length >= minMatureLength)) {
    double tmpweight, my, ratio;

    if ((length >= refWeight.Size()) || (isZero(refWeight[length])))
      tmpweight = maturityParameters[5];
    else
      tmpweight = weight / refWeight[length];

    my = exp(-4.0 * maturityParameters[0] * (LgrpDiv->meanLength(length) - maturityParameters[1])
           - 4.0 * maturityParameters[2] * (age - maturityParameters[3])
           - 4.0 * maturityParameters[4] * (tmpweight - maturityParameters[5]));
    ratio = 1.0 / (1.0 + my);
    return (min(max(0.0, ratio), 1.0));
  }
  return 0.0;
}

int MaturityD::isMaturationStep(int area, const TimeClass* const TimeInfo) {
  int i;
  for (i = 0; i < maturitystep.Size(); i++)
    if (maturitystep[i] == TimeInfo->getStep())
      return 1;
  return 0;
}
