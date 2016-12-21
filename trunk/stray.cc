#include "stray.h"
#include "errorhandler.h"
#include "keeper.h"
#include "areatime.h"
#include "mathfunc.h"
#include "readfunc.h"
#include "readword.h"
#include "readaggregation.h"
#include "gadget.h"
#include "global.h"

StrayData::StrayData(CommentStream& infile, const LengthGroupDivision* const lgrpdiv,
  const IntVector& areas, const AreaClass* const Area, const char* givenname,
  const TimeClass* const TimeInfo, Keeper* const keeper) : HasName(givenname), LivesOnAreas(areas) {

  keeper->addString("stray");
  int i, tmpint = 0;
  istagged = 0;
  ratioscale = 1.0; //JMB used to scale the ratios to ensure that they sum to 1

  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  ifstream datafile;
  CommentStream subdata(datafile);

  LgrpDiv = new LengthGroupDivision(*lgrpdiv);
  if (LgrpDiv->Error())
    handle.logMessage(LOGFAIL, "Error in straying data - failed to create length group");
  strayProportion.resize(LgrpDiv->numLengthGroups(), 0.0);

  infile >> text >> ws;
  if ((strcasecmp(text, "straystep") != 0) && (strcasecmp(text, "straysteps") != 0))
    handle.logFileUnexpected(LOGFAIL, "straysteps", text);

  while (isdigit(infile.peek()) && !infile.eof()) {
    infile >> tmpint >> ws;
    strayStep.resize(1, tmpint);
  }

  for (i = 0; i < strayStep.Size(); i++)
    if (strayStep[i] < 1 || strayStep[i] > TimeInfo->numSteps())
      handle.logFileMessage(LOGFAIL, "invalid straying step", strayStep[i]);

  infile >> text >> ws;
  if ((strcasecmp(text, "strayarea") != 0) && (strcasecmp(text, "strayareas") != 0))
    handle.logFileUnexpected(LOGFAIL, "strayareas", text);

  while (isdigit(infile.peek()) && !infile.eof()) {
    infile >> tmpint >> ws;
    strayArea.resize(1, tmpint);
  }

  for (i = 0; i < strayArea.Size(); i++)
    strayArea[i] = Area->getInnerArea(strayArea[i]);

  infile >> text >> ws;
  if (strcasecmp(text, "straystocksandratios") != 0)
    handle.logFileUnexpected(LOGFAIL, "straystocksandratios", text);

  i = 0;
  infile >> text >> ws;
  while (strcasecmp(text, "proportionfunction") != 0 && !infile.eof()) {
    strayStockNames.resize(new char[strlen(text) + 1]);
    strcpy(strayStockNames[i], text);
    strayRatio.resize(1, keeper);
    if (!(infile >> strayRatio[i]))
      handle.logFileMessage(LOGFAIL, "invalid format for stray ratio");
    strayRatio[i].Inform(keeper);

    infile >> text >> ws;
    i++;
  }

  if (infile.eof())
    handle.logFileEOFMessage(LOGFAIL);
  if (strcasecmp(text, "proportionfunction") != 0)
    handle.logFileUnexpected(LOGFAIL, "proportionfunction", text);

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

  for (i = 0; i < strayStockNames.Size(); i++)
    for (j = 0; j < strayStockNames.Size(); j++)
      if ((strcasecmp(strayStockNames[i], strayStockNames[j]) == 0) && (i != j))
        handle.logMessage(LOGFAIL, "Error in straying data - repeated stock", strayStockNames[i]);

  for (i = 0; i < stockvec.Size(); i++)
    for (j = 0; j < strayStockNames.Size(); j++)
      if (strcasecmp(stockvec[i]->getName(), strayStockNames[j]) == 0)
        strayStocks.resize(stockvec[i]);

  if (strayStocks.Size() != strayStockNames.Size()) {
    handle.logMessage(LOGWARN, "Error in straying data - failed to match straying stocks");
    for (i = 0; i < stockvec.Size(); i++)
      handle.logMessage(LOGWARN, "Error in straying data - found stock", stockvec[i]->getName());
    for (i = 0; i < strayStockNames.Size(); i++)
      handle.logMessage(LOGWARN, "Error in straying data - looking for stock", strayStockNames[i]);
    handle.logMessage(LOGFAIL, ""); //JMB this will exit gadget
  }

  //JMB ensure that the ratio vector is indexed in the right order
  ratioindex.resize(strayStocks.Size(), 0);
  for (i = 0; i < strayStocks.Size(); i++)
    for (j = 0; j < strayStockNames.Size(); j++)
      if (strcasecmp(strayStocks[i]->getName(), strayStockNames[j]) == 0)
        ratioindex[i] = j;

  //JMB check that the straying stocks are defined on all the areas
  int minStrayAge = 9999;
  int maxStrayAge = 0;
  double minlength = 9999.0;
  for (i = 0; i < strayStocks.Size(); i++) {
    CI.resize(new ConversionIndex(LgrpDiv, strayStocks[i]->getLengthGroupDiv()));
    if (CI[i]->Error())
      handle.logMessage(LOGFAIL, "Error in straying data - error when checking length structure");
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
  for (i = 0; i < Storage.Size(); i++)
    Storage[i].setToZero();
}

void StrayData::storeStrayingStock(int area, AgeBandMatrix& Alkeys, const TimeClass* const TimeInfo) {

  int age, len;
  int inarea = this->areaNum(area);
  double straynumber;

  for (age = Storage[inarea].minAge(); age < Storage[inarea].maxAge(); age++) {
    for (len = Storage[inarea].minLength(age); len < Storage[inarea].maxLength(age); len++) {
      straynumber = Alkeys[age][len].N * strayProportion[len];
      Storage[inarea][age][len].N = straynumber;
      Storage[inarea][age][len].W = Alkeys[age][len].W;

      if (len >= minStrayLength)
        Alkeys[age][len].N -= straynumber;
    }
  }
}

void StrayData::storeStrayingStock(int area, AgeBandMatrix& Alkeys,
  AgeBandMatrixRatio& TagAlkeys, const TimeClass* const TimeInfo) {

  if (!istagged)
    handle.logMessage(LOGFAIL, "Error in stray - invalid tagging experiment");

  int age, len, tag;
  int inarea = this->areaNum(area);
  double straynumber;

  for (age = Storage[inarea].minAge(); age < Storage[inarea].maxAge(); age++) {
    for (len = Storage[inarea].minLength(age); len < Storage[inarea].maxLength(age); len++) {
      straynumber = Alkeys[age][len].N * strayProportion[len];
      Storage[inarea][age][len].N = straynumber;
      Storage[inarea][age][len].W = Alkeys[age][len].W;

      if (len >= minStrayLength)
        Alkeys[age][len].N -= straynumber;

      for (tag = 0; tag < TagAlkeys.numTagExperiments(); tag++) {
        straynumber = *(TagAlkeys[age][len][tag].N) * strayProportion[len];
        if (straynumber < verysmall)
          *(tagStorage[inarea][age][len][tag].N) = 0.0;
        else
          *(tagStorage[inarea][age][len][tag].N) = straynumber;

        if (len >= minStrayLength)
          *(TagAlkeys[age][len][tag].N) -= straynumber;
      }
    }
  }
}

void StrayData::addStrayStock(int area, const TimeClass* const TimeInfo) {

  int i, inarea = this->areaNum(area);
  double ratio;
  for (i = 0; i < strayStocks.Size(); i++) {
    if (!strayStocks[i]->isInArea(area))
      handle.logMessage(LOGFAIL, "Error in straying - stray stock doesnt live on area", area);

    if (strayStocks[i]->isBirthday(TimeInfo)) {
      Storage[inarea].IncrementAge();
      if (istagged && tagStorage.numTagExperiments() > 0)
        tagStorage[inarea].IncrementAge(Storage[inarea]);
    }

    ratio = strayRatio[ratioindex[i]] * ratioscale;
    strayStocks[i]->Add(Storage[inarea], CI[i], area, ratio);
    if (istagged && tagStorage.numTagExperiments() > 0)
      strayStocks[i]->Add(tagStorage, CI[i], area, ratio);
  }
  Storage[inarea].setToZero();
  if (istagged && tagStorage.numTagExperiments() > 0)
    tagStorage[inarea].setToZero();
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

  //JMB check that the sum of the ratios is 1
  if (TimeInfo->getTime() == 1) {
    ratioscale = 0.0;
    for (i = 0; i < strayRatio.Size(); i++ )
      ratioscale += strayRatio[i];

    if (isZero(ratioscale)) {
      handle.logMessage(LOGWARN, "Warning in straying - specified ratios are zero");
      ratioscale = 1.0;
    } else if (isEqual(ratioscale, 1.0)) {
      // do nothing
    } else {
      handle.logMessage(LOGWARN, "Warning in straying - scaling ratios using", ratioscale);
      ratioscale = 1.0 / ratioscale;
    }
  }

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

    if (handle.getLogLevel() >= LOGMESSAGE)
      handle.logMessage(LOGMESSAGE, "Reset straying data for stock", this->getName());
  }
}

void StrayData::Print(ofstream& outfile) const {
  int i;
  outfile << "\nStraying information\n\tNames of straying stocks:";
  for (i = 0; i < strayStockNames.Size(); i++)
    outfile << sep << strayStockNames[i];
  outfile << "\n\tRatio moving into each stock:";
  for (i = 0; i < strayRatio.Size(); i++)
    outfile << sep << (strayRatio[ratioindex[i]] * ratioscale);
  outfile << "\n\tStraying timesteps:";
  for (i = 0; i < strayStep.Size(); i++)
    outfile << sep << strayStep[i];
  outfile << endl;
}

const StockPtrVector& StrayData::getStrayStocks() {
  return strayStocks;
}

void StrayData::setTagged() {
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

void StrayData::addStrayTag(const char* tagname) {
  if (!istagged)
    handle.logMessage(LOGFAIL, "Error in stray - invalid tagging experiment", tagname);
  tagStorage.addTag(tagname);
}

void StrayData::deleteStrayTag(const char* tagname) {
  if (!istagged)
    handle.logMessage(LOGFAIL, "Error in stray - invalid tagging experiment", tagname);

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
    handle.logMessage(LOGWARN, "Warning in stray - failed to delete tagging experiment", tagname);
}
