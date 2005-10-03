#include "stray.h"
#include "errorhandler.h"
#include "keeper.h"
#include "areatime.h"
#include "mathfunc.h"
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
    handle.logFileUnexpected(LOGFAIL, "straysteps", text);

  i = 0;
  while (isdigit(infile.peek()) && !infile.eof()) {
    strayStep.resize(1);
    infile >> strayStep[i] >> ws;
    i++;
  }

  for (i = 0; i < strayStep.Size(); i++)
    if (strayStep[i] < 1 || strayStep[i] > TimeInfo->numSteps())
      handle.logFileMessage(LOGFAIL, "invalid straying step", strayStep[i]);

  infile >> text >> ws;
  if (!((strcasecmp(text, "strayarea") == 0) || (strcasecmp(text, "strayareas") == 0)))
    handle.logFileUnexpected(LOGFAIL, "strayareas", text);

  i = 0;
  while (isdigit(infile.peek()) && !infile.eof()) {
    strayArea.resize(1);
    infile >> strayArea[i] >> ws;
    i++;
  }

  for (i = 0; i < strayArea.Size(); i++)
    strayArea[i] = Area->InnerArea(strayArea[i]);

  infile >> text;
  if (strcasecmp(text, "straystocksandratios") == 0) {
    i = 0;
    infile >> text >> ws;
    while (strcasecmp(text, "proportionfunction") != 0 && !infile.eof()) {
      strayStockNames.resize(1, new char[strlen(text) + 1]);
      strcpy(strayStockNames[i], text);
      strayRatio.resize(1);
      infile >> strayRatio[i] >> text >> ws;
      i++;
    }
  } else
    handle.logFileUnexpected(LOGFAIL, "straystocksandratios", text);

  if (infile.eof())
    handle.logFileEOFMessage(LOGFAIL);

  if (strcasecmp(text, "proportionfunction") == 0) {
    infile >> text >> ws;
    if (strcasecmp(text, "constant") == 0)
      fnProportion = new ConstSelectFunc();
    else if (strcasecmp(text, "straightline") == 0)
      fnProportion = new StraightSelectFunc();
    else if (strcasecmp(text, "exponential") == 0)
      fnProportion = new ExpSelectFunc();
    else
      handle.logFileMessage(LOGFAIL, "unrecognised proportion function", text);

    fnProportion->readConstants(infile, TimeInfo, keeper);
  } else
    handle.logFileUnexpected(LOGFAIL, "proportionfunction", text);

  infile >> ws;
  if (!infile.eof()) {
    infile >> text >> ws;
    handle.logFileUnexpected(LOGFAIL, "<end of file>", text);
  }
  handle.logMessage(LOGMESSAGE, "Read straying data file");
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
  int i, j, index;
  DoubleVector tmpratio;

  for (i = 0; i < stockvec.Size(); i++)
    for (j = 0; j < strayStockNames.Size(); j++)
      if (strcasecmp(stockvec[i]->getName(), strayStockNames[j]) == 0) {
        strayStocks.resize(1, stockvec[i]);
        tmpratio.resize(1, strayRatio[j]);
      }

  if (strayStocks.Size() != strayStockNames.Size()) {
    handle.logMessage(LOGWARN, "Error in straying data - failed to match straying stocks");
    for (i = 0; i < stockvec.Size(); i++)
      handle.logMessage(LOGWARN, "Error in straying data - found stock", stockvec[i]->getName());
    for (i = 0; i < strayStockNames.Size(); i++)
      handle.logMessage(LOGWARN, "Error in straying data - looking for stock", strayStockNames[i]);
    exit(EXIT_FAILURE);
  }

  strayRatio.Reset();
  strayRatio.resize(tmpratio.Size());
  for (i = 0; i < tmpratio.Size(); i++)
    strayRatio[i] = tmpratio[i];

  //JMB - check that the straying stocks are defined on the areas
  int minStrayAge = 9999;
  int maxStrayAge = 0;
  double minlength = 9999.0;
  for (i = 0; i < strayStocks.Size(); i++) {
    index = 0;
    for (j = 0; j < strayArea.Size(); j++)
      if (!strayStocks[i]->isInArea(strayArea[j]))
        index++;

    if (index != 0)
      handle.logMessage(LOGWARN, "Warning in straying data - straying stock isnt defined on all areas");

    minStrayAge = min(strayStocks[i]->minAge(), minStrayAge);
    maxStrayAge = max(strayStocks[i]->maxAge(), maxStrayAge);
    minlength = min(strayStocks[i]->getLengthGroupDiv()->minLength(), minlength);
  }

  minStrayLength = LgrpDiv->numLengthGroup(minlength);
  IntVector minlv(maxStrayAge - minStrayAge + 1, 0);
  IntVector sizev(maxStrayAge - minStrayAge + 1, LgrpDiv->numLengthGroups());
  Storage.resize(areas.Size(), minStrayAge, minlv, sizev);
  tagStorage.resize(areas.Size(), minStrayAge, minlv, sizev);
  for (i = 0; i < Storage.Size(); i++) {
    Storage[i].setToZero();
    tagStorage[i].setToZero();
  }

  CI.resize(strayStocks.Size(), 0);
  for (i = 0; i < strayStocks.Size(); i++)
    CI[i] = new ConversionIndex(LgrpDiv, strayStocks[i]->getLengthGroupDiv());

}

void StrayData::storeStrayingStock(int area, AgeBandMatrix& Alkeys,
  AgeBandMatrixRatio& TagAlkeys, const TimeClass* const TimeInfo) {

  int age, len, tag;
  int inarea = this->areaNum(area);
  double straynumber, tagnumber;

  for (age = Storage[inarea].minAge(); age < Storage[inarea].maxAge(); age++) {
    for (len = Storage[inarea].minLength(age); len < Storage[inarea].maxLength(age); len++) {
      straynumber = Alkeys[age][len].N * strayProportion[len];
      Storage[inarea][age][len].N = straynumber;
      Storage[inarea][age][len].W = Alkeys[age][len].W;

      if (len >= minStrayLength)
        Alkeys[age][len].N -= straynumber;

      for (tag = 0; tag < TagAlkeys.numTagExperiments(); tag++) {
        tagnumber = *(TagAlkeys[age][len][tag].N) * strayProportion[len];
        if (tagnumber < verysmall)
          *(tagStorage[inarea][age][len][tag].N) = 0.0;
        else
          *(tagStorage[inarea][age][len][tag].N) = tagnumber;

        if (len >= minStrayLength)
          *(TagAlkeys[age][len][tag].N) -= tagnumber;
      }

    }
  }
  TagAlkeys.updateRatio(Alkeys);
}

void StrayData::addStrayStock(int area, const TimeClass* const TimeInfo) {

  int s, inarea = this->areaNum(area);
  for (s = 0; s < strayStocks.Size(); s++) {
    if (!strayStocks[s]->isInArea(area))
      handle.logMessage(LOGFAIL, "Error in straying - stray stock doesnt live on area", area);

    if (strayStocks[s]->isBirthday(TimeInfo)) {
      Storage[inarea].IncrementAge();
      if (tagStorage.numTagExperiments() > 0)
        tagStorage[inarea].IncrementAge(Storage[inarea]);
    }

    strayStocks[s]->Add(Storage[inarea], CI[s], area, strayRatio[s]);

    if (tagStorage.numTagExperiments() > 0)
      strayStocks[s]->Add(tagStorage, CI[s], area, strayRatio[s]);
  }
}

int StrayData::isStrayStepArea(int area, const TimeClass* const TimeInfo) {
  int i, j;

  for (i = 0; i < strayStep.Size(); i++)
    for (j = 0; j < strayArea.Size(); j++)
      if ((strayStep[i] == TimeInfo->getStep()) && (strayArea[j] == area))
        return 1;
  return 0;
}

void StrayData::Reset(const TimeClass* const TimeInfo) {
  int i;

  fnProportion->updateConstants(TimeInfo);
  if (fnProportion->didChange(TimeInfo)) {
    for (i = 0; i < LgrpDiv->numLengthGroups(); i++) {
      strayProportion[i] = fnProportion->calculate(LgrpDiv->meanLength(i));
      if (strayProportion[i] < 0.0) {
        handle.logMessage(LOGWARN, "Warning in straying - function outside bounds", strayProportion[i]);
        strayProportion[i] = 0.0;
      }
      if (strayProportion[i] > 1.0) {
        handle.logMessage(LOGWARN, "Warning in straying - function outside bounds", strayProportion[i]);
        strayProportion[i] = 1.0;
      }
    }
  }

  for (i = 0; i < Storage.Size(); i++) {
    Storage[i].setToZero();
    tagStorage[i].setToZero();
  }
  if (handle.getLogLevel() >= LOGMESSAGE)
    handle.logMessage(LOGMESSAGE, "Reset straying data");
}

void StrayData::Print(ofstream& outfile) const {
  int i;
  outfile << "\nStraying information\n\tNames of straying stocks:";
  for (i = 0; i < strayStockNames.Size(); i++)
    outfile << sep << strayStockNames[i];
  outfile << "\n\tRatio moving into each stock:";
  for (i = 0; i < strayRatio.Size(); i++)
    outfile << sep << strayRatio[i];
  outfile << "\n\tStraying timesteps:";
  for (i = 0; i < strayStep.Size(); i++)
    outfile << sep << strayStep[i];
  outfile << endl;
}

const StockPtrVector& StrayData::getStrayStocks() {
  return strayStocks;
}

void StrayData::addStrayTag(const char* tagname) {
  tagStorage.addTag(tagname);
}

void StrayData::deleteStrayTag(const char* tagname) {
  int minage, maxage, minlen, maxlen, a, length, i;
  int id = tagStorage.getTagID(tagname);

  if (id >= 0) {
    minage = tagStorage[0].minAge();
    maxage = tagStorage[0].maxAge();

    // Remove allocated memory
    for (i = 0; i < tagStorage.Size(); i++) {
      for (a = minage; a <= maxage; a++) {
        minlen = tagStorage[i].minLength(a);
        maxlen = tagStorage[i].maxLength(a);
        for (length = minlen; length < maxlen; length++) {
          delete[] (tagStorage[i][a][length][id].N);
          (tagStorage[i][a][length][id].N) = NULL;
        }
      }
    }
    tagStorage.deleteTag(tagname);

  } else
    handle.logMessage(LOGWARN, "Warning in stray - failed to delete tagging experiment", tagname);
}
