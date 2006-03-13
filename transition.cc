#include "transition.h"
#include "errorhandler.h"
#include "keeper.h"
#include "readword.h"
#include "gadget.h"

extern ErrorHandler handle;

Transition::Transition(CommentStream& infile, const IntVector& areas, int Age,
  const LengthGroupDivision* const lgrpdiv, const TimeClass* const TimeInfo, Keeper* const keeper)
  : LivesOnAreas(areas), LgrpDiv(new LengthGroupDivision(*lgrpdiv)), age(Age) {

  int i = 0;
  ratioscale = 1.0; //JMB used to scale the ratios to ensure that they sum to 1
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  keeper->addString("transition");

  infile >> text >> ws;
  if (strcasecmp(text, "transitionstocksandratios") != 0)
    handle.logFileUnexpected(LOGFAIL, "transitionstocksandratios", text);

  infile >> text >> ws;
  while (strcasecmp(text, "transitionstep") != 0 && !infile.eof()) {
    transitionStockNames.resize(new char[strlen(text) + 1]);
    strcpy(transitionStockNames[i], text);
    transitionRatio.resize(1, keeper);
    ratioindex.resize(1, 0);
    if (!(infile >> transitionRatio[i]))
      handle.logFileMessage(LOGFAIL, "invalid format for transition ratio");

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
    exit(EXIT_FAILURE);
  }

  //JMB ensure that the ratio vector is indexed in the right order
  for (i = 0; i < transitionStocks.Size(); i++)
    for (j = 0; j < transitionStockNames.Size(); j++)
      if (strcasecmp(transitionStocks[i]->getName(), transitionStockNames[j]) == 0)
        ratioindex[i] = j;

  double mlength = 9999.0;
  for (i = 0; i < transitionStocks.Size(); i++) {
    CI.resize(new ConversionIndex(LgrpDiv, transitionStocks[i]->getLengthGroupDiv()));
    index = 0;
    for (j = 0; j < areas.Size(); j++)
      if (!transitionStocks[i]->isInArea(areas[j]))
        index++;

    if (index != 0)
      handle.logMessage(LOGWARN, "Warning in transition - transition stock isnt defined on all areas");

    if (transitionStocks[i]->getLengthGroupDiv()->minLength() < mlength)
      mlength = transitionStocks[i]->getLengthGroupDiv()->minLength();
  }

  IntVector minlv(2, 0);
  IntVector sizev(2, LgrpDiv->numLengthGroups());
  Storage.resize(areas.Size(), age, minlv, sizev);
  tagStorage.resize(areas.Size(), age, minlv, sizev);
  minTransitionLength = LgrpDiv->numLengthGroup(mlength);
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

void Transition::storeTransitionStock(int area, AgeBandMatrix& Alkeys,
  AgeBandMatrixRatio& TagAlkeys, const TimeClass* const TimeInfo) {

  int inarea = this->areaNum(area);
  int numtags = TagAlkeys.numTagExperiments();
  int i, l, minl, maxl;
  double tagnumber;

  minl = Storage[inarea].minLength(age);
  maxl = Storage[inarea].maxLength(age);
  for (l = minl; l < maxl; l++) {
    Storage[inarea][age][l].N = Alkeys[age][l].N;
    Storage[inarea][age][l].W = Alkeys[age][l].W;

    if (l >= minTransitionLength)
      Alkeys[age][l].setToZero();

    for (i = 0; i < numtags; i++) {
      tagnumber = *(TagAlkeys[age][l][i].N);
      if (tagnumber < verysmall)
        *(tagStorage[inarea][age][l][i].N) = 0.0;
      else
        *(tagStorage[inarea][age][l][i].N) = tagnumber;

      if (l >= minTransitionLength) {
        *(TagAlkeys[age][l][i].N) = 0.0;
        TagAlkeys[age][l][i].R = 0.0;
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
      if (tagStorage.numTagExperiments() > 0)
        tagStorage[inarea].IncrementAge(Storage[inarea]);
    }

    ratio = transitionRatio[ratioindex[i]] * ratioscale;
    transitionStocks[i]->Add(Storage[inarea], CI[i], area, ratio);
    if (tagStorage.numTagExperiments() > 0)
      transitionStocks[i]->Add(tagStorage, CI[i], area, ratio);
  }

  Storage[inarea].setToZero();
  if (tagStorage.numTagExperiments() > 0)
    tagStorage[inarea].setToZero();
}

void Transition::Reset() {
  int i;
  for (i = 0; i < Storage.Size(); i++) {
    Storage[i].setToZero();
    tagStorage[i].setToZero();
  }

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
    handle.logMessage(LOGMESSAGE, "Reset transition data");
}

const StockPtrVector& Transition::getTransitionStocks() {
  return transitionStocks;
}

void Transition::addTransitionTag(const char* tagname) {
  tagStorage.addTag(tagname);
}

void Transition::deleteTransitionTag(const char* tagname) {
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
    handle.logMessage(LOGWARN, "Warning in transition - failed to delete tagging experiment", tagname);
}

int Transition::isTransitionStep(const TimeClass* const TimeInfo) {
  if (TimeInfo->getStep() == transitionStep)
    return 1;
  return 0;
}
