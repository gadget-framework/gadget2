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
    infile >> text;
    i = 0;
    while (strcasecmp(text, "transitionstep") != 0 && infile.good()) {
      transitionStockNames.resize(1);
      transitionStockNames[i] = new char[strlen(text) + 1];
      strcpy(transitionStockNames[i], text);
      Ratio.resize(1);
      infile >> Ratio[i];
      i++;
      infile >> text;
    }
  } else
    handle.Unexpected("transitionstocksandratios", text);

  if (!infile.good())
    handle.Failure();

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
      if (strcasecmp(stockvec[i]->Name(), transitionStockNames[j]) == 0) {
        transitionStocks.resize(1);
        tmpratio.resize(1);
        transitionStocks[index] = stockvec[i];
        tmpratio[index] = Ratio[j];
        index++;
      }

  if (index != transitionStockNames.Size()) {
    handle.logWarning("Error in transition - failed to match transition stocks");
    for (i = 0; i < stockvec.Size(); i++)
      handle.logWarning("Error in transition - found stock", stockvec[i]->Name());
    for (i = 0; i < transitionStockNames.Size(); i++)
      handle.logWarning("Error in transition - looking for stock", transitionStockNames[i]);
    exit(EXIT_FAILURE);
  }

  for (i = Ratio.Size(); i > 0; i--)
    Ratio.Delete(0);
  Ratio.resize(tmpratio.Size());
  for (i = 0; i < tmpratio.Size(); i++)
    Ratio[i] = tmpratio[i];

  numstocks = transitionStocks.Size();
  CI.resize(numstocks, 0);
  for (i = 0; i < numstocks; i++)
    CI[i] = new ConversionIndex(LgrpDiv, transitionStocks[i]->returnLengthGroupDiv());

  //JMB - check that the transition stocks are defined on the areas
  double mlength = 9999.0;
  for (i = 0; i < numstocks; i++) {
    index = 0;
    for (j = 0; j < areas.Size(); j++)
      if (!transitionStocks[i]->IsInArea(areas[j]))
        index++;

    if (index != 0)
      handle.logWarning("Warning in transition - transition stock isnt defined on all areas");

    if (transitionStocks[i]->returnLengthGroupDiv()->minLength() < mlength)
      mlength = transitionStocks[i]->returnLengthGroupDiv()->minLength();
  }

  IntVector minlv(2, 0);
  IntVector sizev(2, LgrpDiv->NoLengthGroups());
  AgeGroup.resize(areas.Size(), age, minlv, sizev);
  TagAgeGroup.resize(areas.Size(), age, minlv, sizev);
  minTransitionLength = LgrpDiv->NoLengthGroup(mlength);
}

void Transition::Print(ofstream& outfile) const {
  int i;
  outfile << "\nTransition\n\tNames of transition stocks:";
  for (i = 0; i < transitionStocks.Size(); i++)
    outfile << sep << (const char*)(transitionStocks[i]->Name());
  outfile << "\n\tRatio moving into each stock:";
  for (i = 0; i < Ratio.Size(); i++)
    outfile << sep << Ratio[i];
  outfile << "\n\tTransition step " << transitionStep << endl;
}

void Transition::keepAgeGroup(int area, AgeBandMatrix& Alkeys,
  AgeBandMatrixRatio& TagAlkeys, const TimeClass* const TimeInfo) {

  if (!(this->IsTransitionStep(area, TimeInfo)))
    handle.logFailure("Error in transition - transition requested on wrong timestep");

  int inarea = AreaNr[area];
  int numtags = TagAlkeys.numTagExperiments();
  int i, l, minl, maxl;
  double tagnumber;

  minl = AgeGroup[inarea].minLength(age);
  maxl = AgeGroup[inarea].maxLength(age);
  for (l = minl; l < maxl; l++) {
    AgeGroup[inarea][age][l].N = Alkeys[age][l].N;
    AgeGroup[inarea][age][l].W = Alkeys[age][l].W;

    if (l >= minTransitionLength) {
      Alkeys[age][l].N = 0.0;
      Alkeys[age][l].W = 0.0;
    }

    for (i = 0; i < numtags; i++) {
      tagnumber = *(TagAlkeys[age][l][i].N);
      if (tagnumber < verysmall)
        *(TagAgeGroup[inarea][age][l][i].N) = 0.0;
      else
        *(TagAgeGroup[inarea][age][l][i].N) = tagnumber;

      if (l >= minTransitionLength) {
        *(TagAlkeys[age][l][i].N) = 0.0;
        TagAlkeys[age][l][i].R = 0.0;
      }
    }
  }
}

//area in the call to this routine is not in the local area numbering of the stock.
void Transition::Move(int area, const TimeClass* const TimeInfo) {

  if (!(this->IsTransitionStep(area, TimeInfo)))
    handle.logFailure("Error in transition - transition requested on wrong timestep");

  int s, inarea = AreaNr[area];
  for (s = 0; s < transitionStocks.Size(); s++) {
    if (!transitionStocks[s]->IsInArea(area))
      handle.logFailure("Error in transition - transition stock doesnt live on area", area);

    if (transitionStocks[s]->Birthday(TimeInfo)) {
      AgeGroup[inarea].IncrementAge();
      if (TagAgeGroup.numTagExperiments() > 0)
        TagAgeGroup[inarea].IncrementAge(AgeGroup[inarea]);
    }

    transitionStocks[s]->Add(AgeGroup[inarea], CI[s], area, Ratio[s],
      AgeGroup[inarea].minAge(), AgeGroup[inarea].maxAge());

    if (TagAgeGroup.numTagExperiments() > 0)
      transitionStocks[s]->Add(TagAgeGroup, inarea, CI[s], area, Ratio[s],
        TagAgeGroup[inarea].minAge(), TagAgeGroup[inarea].maxAge());
  }

  AgeGroup[inarea].setToZero();
  TagAgeGroup[inarea].setToZero();
}

void Transition::Reset() {
  int i;
  for (i = 0; i < AgeGroup.Size(); i++) {
    AgeGroup[i].setToZero();
    TagAgeGroup[i].setToZero();
  }
  handle.logMessage("Reset transition data");
}

const StockPtrVector& Transition::getTransitionStocks() {
  return transitionStocks;
}

void Transition::addTransitionTag(const char* tagname) {
  TagAgeGroup.addTag(tagname);
}

void Transition::deleteTransitionTag(const char* tagname) {
  int minage, maxage, minlen, maxlen, a, length, i;
  int id = TagAgeGroup.getID(tagname);

  if (id >= 0) {
    minage = TagAgeGroup[0].minAge();
    maxage = TagAgeGroup[0].maxAge();

    // Remove allocated memory
    for (i = 0; i < TagAgeGroup.Size(); i++) {
      for (a = minage; a <= maxage; a++) {
        minlen = TagAgeGroup[i].minLength(a);
        maxlen = TagAgeGroup[i].maxLength(a);
        for (length = minlen; length < maxlen; length++) {
          delete[] (TagAgeGroup[i][a][length][id].N);
          (TagAgeGroup[i][a][length][id].N) = NULL;
        }
      }
    }
    TagAgeGroup.deleteTag(tagname);

  } else
    handle.logWarning("Warning in transition - failed to delete tagging experiment", tagname);
}

int Transition::IsTransitionStep(int area, const TimeClass* const TimeInfo) {
  if (TimeInfo->CurrentStep() == transitionStep)
    return 1;
  return 0;
}
