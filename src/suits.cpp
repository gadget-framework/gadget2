#include "prey.h"
#include "predator.h"
#include "suits.h"
#include "mathfunc.h"
#include "gadget.h"

Suits::~Suits() {
  int i;
  for (i = 0; i < preynames.Size(); i++) {
    delete[] preynames[i];
    delete suitFunction[i];
    delete preCalcSuitability[i];
  }
}

void Suits::addPrey(const char* preyname, SuitFunc* suitf) {
  preCalcSuitability.resizeBlank(1);
  suitFunction.resize(suitf);
  preynames.resize(new char[strlen(preyname) + 1]);
  strcpy(preynames[preynames.Size() - 1], preyname);
}

void Suits::deletePrey(int i, Keeper* const keeper) {
  if (preynames.Size() == 0)
    return;
  suitFunction.Delete(i, keeper);
  preynames.Delete(i);
  preCalcSuitability.Delete(i);
}

void Suits::Initialise(const Predator* const pred) {
  int p;
  for (p = 0; p < preynames.Size(); p++)
    preCalcSuitability[p] = new DoubleMatrix(pred->getLengthGroupDiv()->numLengthGroups(), pred->getPrey(p)->getLengthGroupDiv()->numLengthGroups(), 0.0);
}

void Suits::Reset(const Predator* const pred, const TimeClass* const TimeInfo) {
  int i, j, p;

  for (p = 0; p < preynames.Size(); p++) {
    suitFunction[p]->updateConstants(TimeInfo);
    if (suitFunction[p]->didChange(TimeInfo)) {
      for (i = 0; i < preCalcSuitability[p]->Nrow(); i++) {
        for (j = 0; j < preCalcSuitability[p]->Ncol(i); j++) {
          if (suitFunction[p]->usesPreyLength())
            suitFunction[p]->setPreyLength(pred->getPrey(p)->getLengthGroupDiv()->meanLength(j));

          if (suitFunction[p]->usesPredLength())
            suitFunction[p]->setPredLength(pred->getLengthGroupDiv()->meanLength(i));

          (*preCalcSuitability[p])[i][j] = suitFunction[p]->calculate();
        }
      }
    }
  }

  #ifdef SUIT_SCALE
  //Scaling of suitabilities, so that in each lengthgroup of each predator, the
  //maximum suitability is exactly 1, if any suitability is different from 0.
  double mult;
  for (i = 0; i < pred->getLengthGroupDiv()->numLengthGroups(); i++) {
    mult = 0.0;
    for (p = 0; p < preynames.Size(); p++)
      for (j = 0; j < preCalcSuitability[p]->Ncol(i); j++)
        mult = max((*preCalcSuitability[p])[i][j], mult);

    if (!(isZero(mult)))
      for (p = 0; p < preynames.Size(); p++)
        for (j = 0; j < preCalcSuitability[p]->Ncol(i); j++)
          (*preCalcSuitability[p])[i][j] /= mult;
  }
  #endif
}
