#include "stockprey.h"
#include "print.h"

StockPrey::StockPrey(CommentStream& infile, const intvector& Areas,
  const char* givenname, int minage, int maxage, Keeper* const keeper)
  : Prey(infile, Areas, givenname, keeper) {

  intvector size(maxage - minage + 1, LgrpDiv->NoLengthGroups());
  intvector minlength(maxage - minage + 1, 0);
  Alkeys.resize(areas.Size(), minage, minlength, size);
}

StockPrey::StockPrey(const doublevector& lengths, const intvector& Areas,
  int minage, int maxage, const char* givenname)
  : Prey(lengths, Areas, givenname) {

  intvector size(maxage - minage + 1, LgrpDiv->NoLengthGroups());
  intvector minlength(maxage - minage + 1, 0);
  Alkeys.resize(areas.Size(), minage, minlength, size);
}

StockPrey::~StockPrey() {
}

void StockPrey::Sum(const Agebandmatrix& stock, int area, int CurrentSubstep) {
  int inarea = AreaNr[area];
  int i, j;

  tooMuchConsumption[inarea] = 0;
  for (j = 0; j < cons.Ncol(inarea); j++)
    cons[inarea][j] = 0.0;
  for (i = 0; i < Number[inarea].Size(); i++)
    Number[inarea][i].N = 0.0;

  Alkeys[inarea].SettoZero();
  AgebandmAdd(Alkeys[inarea], stock, *CI);
  Alkeys[inarea].Colsum(Number[inarea]);

  popinfo sum;
  for (i = 0; i < Number.Ncol(inarea); i++) {
    sum += Number[inarea][i];
    biomass[inarea][i] = Number[inarea][i].N * Number[inarea][i].W;
  }

  total[inarea] = sum.N * sum.W;
  for (i = 0; i < Number[inarea].Size(); i++)
    numberPriortoEating[inarea][i] = Number[inarea][i]; //should be inside if

  if (CurrentSubstep == 1) {
    for (j = 0; j < consumption.Ncol(inarea); j++) {
      consumption[inarea][j] = 0.0;
      overconsumption[inarea][j] = 0.0;
    }
  }
}

const Agebandmatrix& StockPrey::AlkeysPriorToEating(int area) const {
  return Alkeys[AreaNr[area]];
}

void StockPrey::Print(ofstream& outfile) const {
  outfile << "Stock Prey\n";
  int area;
  for (area = 0; area < areas.Size(); area++) {
    outfile << "Alkeys on area " << areas[area] << endl;
    Printagebandm(outfile, Alkeys[area]);
  }
  Prey::Print(outfile);
}

void StockPrey::Reset() {
  this->Prey::Reset();
  int area, age, l;
  for (area = 0; area < areas.Size(); area++)
    for (age = Alkeys[area].Minage(); age <= Alkeys[area].Maxage(); age++)
      for (l = Alkeys[area].Minlength(age);
          l < Alkeys[area].Maxlength(age); l++) {
        Alkeys[area][age][l].N = 0.0;
        Alkeys[area][age][l].W = 0.0;
      }
}
