#include "prey.h"
#include "predator.h"
#include "suits.h"
#include "mathfunc.h"
#include "gadget.h"

Suits::Suits() {
}

Suits::~Suits() {
  int i;
  for (i = 0; i < this->numPreys(); i++) {
    delete[] preynames[i];
    delete suitFunction[i];
  }
}

void Suits::addPrey(const char* preyname, SuitFunc* suitf) {
  preCalcSuitability.resize(1);
  suitFunction.resize(1, suitf);
  preynames.resize(1);
  preynames[preynames.Size() - 1] = new char[strlen(preyname) + 1];
  strcpy(preynames[preynames.Size() - 1], preyname);
}

int Suits::didChange(int i, const TimeClass* const TimeInfo) const {
  return suitFunction[i]->didChange(TimeInfo);
}

void Suits::deletePrey(int i, Keeper* const keeper) {
  if (this->numPreys() == 0)
    return;
  suitFunction.Delete(i, keeper);
  preynames.Delete(i);
  preCalcSuitability.Delete(i);
}

void Suits::Reset(const Predator* const pred, const TimeClass* const TimeInfo) {
  int i, j, p;
  DoubleMatrix* temp = 0;
  BandMatrix* suit = 0;

  for (p = 0; p < this->numPreys(); p++) {
    suitFunction[p]->updateConstants(TimeInfo);
    if (suitFunction[p]->didChange(TimeInfo)) {
      temp = new DoubleMatrix(pred->numLengthGroups(), pred->Preys(p)->numLengthGroups(), 0.0);
      for (i = 0; i < pred->numLengthGroups(); i++) {
        for (j = 0; j < pred->Preys(p)->numLengthGroups(); j++) {
          if (suitFunction[p]->usesPreyLength())
            suitFunction[p]->setPreyLength(pred->Preys(p)->meanLength(j));

          if (suitFunction[p]->usesPredLength())
            suitFunction[p]->setPredLength(pred->meanLength(i));

          (*temp)[i][j] = suitFunction[p]->calculate();
          if ((*temp)[i][j] < 0)
            (*temp)[i][j] = 0.0;
        }
      }
      suit = new BandMatrix(*temp);
      preCalcSuitability.changeElement(p, *suit);
      delete suit;
      delete temp;
    }
  }

  #ifdef SUIT_SCALE
  //Scaling of suitabilities, so that in each lengthgroup of each predator, the
  //maximum suitability is exactly 1, if any suitability is different from 0.
  double mult;
  for (i = 0; i < pred->numLengthGroups(); i++) {
    mult = 0.0;
    for (p = 0; p < this->numPreys(); p++)
      if (i >= preCalcSuitability[p].minRow() && i <= preCalcSuitability[p].maxRow())
        for (j = preCalcSuitability[p].minCol(i); j < preCalcSuitability[p].maxCol(i); j++)
          mult = max(preCalcSuitability[p][i][j], mult);

    if (!(isZero(mult)))
      for (p = 0; p < this->numPreys(); p++)
        if (i >= preCalcSuitability[p].minRow() && i <= preCalcSuitability[p].maxRow())
          for (j = preCalcSuitability[p].minCol(i); j < preCalcSuitability[p].maxCol(i); j++)
            preCalcSuitability[p][i][j] /= mult;
  }
  #endif
}
