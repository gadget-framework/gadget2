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
  for (i = 0; i < FuncPreynames.Size(); i++)
    delete[] FuncPreynames[i];
  for (i = 0; i < MatrixPreynames.Size(); i++)
    delete[] MatrixPreynames[i];
  for (i = 0; i < MatrixSuit.Size(); i++)
    delete MatrixSuit[i];
}

Suits::Suits(const Suits& initial, Keeper* const keeper)
  : FuncPreynames(initial.FuncPreynames), MatrixPreynames(initial.MatrixPreynames),
    Multiplication(initial.Multiplication), MatrixSuit(initial.MatrixSuit) {

  int i;

  for (i = 0; i < initial.NoFuncPreys(); i++)
    SuitFunction.resize(1, initial.SuitFunction[i]);

  //Inform keeper of MatrixPreys.
  for (i = 0; i < MatrixPreynames.Size(); i++) {
    MatrixPreynames[i] = new char[strlen(initial.MatrixPreynames[i]) + 1];
    strcpy(MatrixPreynames[i], initial.MatrixPreynames[i]);
    MatrixSuit[i] = new doublematrix(*initial.MatrixSuit[i]);
    keeper->ChangeVariable(initial.Multiplication[i], Multiplication[i]);
  }

  for (i = 0; i < PrecalcSuitability.Size(); i++)
    PrecalcSuitability.resize(1, initial.PrecalcSuitability[i]);
}

void Suits::AddPrey(const char* preyname, SuitFunc* suitf) {
  int i = FuncPreynames.Size();
  FuncPreynames.resize(1);
  SuitFunction.resize(1, suitf);
  FuncPreynames[i] = new char[strlen(preyname) + 1];
  strcpy(FuncPreynames[i], preyname);
}

void Suits::AddPrey(const char* preyname, double multiplication,
  const doublematrix& suitabilities, Keeper* const keeper) {

  int i = MatrixPreynames.Size();
  MatrixPreynames.resize(1);
  Multiplication.resize(1, multiplication);
  doublematrix* dm = new doublematrix(suitabilities);
  MatrixSuit.resize(1, dm);
  MatrixPreynames[i] = new char[strlen(preyname) + 1];
  strcpy(MatrixPreynames[i], preyname);
  keeper->ChangeVariable(multiplication, Multiplication[Multiplication.Size() - 1]);
}

const char* Suits::Preyname(int prey) const {
  if (prey < FuncPreynames.Size())
    return FuncPreynames[prey];
  else
    return MatrixPreynames[prey - SuitFunction.Size()];
}

int Suits::NoPreys() const {
  return SuitFunction.Size() + MatrixPreynames.Size();
}

int Suits::NoFuncPreys() const {
  return SuitFunction.Size();
}

const bandmatrix& Suits::Suitable(int prey) const {
  return PrecalcSuitability[prey];
}

void Suits::DeletePrey(int prey, Keeper* const keeper) {
  if (prey < SuitFunction.Size())
    this->DeleteFuncPrey(prey, keeper);
  else
    this->DeleteMatrixPrey(prey - SuitFunction.Size(), keeper);

  /* The functions above delete the information received through the
   * member functions, not calculated information about the preys.
   * Therefore we must delete it here.
   * Remember that it is quite possible that PrecalcSuitability has
   * been resized, possible that it has not. */
  if (PrecalcSuitability.Size() != 0)
    PrecalcSuitability.Delete(prey);
}

//Calculate suitabilities and set in the bandmatrixvector PrecalcSuitability.
void Suits::Reset(const Predator* const pred, const TimeClass* const TimeInfo) {
  //First time.
  if (PrecalcSuitability.Size() == 0)
    PrecalcSuitability.resize(SuitFunction.Size()+MatrixPreynames.Size());

  /* remember that Suits is a friend of Predator.
   * Therefore we access pred through protected functions, but we only
   * use const access functions to access the length group division of
   * pred and its preys -- we do not change anything in pred. */

  doublematrix* temp = 0;
  bandmatrix* suit = 0;
  double mult;
  int i, j, p;

  for (p = 0; p < SuitFunction.Size(); p++) {
    SuitFunction[p]->updateConstants(TimeInfo);
    if (SuitFunction[p]->constantsHaveChanged(TimeInfo)) {
      temp = new doublematrix(pred->NoLengthGroups(), pred->Preys(p)->NoLengthGroups(), 0.0);
      for (i = 0; i < pred->NoLengthGroups(); i++) {
        for (j = 0; j < pred->Preys(p)->NoLengthGroups(); j++) {
          if (SuitFunction[p]->usesPreyLength())
            SuitFunction[p]->setPreyLength(pred->Preys(p)->Length(j));

          if (SuitFunction[p]->usesPredLength())
            SuitFunction[p]->setPredLength(pred->Length(i));

          (*temp)[i][j] = SuitFunction[p]->calculate();
          if ((*temp)[i][j] < 0)
            (*temp)[i][j] = 0;
        }
      }
      suit = new bandmatrix(*temp);
      PrecalcSuitability.ChangeElement(p, *suit);
      delete suit;
      delete temp;
    }
  }

  int shift = SuitFunction.Size();
  if (TimeInfo->CurrentTime() == 1) {
    for (p = 0; p < MatrixPreynames.Size(); p++) {
      temp = new doublematrix(*MatrixSuit[p]);
      mult = Multiplication[p];
      assert(temp->Nrow() == pred->NoLengthGroups());
      for (i = 0; i < pred->NoLengthGroups(); i++) {
        assert(temp->Ncol(i) ==  pred->Preys(p + shift)->NoLengthGroups());
        for (j = 0; j < pred->Preys(p + shift)->NoLengthGroups(); j++)
          if ((*temp)[i][j] < 0)
            (*temp)[i][j] = 0;
          else
            (*temp)[i][j] *= mult;
      }
      suit = new bandmatrix(*temp);
      PrecalcSuitability.resize(1, *suit);
      delete suit;
      delete temp;
    }
  }

  #ifdef SUIT_SCALE
  //Scaling of suitabilities, so that in each lengthgroup of each predator, the
  //maximum suitability is exactly 1, if any suitability is different from 0.
  double maxL;
  for (i = 0; i < pred->NoLengthGroups(); i++) {
    maxL = 0;
    for (p = 0; p < FuncPreynames.Size() + MatrixPreynames.Size(); p++)
      if (i >= PrecalcSuitability[p].Minrow() && i <= PrecalcSuitability[p].Maxrow())
        for (j = PrecalcSuitability[p].Mincol(i);
            j < PrecalcSuitability[p].Maxcol(i); j++)
          maxL = max(PrecalcSuitability[p][i][j], maxL);

    if (maxL != 0)
      for (p = 0; p < FuncPreynames.Size() + MatrixPreynames.Size(); p++)
        if (i >= PrecalcSuitability[p].Minrow() && i <= PrecalcSuitability[p].Maxrow())
          for (j = PrecalcSuitability[p].Mincol(i); j < PrecalcSuitability[p].Maxcol(i); j++)
            PrecalcSuitability[p][i][j] /= maxL;
  }
  #endif
}

void Suits::DeleteFuncPrey(int prey, Keeper* const keeper) {
  assert( prey >= 0);
  assert(prey < SuitFunction.Size());
  SuitFunction.Delete(prey, keeper);
  FuncPreynames.Delete(prey);
}

void Suits::DeleteMatrixPrey(int prey, Keeper* const keeper) {
  delete[] MatrixPreynames[prey];
  MatrixPreynames.Delete(prey);
  keeper->DeleteParam(Multiplication[prey]);
  Multiplication.Delete(prey);
  delete MatrixSuit[prey];
}

int Suits::DidChange(int prey, const TimeClass* const TimeInfo) const {
  assert(prey >= 0);
  assert(prey < SuitFunction.Size());
  return SuitFunction[prey]->constantsHaveChanged(TimeInfo);
}

SuitFunc* Suits::FuncPrey(int prey) {
  assert(prey > 0);
  assert(prey < SuitFunction.Size());
  return SuitFunction[prey];
}
