#include "stockprey.h"

StockPrey::StockPrey(CommentStream& infile, const IntVector& Areas,
  const char* givenname, int minage, int maxage, Keeper* const keeper)
  : Prey(infile, Areas, givenname, keeper) {

  type = STOCKPREYTYPE;
  IntVector size(maxage - minage + 1, LgrpDiv->NoLengthGroups());
  IntVector minlength(maxage - minage + 1, 0);
  Alkeys.resize(areas.Size(), minage, minlength, size);
}

void StockPrey::Sum(const AgeBandMatrix& stock, int area, int CurrentSubstep) {
  int i, inarea = AreaNr[area];

  tooMuchConsumption[inarea] = 0;
  for (i = 0; i < cons.Ncol(inarea); i++)
    cons[inarea][i] = 0.0;
  for (i = 0; i < Number[inarea].Size(); i++)
    Number[inarea][i].N = 0.0;

  Alkeys[inarea].setToZero();
  Alkeys[inarea].Add(stock, *CI);
  Alkeys[inarea].Colsum(Number[inarea]);

  PopInfo sum;
  for (i = 0; i < Number.Ncol(inarea); i++) {
    sum += Number[inarea][i];
    biomass[inarea][i] = Number[inarea][i].N * Number[inarea][i].W;
  }

  total[inarea] = sum.N * sum.W;
  for (i = 0; i < Number[inarea].Size(); i++)
    numberPriortoEating[inarea][i] = Number[inarea][i]; //should be inside if

  if (CurrentSubstep == 1) {
    for (i = 0; i < consumption.Ncol(inarea); i++) {
      consumption[inarea][i] = 0.0;
      overconsumption[inarea][i] = 0.0;
    }
  }
}

void StockPrey::Print(ofstream& outfile) const {
  Prey::Print(outfile);
  outfile << "\nStock prey\n";
  int area;
  for (area = 0; area < areas.Size(); area++) {
    outfile << "\tAlkeys on internal area " << areas[area] << endl;
    Alkeys[area].printNumbers(outfile);
  }
}

void StockPrey::Reset() {
  this->Prey::Reset();
  int area, age, l;
  for (area = 0; area < areas.Size(); area++) {
    for (age = Alkeys[area].minAge(); age <= Alkeys[area].maxAge(); age++) {
      for (l = Alkeys[area].minLength(age); l < Alkeys[area].maxLength(age); l++) {
        Alkeys[area][age][l].N = 0.0;
        Alkeys[area][age][l].W = 0.0;
      }
    }
  }
}
