#include "transition.h"
#include "errorhandler.h"
#include "keeper.h"
#include "readword.h"
#include "gadget.h"

extern ErrorHandler handle;

Transition::Transition(CommentStream& infile, const IntVector& areas, int Age,
  const LengthGroupDivision* const lgrpdiv, Keeper* const keeper)
  : LivesOnAreas(areas), LgrpDiv(new LengthGroupDivision(*lgrpdiv)), age(Age) {

  int i;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  keeper->addString("transition");

  infile >> text;
  if (strcasecmp(text, "transitionstocksandratios") == 0) {
    i = 0;
    infile >> text >> ws;
    while (strcasecmp(text, "transitionstep") != 0 && !infile.eof()) {
      transitionStockNames.resize(1);
      transitionStockNames[i] = new char[strlen(text) + 1];
      strcpy(transitionStockNames[i], text);
      transitionRatio.resize(1);
      infile >> transitionRatio[i] >> text >> ws;
      i++;
    }
  } else
    handle.logFileUnexpected(LOGFAIL, "transitionstocksandratios", text);

  if (infile.eof())
    handle.logFileEOFMessage(LOGFAIL);
  infile >> transitionStep >> ws;
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
  int index = 0;
  int i, j, numstocks;
  DoubleVector tmpratio;

  for (i = 0; i < stockvec.Size(); i++)
    for (j = 0; j < transitionStockNames.Size(); j++)
      if (strcasecmp(stockvec[i]->getName(), transitionStockNames[j]) == 0) {
        transitionStocks.resize(1);
        tmpratio.resize(1);
        transitionStocks[index] = stockvec[i];
        tmpratio[index] = transitionRatio[j];
        index++;
      }

  if (index != transitionStockNames.Size()) {
    handle.logMessage(LOGWARN, "Error in transition - failed to match transition stocks");
    for (i = 0; i < stockvec.Size(); i++)
      handle.logMessage(LOGWARN, "Error in transition - found stock", stockvec[i]->getName());
    for (i = 0; i < transitionStockNames.Size(); i++)
      handle.logMessage(LOGWARN, "Error in transition - looking for stock", transitionStockNames[i]);
    exit(EXIT_FAILURE);
  }

  for (i = transitionRatio.Size(); i > 0; i--)
    transitionRatio.Delete(0);
  transitionRatio.resize(tmpratio.Size());
  for (i = 0; i < tmpratio.Size(); i++)
    transitionRatio[i] = tmpratio[i];

  numstocks = transitionStocks.Size();
  CI.resize(numstocks, 0);
  for (i = 0; i < numstocks; i++)
    CI[i] = new ConversionIndex(LgrpDiv, transitionStocks[i]->getLengthGroupDiv());

  //JMB - check that the transition stocks are defined on the areas
  double mlength = 9999.0;
  for (i = 0; i < numstocks; i++) {
    index = 0;
    for (j = 0; j < areas.Size(); j++)
      if (!transitionStocks[i]->isInArea(areas[j]))
        index++;

    if ((handle.getLogLevel() >= LOGWARN) && (index != 0))
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
    outfile << sep << transitionRatio[i];
  outfile << "\n\tTransition step " << transitionStep << endl;
}

void Transition::keepAgeGroup(int area, AgeBandMatrix& Alkeys,
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

    if (l >= minTransitionLength) {
      Alkeys[age][l].N = 0.0;
      Alkeys[age][l].W = 0.0;
    }

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

  int s, inarea = this->areaNum(area);
  for (s = 0; s < transitionStocks.Size(); s++) {
    if (!transitionStocks[s]->isInArea(area))
      handle.logMessage(LOGFAIL, "Error in transition - transition stock doesnt live on area", area);

    if (transitionStocks[s]->isBirthday(TimeInfo)) {
      Storage[inarea].IncrementAge();
      if (tagStorage.numTagExperiments() > 0)
        tagStorage[inarea].IncrementAge(Storage[inarea]);
    }

    transitionStocks[s]->Add(Storage[inarea], CI[s], area, transitionRatio[s],
      Storage[inarea].minAge(), Storage[inarea].maxAge());

    if (tagStorage.numTagExperiments() > 0)
      transitionStocks[s]->Add(tagStorage, inarea, CI[s], area, transitionRatio[s],
        tagStorage[inarea].minAge(), tagStorage[inarea].maxAge());
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

int Transition::isTransitionStep(int area, const TimeClass* const TimeInfo) {
  if (TimeInfo->getStep() == transitionStep)
    return 1;
  return 0;
}
