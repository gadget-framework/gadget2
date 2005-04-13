#include "prey.h"
#include "predator.h"
#include "suits.h"
#include "keeper.h"
#include "mathfunc.h"
#include "gadget.h"

Suits::Suits() {
}

Suits::~Suits() {
  int i;
  for (i = 0; i < preynames.Size(); i++)
    delete[] preynames[i];
}

void Suits::addPrey(const char* preyname, SuitFunc* suitf) {
  int i = preynames.Size();
  preynames.resize(1);
  SuitFunction.resize(1, suitf);
  preynames[i] = new char[strlen(preyname) + 1];
  strcpy(preynames[i], preyname);
}

const char* Suits::Preyname(int prey) const {
  return preynames[prey];
}

int Suits::numPreys() const {
  return SuitFunction.Size();
}

const BandMatrix& Suits::Suitable(int prey) const {
  return preCalcSuitability[prey];
}

void Suits::DeletePrey(int prey, Keeper* const keeper) {
  if (SuitFunction.Size() == 0)
    return;
  SuitFunction.Delete(prey, keeper);
  preynames.Delete(prey);
  if (preCalcSuitability.Size() != 0)
    preCalcSuitability.Delete(prey);
}

//Calculate suitabilities and set in the BandMatrixVector preCalcSuitability.
void Suits::Reset(const Predator* const pred, const TimeClass* const TimeInfo) {
  //First time.
  if (preCalcSuitability.Size() == 0)
    preCalcSuitability.resize(SuitFunction.Size());

  /* remember that Suits is a friend of Predator.
   * Therefore we access pred through protected functions, but we only
   * use const access functions to access the length group division of
   * pred and its preys -- we do not change anything in pred. */

  DoubleMatrix* temp = 0;
  BandMatrix* suit = 0;
  double mult;
  int i, j, p;

  for (p = 0; p < SuitFunction.Size(); p++) {
    SuitFunction[p]->updateConstants(TimeInfo);
    if (SuitFunction[p]->constantsHaveChanged(TimeInfo)) {
      temp = new DoubleMatrix(pred->numLengthGroups(), pred->Preys(p)->numLengthGroups(), 0.0);
      for (i = 0; i < pred->numLengthGroups(); i++) {
        for (j = 0; j < pred->Preys(p)->numLengthGroups(); j++) {
          if (SuitFunction[p]->usesPreyLength())
            SuitFunction[p]->setPreyLength(pred->Preys(p)->Length(j));

          if (SuitFunction[p]->usesPredLength())
            SuitFunction[p]->setPredLength(pred->Length(i));

          (*temp)[i][j] = SuitFunction[p]->calculate();
          if ((*temp)[i][j] < 0)
            (*temp)[i][j] = 0.0;
        }
      }
      suit = new BandMatrix(*temp);
      preCalcSuitability.ChangeElement(p, *suit);
      delete suit;
      delete temp;
    }
  }

  #ifdef SUIT_SCALE
  //Scaling of suitabilities, so that in each lengthgroup of each predator, the
  //maximum suitability is exactly 1, if any suitability is different from 0.
  for (i = 0; i < pred->numLengthGroups(); i++) {
    mult = 0.0;
    for (p = 0; p < preynames.Size(); p++)
      if (i >= preCalcSuitability[p].minRow() && i <= preCalcSuitability[p].maxRow())
        for (j = preCalcSuitability[p].minCol(i); j < preCalcSuitability[p].maxCol(i); j++)
          mult = max(preCalcSuitability[p][i][j], mult);

    if (!(isZero(mult)))
      for (p = 0; p < preynames.Size(); p++)
        if (i >= preCalcSuitability[p].minRow() && i <= preCalcSuitability[p].maxRow())
          for (j = preCalcSuitability[p].minCol(i); j < preCalcSuitability[p].maxCol(i); j++)
            preCalcSuitability[p][i][j] /= mult;
  }
  #endif
}

int Suits::DidChange(int prey, const TimeClass* const TimeInfo) const {
  return SuitFunction[prey]->constantsHaveChanged(TimeInfo);
}

SuitFunc* Suits::FuncPrey(int prey) {
  return SuitFunction[prey];
}
