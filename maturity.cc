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
    TagStorage(tmpareas.Size(), minage, minlength, size) {

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
      if (strcasecmp(stockvec[i]->Name(), matureStockNames[j]) == 0) {
        matureStocks.resize(1);
        tmpratio.resize(1);
        matureStocks[index] = stockvec[i];
        tmpratio[index] = Ratio[j];
        index++;
      }

  if (index != matureStockNames.Size()) {
    handle.logWarning("Error in maturity - failed to match mature stocks");
    for (i = 0; i < stockvec.Size(); i++)
      handle.logWarning("Error in maturity - found stock", stockvec[i]->Name());
    for (i = 0; i < matureStockNames.Size(); i++)
      handle.logWarning("Error in maturity - looking for stock", matureStockNames[i]);
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
      if (!matureStocks[i]->IsInArea(areas[j]))
        index++;

    if (index != 0)
      handle.logWarning("Warning in maturity - mature stock isnt defined on all areas");
  }

  for (i = 0; i < areas.Size(); i++) {
    Storage[i].setToZero();
    TagStorage[i].setToZero();
  }
}

void Maturity::Print(ofstream& outfile) const {
  int i;
  outfile << "\nMaturity\n\tRead names of mature stocks:";
  for (i = 0; i < matureStockNames.Size(); i++)
    outfile << sep << (const char*)(matureStockNames[i]);
  outfile << "\n\tNames of mature stocks (through pointers):";
  for (i = 0; i < matureStocks.Size(); i++)
    outfile << sep << (const char*)(matureStocks[i]->Name());
  outfile << "\n\tStored numbers:\n";
  for (i = 0; i < areas.Size(); i++) {
    outfile << "\tInternal area " << areas[i] << endl;
    Storage[i].printNumbers(outfile);
  }
}

void Maturity::Move(int area, const TimeClass* const TimeInfo) {
  if (!(this->IsMaturationStep(area, TimeInfo)))
    handle.logFailure("Error in maturity - maturity requested on wrong timestep");
  int i, inarea = AreaNr[area];
  for (i = 0; i < matureStocks.Size(); i++) {
    if (!matureStocks[i]->IsInArea(area))
      handle.logFailure("Error in maturity - mature stock doesnt live on area", area);

    matureStocks[i]->Add(Storage[inarea], CI[i], area, Ratio[i],
      Storage[inarea].minAge(), Storage[inarea].maxAge());

    if (TagStorage.numTagExperiments() > 0)
      matureStocks[i]->Add(TagStorage, inarea, CI[i], area, Ratio[i],
        TagStorage[inarea].minAge(), TagStorage[inarea].maxAge());
  }

  Storage[inarea].setToZero();
  TagStorage[inarea].setToZero();
}

void Maturity::PutInStorage(int area, int age, int length, double number,
  double weight, const TimeClass* const TimeInfo) {

  if (!(this->IsMaturationStep(area, TimeInfo)))
    handle.logFailure("Error in maturity - maturity requested on wrong timestep");
  int inarea = AreaNr[area];
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

  if (!(this->IsMaturationStep(area, TimeInfo)))
    handle.logFailure("Error in maturity - maturity requested on wrong timestep");
  if (TagStorage.numTagExperiments() <= 0)
    handle.logFailure("Error in maturity - no tagging experiment");
  else if ((id >= TagStorage.numTagExperiments()) || (id < 0))
    handle.logFailure("Error in maturity - illegal tagging experiment");
  else
    *(TagStorage[AreaNr[area]][age][length][id].N) = (number > 0.0 ? number: 0.0);
}

const StockPtrVector& Maturity::getMatureStocks() {
  return matureStocks;
}

void Maturity::addMaturityTag(const char* tagname) {
  TagStorage.addTag(tagname);
}

void Maturity::deleteMaturityTag(const char* tagname) {
  int minage, maxage, minlen, maxlen, age, length, i;
  int id = TagStorage.getID(tagname);

  if (id >= 0) {
    minage = TagStorage[0].minAge();
    maxage = TagStorage[0].maxAge();
    // Remove allocated memory
    for (i = 0; i < TagStorage.Size(); i++) {
      for (age = minage; age <= maxage; age++) {
        minlen = TagStorage[i].minLength(age);
        maxlen = TagStorage[i].maxLength(age);
        for (length = minlen; length < maxlen; length++) {
          delete[] (TagStorage[i][age][length][id].N);
          (TagStorage[i][age][length][id].N) = NULL;
        }
      }
    }
    TagStorage.deleteTag(tagname);

  } else
    handle.logWarning("Warning in maturity - failed to delete tagging experiment", tagname);
}

// ********************************************************
// Functions for MaturityA
// ********************************************************
MaturityA::MaturityA(CommentStream& infile, const TimeClass* const TimeInfo,
  Keeper* const keeper, int minage, const IntVector& minabslength, const IntVector& size,
  const IntVector& tmpareas, const LengthGroupDivision* const lgrpdiv, int numMatConst)
  : Maturity(tmpareas, minage, minabslength, size, lgrpdiv), PrecalcMaturation(minabslength, size, minage) {

  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  int i;
  keeper->addString("maturity");
  infile >> text;
  if ((strcasecmp(text, "nameofmaturestocksandratio") == 0) || (strcasecmp(text, "maturestocksandratios") == 0)) {
    infile >> text;
    i = 0;
    while (strcasecmp(text, "coefficients") != 0 && infile.good()) {
      matureStockNames.resize(1);
      matureStockNames[i] = new char[strlen(text) + 1];
      strcpy(matureStockNames[i], text);
      Ratio.resize(1);
      infile >> Ratio[i];
      i++;
      infile >> text;
    }
  } else
    handle.Unexpected("maturestocksandratios", text);

  if (!infile.good())
    handle.Failure();
  maturityParameters.resize(numMatConst, keeper);
  maturityParameters.read(infile, TimeInfo, keeper);

  infile >> ws;
  if (!infile.eof()) {
    infile >> text >> ws;
    handle.Unexpected("<end of file>", text);
  }
  handle.logMessage("Read maturity data file");
  keeper->clearLast();
}

void MaturityA::Reset(const TimeClass* const TimeInfo) {
  maturityParameters.Update(TimeInfo);
  double my;
  int age, len;
  if (maturityParameters.DidChange(TimeInfo)) {
    for (age = PrecalcMaturation.minAge(); age <= PrecalcMaturation.maxAge(); age++) {
      for (len = PrecalcMaturation.minLength(age); len < PrecalcMaturation.maxLength(age); len++) {
        my = exp(-maturityParameters[0] * (LgrpDiv->meanLength(len) - maturityParameters[1])
               - maturityParameters[2] * (age - maturityParameters[3]));
        PrecalcMaturation[age][len] = 1.0 / (1.0 + my);
      }
    }
  }
  handle.logMessage("Reset maturity data");
}

void MaturityA::setStock(StockPtrVector& stockvec) {
  this->Maturity::setStock(stockvec);

  int i;
  minMatureAge = 9999;
  double minlength = 9999.0;
  for (i = 0; i < matureStocks.Size(); i++) {
    if (matureStocks[i]->minAge() < minMatureAge)
      minMatureAge = matureStocks[i]->minAge();
    if (matureStocks[i]->returnLengthGroupDiv()->minLength() < minlength)
      minlength = matureStocks[i]->returnLengthGroupDiv()->minLength();
  }
  minMatureLength = LgrpDiv->NoLengthGroup(minlength);
}

double MaturityA::MaturationProbability(int age, int length, int growth,
  const TimeClass* const TimeInfo, int area, double weight) {

  if ((age >= minMatureAge) && ((length + growth) >= minMatureLength)) {
    double prob = PrecalcMaturation[age][length] * (maturityParameters[0] * growth * LgrpDiv->dl()
                  + maturityParameters[2] * TimeInfo->LengthOfCurrent() / TimeInfo->LengthOfYear());
    return (min(max(0.0, prob), 1.0));
  }
  return 0.0;
}

void MaturityA::Print(ofstream& outfile) const {
  Maturity::Print(outfile);
  outfile << "\tPrecalculated maturity:\n";
  PrecalcMaturation.Print(outfile);
}

int MaturityA::IsMaturationStep(int area, const TimeClass* const TimeInfo) {
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
    infile >> text;
    i = 0;
    while (!(strcasecmp(text, "maturitysteps") == 0) && infile.good()) {
      matureStockNames.resize(1);
      matureStockNames[i] = new char[strlen(text) + 1];
      strcpy(matureStockNames[i], text);
      Ratio.resize(1);
      infile >> Ratio[i];
      i++;
      infile >> text;
    }
  } else
    handle.Unexpected("maturestocksandratios", text);

  if (!infile.good())
    handle.Failure();
  infile >> ws;
  while (isdigit(infile.peek()) && !infile.eof()) {
    maturitystep.resize(1);
    infile >> maturitystep[maturitystep.Size() - 1] >> ws;
  }
  infile >> text;
  if (!(strcasecmp(text, "maturitylengths") == 0))
    handle.Unexpected("maturitylengths", text);
  while (maturitylength.Size() < maturitystep.Size() && !infile.eof()) {
    maturitylength.resize(1, keeper);
    maturitylength[maturitylength.Size() - 1].read(infile, TimeInfo, keeper);
  }

  if (maturitylength.Size() != maturitystep.Size())
    handle.Message("Number of maturitysteps does not equal number of maturitylengths");

  infile >> ws;
  if (!infile.eof()) {
    infile >> text >> ws;
    handle.Unexpected("<end of file>", text);
  }
  handle.logMessage("Read maturity data file");
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
  this->Maturity::setStock(stockvec);
}

void MaturityB::Reset(const TimeClass* const TimeInfo) {
  maturitylength.Update(TimeInfo);
  handle.logMessage("Reset maturity data");
}

double MaturityB::MaturationProbability(int age, int length, int growth,
  const TimeClass* const TimeInfo, int area, double weight) {

  int i;
  for (i = 0; i < maturitylength.Size(); i++)
    if (TimeInfo->CurrentStep() == maturitystep[i])
      if (LgrpDiv->meanLength(length) >= maturitylength[i])
        return 1.0;

  return 0.0;
}

int MaturityB::IsMaturationStep(int area, const TimeClass* const TimeInfo) {
  int i;
  for (i = 0; i < maturitystep.Size(); i++)
    if (maturitystep[i] == TimeInfo->CurrentStep())
      return 1;
  return 0;
}

// ********************************************************
// Functions for MaturityC
// ********************************************************
MaturityC::MaturityC(CommentStream& infile, const TimeClass* const TimeInfo,
  Keeper* const keeper, int minage, const IntVector& minabslength, const IntVector& size,
  const IntVector& tmpareas, const LengthGroupDivision* const lgrpdiv, int numMatConst)
  : Maturity(tmpareas, minage, minabslength, size, lgrpdiv), PrecalcMaturation(minabslength, size, minage) {

  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  int i = 0;
  keeper->addString("maturity");
  infile >> text;
  if ((strcasecmp(text, "nameofmaturestocksandratio") == 0) || (strcasecmp(text, "maturestocksandratios") == 0)) {
    infile >> text;
    i = 0;
    while (strcasecmp(text, "coefficients") != 0 && infile.good()) {
      matureStockNames.resize(1);
      matureStockNames[i] = new char[strlen(text) + 1];
      strcpy(matureStockNames[i], text);
      Ratio.resize(1);
      infile >> Ratio[i];
      i++;
      infile >> text;
    }
  } else
    handle.Unexpected("maturestocksandratios", text);

  if (!infile.good())
    handle.Failure();
  maturityParameters.resize(numMatConst, keeper);
  maturityParameters.read(infile, TimeInfo, keeper);

  infile >> text >> ws;
  if (!((strcasecmp(text, "maturitystep") == 0) || (strcasecmp(text, "maturitysteps") == 0)))
    handle.Unexpected("maturitysteps", text);

  i = 0;
  while (isdigit(infile.peek()) && !infile.eof()) {
    maturitystep.resize(1);
    infile >> maturitystep[i] >> ws;
    i++;
  }

  for (i = 0; i < maturitystep.Size(); i++)
    if (maturitystep[i] < 1 || maturitystep[i] > TimeInfo->StepsInYear())
      handle.Message("Illegal maturity step in maturation function");

  infile >> ws;
  if (!infile.eof()) {
    infile >> text >> ws;
    handle.Unexpected("<end of file>", text);
  }
  handle.logMessage("Read maturity data file");
  keeper->clearLast();
}

void MaturityC::setStock(StockPtrVector& stockvec) {
  this->Maturity::setStock(stockvec);

  int i;
  minMatureAge = 9999;
  double minlength = 9999.0;
  for (i = 0; i < matureStocks.Size(); i++) {
    if (matureStocks[i]->minAge() < minMatureAge)
      minMatureAge = matureStocks[i]->minAge();
    if (matureStocks[i]->returnLengthGroupDiv()->minLength() < minlength)
      minlength = matureStocks[i]->returnLengthGroupDiv()->minLength();
  }
  minMatureLength = LgrpDiv->NoLengthGroup(minlength);
}

void MaturityC::Reset(const TimeClass* const TimeInfo) {
  maturityParameters.Update(TimeInfo);
  double my;
  int age, len;
  if (maturityParameters.DidChange(TimeInfo)) {
    for (age = PrecalcMaturation.minAge(); age <= PrecalcMaturation.maxAge(); age++) {
      for (len = PrecalcMaturation.minLength(age); len < PrecalcMaturation.maxLength(age); len++) {
        if ((age >= minMatureAge) && (len >= minMatureLength)) {
          my = exp(-4.0 * maturityParameters[0] * (LgrpDiv->meanLength(len) - maturityParameters[1])
                 - 4.0 * maturityParameters[2] * (age - maturityParameters[3]));
          PrecalcMaturation[age][len] = 1.0 / (1.0 + my);
        } else
          PrecalcMaturation[age][len] = 0.0;
      }
    }
  }
  handle.logMessage("Reset maturity data");
}

double MaturityC::MaturationProbability(int age, int length, int growth,
  const TimeClass* const TimeInfo, int area, double weight) {

  if (this->IsMaturationStep(area, TimeInfo))
    return (min(max(0.0, PrecalcMaturation[age][length]), 1.0));
  return 0.0;
}

int MaturityC::IsMaturationStep(int area, const TimeClass* const TimeInfo) {
  int i;
  for (i = 0; i < maturitystep.Size(); i++)
    if (maturitystep[i] == TimeInfo->CurrentStep())
      return 1;
  return 0;
}

void MaturityC::Print(ofstream& outfile) const {
  int i;
  Maturity::Print(outfile);
  outfile << "\tPrecalculated maturity:\n";
  PrecalcMaturation.Print(outfile);
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
    handle.Message("Wrong format for reference weights");
  handle.Close();
  subweightfile.close();
  subweightfile.clear();

  //Aggregate the reference weight data to be the same length groups
  if (LgrpDiv->meanLength(0) < tmpRefW[0][0] ||
      LgrpDiv->meanLength(LgrpDiv->NoLengthGroups() - 1) > tmpRefW[tmpRefW.Nrow() - 1][0])
    handle.Message("Lengths for reference weights must span the range of growth lengths");

  refWeight.resize(LgrpDiv->NoLengthGroups(), 0.0);
  int i, j, pos = 0;
  double tmp;
  for (j = 0; j < LgrpDiv->NoLengthGroups(); j++) {
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
  this->Maturity::setStock(stockvec);

  int i;
  minMatureAge = 9999;
  double minlength = 9999.0;
  for (i = 0; i < matureStocks.Size(); i++) {
    if (matureStocks[i]->minAge() < minMatureAge)
      minMatureAge = matureStocks[i]->minAge();
    if (matureStocks[i]->returnLengthGroupDiv()->minLength() < minlength)
      minlength = matureStocks[i]->returnLengthGroupDiv()->minLength();
  }
  minMatureLength = LgrpDiv->NoLengthGroup(minlength);
}

void MaturityD::Reset(const TimeClass* const TimeInfo) {
  maturityParameters.Update(TimeInfo);
  handle.logMessage("Reset maturity data");
}

double MaturityD::MaturationProbability(int age, int length, int growth,
  const TimeClass* const TimeInfo, int area, double weight) {

  if ((this->IsMaturationStep(area, TimeInfo)) && (age >= minMatureAge) && (length >= minMatureLength)) {
    double tmpweight, my, ratio;

    if (length >= refWeight.Size())
      tmpweight = maturityParameters[5];
    else if (isZero(refWeight[length]))
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

int MaturityD::IsMaturationStep(int area, const TimeClass* const TimeInfo) {
  int i;
  for (i = 0; i < maturitystep.Size(); i++)
    if (maturitystep[i] == TimeInfo->CurrentStep())
      return 1;
  return 0;
}
