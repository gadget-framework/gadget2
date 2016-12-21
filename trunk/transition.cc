#include "transition.h"
#include "errorhandler.h"
#include "keeper.h"
#include "readword.h"
#include "gadget.h"
#include "global.h"

Transition::Transition(CommentStream& infile, const IntVector& areas, int Age,
  const LengthGroupDivision* const lgrpdiv, const char* givenname,
  const TimeClass* const TimeInfo, Keeper* const keeper) 
  : HasName(givenname), LivesOnAreas(areas), age(Age) {

  int i = 0;
  istagged = 0;
  ratioscale = 1.0; //JMB used to scale the ratios to ensure that they sum to 1
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  keeper->addString("transition");

  LgrpDiv = new LengthGroupDivision(*lgrpdiv);
  if (LgrpDiv->Error())
    handle.logMessage(LOGFAIL, "Error in transition - failed to create length group");

  infile >> text >> ws;
  if (strcasecmp(text, "transitionstocksandratios") != 0)
    handle.logFileUnexpected(LOGFAIL, "transitionstocksandratios", text);

  infile >> text >> ws;
  while (strcasecmp(text, "transitionstep") != 0 && !infile.eof()) {
    transitionStockNames.resize(new char[strlen(text) + 1]);
    strcpy(transitionStockNames[i], text);
    transitionRatio.resize(1, keeper);
    if (!(infile >> transitionRatio[i]))
      handle.logFileMessage(LOGFAIL, "invalid format for transition ratio");
    transitionRatio[i].Inform(keeper);

    infile >> text >> ws;
    i++;
  }

  if (infile.eof())
    handle.logFileEOFMessage(LOGFAIL);
  infile >> transitionStep >> ws;
  if (transitionStep < 1 || transitionStep > TimeInfo->numSteps())
    handle.logFileMessage(LOGFAIL, "invalid transition step", transitionStep);
  keeper->clearLast();
}

Transition::~Transition() {
  int i;
  for (i = 0; i < transitionStockNames.Size(); i++)
    delete[] transitionStockNames[i];
  for (i = 0; i < CI.Size(); i++)
    delete CI[i];
  delete LgrpDiv;
}

void Transition::setStock(StockPtrVector& stockvec) {
  int i, j, index;

  for (i = 0; i < transitionStockNames.Size(); i++)
    for (j = 0; j < transitionStockNames.Size(); j++)
      if ((strcasecmp(transitionStockNames[i], transitionStockNames[j]) == 0) && (i != j))
        handle.logMessage(LOGFAIL, "Error in transition - repeated stock", transitionStockNames[i]);

  for (i = 0; i < stockvec.Size(); i++)
    for (j = 0; j < transitionStockNames.Size(); j++)
      if (strcasecmp(stockvec[i]->getName(), transitionStockNames[j]) == 0)
        transitionStocks.resize(stockvec[i]);

  if (transitionStocks.Size() != transitionStockNames.Size()) {
    handle.logMessage(LOGWARN, "Error in transition - failed to match transition stocks");
    for (i = 0; i < stockvec.Size(); i++)
      handle.logMessage(LOGWARN, "Error in transition - found stock", stockvec[i]->getName());
    for (i = 0; i < transitionStockNames.Size(); i++)
      handle.logMessage(LOGWARN, "Error in transition - looking for stock", transitionStockNames[i]);
    handle.logMessage(LOGFAIL, ""); //JMB this will exit gadget
  }

  //JMB ensure that the ratio vector is indexed in the right order
  ratioindex.resize(transitionStocks.Size(), 0);
  for (i = 0; i < transitionStocks.Size(); i++)
    for (j = 0; j < transitionStockNames.Size(); j++)
      if (strcasecmp(transitionStocks[i]->getName(), transitionStockNames[j]) == 0)
        ratioindex[i] = j;

  double mlength = 9999.0;
  for (i = 0; i < transitionStocks.Size(); i++) {
    CI.resize(new ConversionIndex(LgrpDiv, transitionStocks[i]->getLengthGroupDiv()));
    if (CI[i]->Error())
      handle.logMessage(LOGFAIL, "Error in transition - error when checking length structure");
    index = 0;
    for (j = 0; j < areas.Size(); j++)
      if (!transitionStocks[i]->isInArea(areas[j]))
        index++;

    if (index != 0)
      handle.logMessage(LOGWARN, "Warning in transition - transition stock isnt defined on all areas");

    if (transitionStocks[i]->getLengthGroupDiv()->minLength() < mlength)
      mlength = transitionStocks[i]->getLengthGroupDiv()->minLength();
  }

  minTransitionLength = LgrpDiv->numLengthGroup(mlength);
  IntVector minlv(2, 0);
  IntVector sizev(2, LgrpDiv->numLengthGroups());
  Storage.resize(areas.Size(), age, minlv, sizev);
  for (i = 0; i < Storage.Size(); i++)
    Storage[i].setToZero();
}

void Transition::Print(ofstream& outfile) const {
  int i;
  outfile << "\nTransition\n\tNames of transition stocks:";
  for (i = 0; i < transitionStockNames.Size(); i++)
    outfile << sep << transitionStockNames[i];
  outfile << "\n\tRatio moving into each stock:";
  for (i = 0; i < transitionRatio.Size(); i++)
    outfile << sep << (transitionRatio[ratioindex[i]] * ratioscale);
  outfile << "\n\tTransition step " << transitionStep << endl;
}

void Transition::storeTransitionStock(int area, AgeBandMatrix& Alkeys, const TimeClass* const TimeInfo) {

  int len, inarea = this->areaNum(area);
  for (len = Storage[inarea].minLength(age); len < Storage[inarea].maxLength(age); len++) {
    Storage[inarea][age][len].N = Alkeys[age][len].N;
    Storage[inarea][age][len].W = Alkeys[age][len].W;

    if (len >= minTransitionLength)
      Alkeys[age][len].setToZero();
  }
}

void Transition::storeTransitionStock(int area, AgeBandMatrix& Alkeys,
  AgeBandMatrixRatio& TagAlkeys, const TimeClass* const TimeInfo) {

  if (!istagged)
    handle.logMessage(LOGFAIL, "Error in transition - invalid tagging experiment");

  int len, tag;
  int inarea = this->areaNum(area);
  double tagnumber;

  for (len = Storage[inarea].minLength(age); len < Storage[inarea].maxLength(age); len++) {
    Storage[inarea][age][len].N = Alkeys[age][len].N;
    Storage[inarea][age][len].W = Alkeys[age][len].W;

    if (len >= minTransitionLength)
      Alkeys[age][len].setToZero();

    for (tag = 0; tag < TagAlkeys.numTagExperiments(); tag++) {
      tagnumber = *(TagAlkeys[age][len][tag].N);
      if (tagnumber < verysmall)
        *(tagStorage[inarea][age][len][tag].N) = 0.0;
      else
        *(tagStorage[inarea][age][len][tag].N) = tagnumber;

      if (len >= minTransitionLength) {
        *(TagAlkeys[age][len][tag].N) = 0.0;
        TagAlkeys[age][len][tag].R = 0.0;
      }
    }
  }
}

//area in the call to this routine is not in the local area numbering of the stock.
void Transition::Move(int area, const TimeClass* const TimeInfo) {

  int i, inarea = this->areaNum(area);
  double ratio;
  for (i = 0; i < transitionStocks.Size(); i++) {
    if (!transitionStocks[i]->isInArea(area))
      handle.logMessage(LOGFAIL, "Error in transition - transition stock doesnt live on area", area);

    if (transitionStocks[i]->isBirthday(TimeInfo)) {
      Storage[inarea].IncrementAge();
      if (istagged && tagStorage.numTagExperiments() > 0)
        tagStorage[inarea].IncrementAge(Storage[inarea]);
    }

    ratio = transitionRatio[ratioindex[i]] * ratioscale;
    transitionStocks[i]->Add(Storage[inarea], CI[i], area, ratio);
    if (istagged && tagStorage.numTagExperiments() > 0)
      transitionStocks[i]->Add(tagStorage, CI[i], area, ratio);
  }

  Storage[inarea].setToZero();
  if (istagged && tagStorage.numTagExperiments() > 0)
    tagStorage[inarea].setToZero();
}

void Transition::Reset() {
  int i;

  //JMB check that the sum of the ratios is 1
  ratioscale = 0.0;
  for (i = 0; i < transitionRatio.Size(); i++ )
    ratioscale += transitionRatio[i];

  if (isZero(ratioscale)) {
    handle.logMessage(LOGWARN, "Warning in transition - specified ratios are zero");
    ratioscale = 1.0;
  } else if (isEqual(ratioscale, 1.0)) {
    // do nothing
  } else {
    handle.logMessage(LOGWARN, "Warning in transition - scaling ratios using", ratioscale);
    ratioscale = 1.0 / ratioscale;
  }

  if (handle.getLogLevel() >= LOGMESSAGE)
    handle.logMessage(LOGMESSAGE, "Reset transition data for stock", this->getName());
}

const StockPtrVector& Transition::getTransitionStocks() {
  return transitionStocks;
}

void Transition::setTagged() {
  istagged = 1;
  //resize tagStorage to be the same size as Storage
  int i;
  IntVector lower(2, 0);
  IntVector size(2, LgrpDiv->numLengthGroups());
  tagStorage.resize(areas.Size(), age, lower, size);
  for (i = 0; i < tagStorage.Size(); i++)
    tagStorage[i].setToZero();
}

void Transition::addTransitionTag(const char* tagname) {
  if (!istagged)
    handle.logMessage(LOGFAIL, "Error in transition - invalid tagging experiment", tagname);
  tagStorage.addTag(tagname);
}

void Transition::deleteTransitionTag(const char* tagname) {
  if (!istagged)
    handle.logMessage(LOGFAIL, "Error in transition - invalid tagging experiment", tagname);

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
    handle.logMessage(LOGWARN, "Warning in transition - failed to delete tagging experiment", tagname);
}

int Transition::isTransitionStep(const TimeClass* const TimeInfo) {
  if (TimeInfo->getStep() == transitionStep)
    return 1;
  return 0;
}
