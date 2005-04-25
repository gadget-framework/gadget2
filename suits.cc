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
  int i = this->numPreys();
  preynames.resize(1);
  suitFunction.resize(1, suitf);
  preynames[i] = new char[strlen(preyname) + 1];
  strcpy(preynames[i], preyname);
}

int Suits::DidChange(int prey, const TimeClass* const TimeInfo) const {
  return suitFunction[prey]->constantsHaveChanged(TimeInfo);
}

void Suits::deletePrey(int prey, Keeper* const keeper) {
  if (this->numPreys() == 0)
    return;
  suitFunction.Delete(prey, keeper);
  preynames.Delete(prey);
  if (preCalcSuitability.Size() != 0)
    preCalcSuitability.Delete(prey);
}

//Calculate suitabilities and set in the BandMatrixVector preCalcSuitability.
void Suits::Reset(const Predator* const pred, const TimeClass* const TimeInfo) {
  //First time.
  if (preCalcSuitability.Size() == 0)
    preCalcSuitability.resize(this->numPreys());

  /* remember that Suits is a friend of Predator.
   * Therefore we access pred through protected functions, but we only
   * use const access functions to access the length group division of
   * pred and its preys -- we do not change anything in pred. */

  DoubleMatrix* temp = 0;
  BandMatrix* suit = 0;
  int i, j, p;

  for (p = 0; p < this->numPreys(); p++) {
    suitFunction[p]->updateConstants(TimeInfo);
    if (suitFunction[p]->constantsHaveChanged(TimeInfo)) {
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
