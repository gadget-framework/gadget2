#include "stray.h"
#include "errorhandler.h"
#include "keeper.h"
#include "areatime.h"
#include "readfunc.h"
#include "readword.h"
#include "readaggregation.h"
#include "gadget.h"

extern ErrorHandler handle;

StrayData::StrayData(CommentStream& infile, const LengthGroupDivision* const lgrpdiv,
  const IntVector& areas, const AreaClass* const Area, const TimeClass* const TimeInfo,
  Keeper* const keeper) : LivesOnAreas(areas) {

  keeper->addString("stray");
  int i;

  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  ifstream datafile;
  CommentStream subdata(datafile);

  LgrpDiv = new LengthGroupDivision(*lgrpdiv);
  strayProportion.resize(LgrpDiv->numLengthGroups(), 0.0);

  infile >> text >> ws;
  if (!((strcasecmp(text, "straystep") == 0) || (strcasecmp(text, "straysteps") == 0)))
    handle.Unexpected("straysteps", text);

  i = 0;
  while (isdigit(infile.peek()) && !infile.eof()) {
    strayStep.resize(1);
    infile >> strayStep[i] >> ws;
    i++;
  }

  for (i = 0; i < strayStep.Size(); i++)
    if (strayStep[i] < 1 || strayStep[i] > TimeInfo->StepsInYear())
      handle.Message("Error in straying data - invalid straying step");

  infile >> text >> ws;
  if (!((strcasecmp(text, "strayarea") == 0) || (strcasecmp(text, "strayareas") == 0)))
    handle.Unexpected("strayareas", text);

  i = 0;
  while (isdigit(infile.peek()) && !infile.eof()) {
    strayArea.resize(1);
    infile >> strayArea[i] >> ws;
    i++;
  }

  for (i = 0; i < strayArea.Size(); i++)
    if ((strayArea[i] = Area->InnerArea(strayArea[i])) == -1)
      handle.UndefinedArea(strayArea[i]);

  infile >> text;
  if (strcasecmp(text, "straystocksandratios") == 0) {
    infile >> text >> ws;
    i = 0;
    while (strcasecmp(text, "proportionfunction") != 0 && infile.good()) {
      strayStockNames.resize(1);
      strayStockNames[i] = new char[strlen(text) + 1];
      strcpy(strayStockNames[i], text);
      Ratio.resize(1);
      infile >> Ratio[i];
      i++;
      infile >> text >> ws;
    }
  } else
    handle.Unexpected("straystocksandratios", text);

  if (!infile.good())
    handle.Failure();

  if (strcasecmp(text, "proportionfunction") == 0) {
    infile >> text >> ws;
    if (strcasecmp(text, "constant") == 0)
      fnProportion = new ConstSelectFunc();
    else if (strcasecmp(text, "straightline") == 0)
      fnProportion = new StraightSelectFunc();
    else if (strcasecmp(text, "exponential") == 0)
      fnProportion = new ExpSelectFunc();
    else
      handle.Message("Error in straying data - unrecognised proportion function", text);

    fnProportion->readConstants(infile, TimeInfo, keeper);
  } else
    handle.Unexpected("proportionfunction", text);

  infile >> ws;
  if (!infile.eof()) {
    infile >> text >> ws;
    handle.Unexpected("<end of file>", text);
  }
  handle.logMessage("Read straying data file");
  keeper->clearLast();
}

StrayData::~StrayData() {
  int i;
  for (i = 0; i < strayStockNames.Size(); i++)
    delete[] strayStockNames[i];
  for (i = 0; i < CI.Size(); i++)
    delete CI[i];
  delete LgrpDiv;
  delete fnProportion;
}

void StrayData::setStock(StockPtrVector& stockvec) {
  int index = 0;
  int i, j;
  DoubleVector tmpratio;

  for (i = 0; i < stockvec.Size(); i++)
    for (j = 0; j < strayStockNames.Size(); j++)
      if (strcasecmp(stockvec[i]->Name(), strayStockNames[j]) == 0) {
        strayStocks.resize(1);
        tmpratio.resize(1);
        strayStocks[index] = stockvec[i];
        tmpratio[index] = Ratio[j];
        index++;
      }

  if (index != strayStockNames.Size()) {
    handle.logWarning("Error in straying data - failed to match straying stocks");
    for (i = 0; i < stockvec.Size(); i++)
      handle.logWarning("Error in straying data - found stock", stockvec[i]->Name());
    for (i = 0; i < strayStockNames.Size(); i++)
      handle.logWarning("Error in straying data - looking for stock", strayStockNames[i]);
    exit(EXIT_FAILURE);
  }

  for (i = Ratio.Size(); i > 0; i--)
    Ratio.Delete(0);
  Ratio.resize(tmpratio.Size());
  for (i = 0; i < tmpratio.Size(); i++)
    Ratio[i] = tmpratio[i];

  //JMB - check that the straying stocks are defined on the areas
  int minStrayAge = 9999;
  int maxStrayAge = 0;
  double minlength = 9999.0;
  for (i = 0; i < strayStocks.Size(); i++) {
    index = 0;
    for (j = 0; j < strayArea.Size(); j++)
      if (!strayStocks[i]->IsInArea(strayArea[j]))
        index++;

    if (index != 0)
      handle.logWarning("Warning in straying data - straying stock isnt defined on all areas");

    if (strayStocks[i]->minAge() < minStrayAge)
      minStrayAge = strayStocks[i]->minAge();
    if (strayStocks[i]->maxAge() > maxStrayAge)
      maxStrayAge = strayStocks[i]->maxAge();
    if (strayStocks[i]->returnLengthGroupDiv()->minLength() < minlength)
      minlength = strayStocks[i]->returnLengthGroupDiv()->minLength();
  }

  minStrayLength = LgrpDiv->numLengthGroup(minlength);
  IntVector minlv(maxStrayAge - minStrayAge + 1, 0);
  IntVector sizev(maxStrayAge - minStrayAge + 1, LgrpDiv->numLengthGroups());
  Storage.resize(areas.Size(), minStrayAge, minlv, sizev);
  TagStorage.resize(areas.Size(), minStrayAge, minlv, sizev);
  for (i = 0; i < Storage.Size(); i++) {
    Storage[i].setToZero();
    TagStorage[i].setToZero();
  }

  CI.resize(strayStocks.Size(), 0);
  for (i = 0; i < strayStocks.Size(); i++)
    CI[i] = new ConversionIndex(LgrpDiv, strayStocks[i]->returnLengthGroupDiv());

}

void StrayData::storeStrayingStock(int area, AgeBandMatrix& Alkeys,
  AgeBandMatrixRatio& TagAlkeys, const TimeClass* const TimeInfo) {

  if (this->isStrayStepArea(area, TimeInfo) == 0)
    return;

  int age, len, tag;
  double straynumber, tagnumber;

  for (age = Storage[area].minAge(); age < Storage[area].maxAge(); age++) {
    for (len = Storage[area].minLength(age); len < Storage[area].maxLength(age); len++) {
      straynumber = Alkeys[age][len].N * strayProportion[len];
      Storage[area][age][len].N = straynumber;
      Storage[area][age][len].W = Alkeys[age][len].W;

      if (len >= minStrayLength)
        Alkeys[age][len].N -= straynumber;

      for (tag = 0; tag < TagAlkeys.numTagExperiments(); tag++) {
        tagnumber = *(TagAlkeys[age][len][tag].N) * strayProportion[len];
        if (tagnumber < verysmall)
          *(TagStorage[area][age][len][tag].N) = 0.0;
        else
          *(TagStorage[area][age][len][tag].N) = tagnumber;

        if (len >= minStrayLength)
          *(TagAlkeys[age][len][tag].N) -= tagnumber;
      }

    }
  }
  TagAlkeys.updateRatio(Alkeys);
}

void StrayData::addStrayStock(int area, const TimeClass* const TimeInfo) {

  if (this->isStrayStepArea(area, TimeInfo) == 0)
    return;

  int s;
  for (s = 0; s < strayStocks.Size(); s++) {
    if (!strayStocks[s]->IsInArea(area))
      handle.logFailure("Error in straying - stray stock doesnt live on area", area);

    if (strayStocks[s]->Birthday(TimeInfo)) {
      Storage[area].IncrementAge();
      if (TagStorage.numTagExperiments() > 0)
        TagStorage[area].IncrementAge(Storage[area]);
    }

    strayStocks[s]->Add(Storage[area], CI[s], area, Ratio[s],
      Storage[area].minAge(), Storage[area].maxAge());

    if (TagStorage.numTagExperiments() > 0)
      strayStocks[s]->Add(TagStorage, area, CI[s], area, Ratio[s],
        TagStorage[area].minAge(), TagStorage[area].maxAge());
  }
}

int StrayData::isStrayStepArea(int area, const TimeClass* const TimeInfo) {
  int i, j;

  for (i = 0; i < strayStep.Size(); i++)
    for (j = 0; j < strayArea.Size(); j++)
      if ((strayStep[i] == TimeInfo->CurrentStep()) && (strayArea[j] == area))
        return 1;
  return 0;
}

void StrayData::Reset(const TimeClass* const TimeInfo) {
  int i;

  fnProportion->updateConstants(TimeInfo);
  if (fnProportion->constantsHaveChanged(TimeInfo)) {
    for (i = 0; i < LgrpDiv->numLengthGroups(); i++) {
      strayProportion[i] = fnProportion->calculate(LgrpDiv->meanLength(i));
      if (strayProportion[i] < 0.0) {
        handle.logWarning("Warning in straying - function outside bounds", strayProportion[i]);
        strayProportion[i] = 0.0;
      }
      if (strayProportion[i] > 1.0) {
        handle.logWarning("Warning in straying - function outside bounds", strayProportion[i]);
        strayProportion[i] = 1.0;
      }
    }
  }

  for (i = 0; i < Storage.Size(); i++) {
    Storage[i].setToZero();
    TagStorage[i].setToZero();
  }
  handle.logMessage("Reset straying data");
}

void StrayData::Print(ofstream& outfile) const {
  int i;
  outfile << "\nStraying information\n\tNames of straying stocks:";
  for (i = 0; i < strayStocks.Size(); i++)
    outfile << sep << (const char*)(strayStocks[i]->Name());
  outfile << "\n\tRatio moving into each stock:";
  for (i = 0; i < Ratio.Size(); i++)
    outfile << sep << Ratio[i];
  outfile << "\n\tStraying timesteps:";
  for (i = 0; i < strayStep.Size(); i++)
    outfile << sep << strayStep[i];
  outfile << endl;
}

const StockPtrVector& StrayData::getStrayStocks() {
  return strayStocks;
}

void StrayData::addStrayTag(const char* tagname) {
  TagStorage.addTag(tagname);
}

void StrayData::deleteStrayTag(const char* tagname) {
  int minage, maxage, minlen, maxlen, a, length, i;
  int id = TagStorage.getID(tagname);

  if (id >= 0) {
    minage = TagStorage[0].minAge();
    maxage = TagStorage[0].maxAge();

    // Remove allocated memory
    for (i = 0; i < TagStorage.Size(); i++) {
      for (a = minage; a <= maxage; a++) {
        minlen = TagStorage[i].minLength(a);
        maxlen = TagStorage[i].maxLength(a);
        for (length = minlen; length < maxlen; length++) {
          delete[] (TagStorage[i][a][length][id].N);
          (TagStorage[i][a][length][id].N) = NULL;
        }
      }
    }
    TagStorage.deleteTag(tagname);

  } else
    handle.logWarning("Warning in stray - failed to delete tagging experiment", tagname);
}
