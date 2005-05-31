#include "stockprey.h"

StockPrey::StockPrey(CommentStream& infile, const IntVector& Areas,
  const char* givenname, int minage, int maxage)
  : Prey(infile, Areas, givenname) {

  type = STOCKPREYTYPE;
  IntVector size(maxage - minage + 1, LgrpDiv->numLengthGroups());
  IntVector minlength(maxage - minage + 1, 0);
  Alkeys.resize(areas.Size(), minage, minlength, size);
}

void StockPrey::Sum(const AgeBandMatrix& stock, int area, int CurrentSubstep) {
  int i, inarea = this->areaNum(area);
  tooMuchConsumption[inarea] = 0;

  for (i = 0; i < cons.Ncol(inarea); i++)
    cons[inarea][i] = 0.0;

  Alkeys[inarea].setToZero();
  Alkeys[inarea].Add(stock, *CI);
  Alkeys[inarea].sumColumns(Number[inarea]);

  PopInfo sum;
  for (i = 0; i < Number.Ncol(inarea); i++) {
    sum += Number[inarea][i];
    biomass[inarea][i] = Number[inarea][i].N * Number[inarea][i].W;
  }
  total[inarea] = sum.N * sum.W;

  if (CurrentSubstep == 1) {
    for (i = 0; i < consumption.Ncol(inarea); i++) {
      consumption[inarea][i] = 0.0;
      overconsumption[inarea][i] = 0.0;
    }
    for (i = 0; i < Number[inarea].Size(); i++)
      numberPriorToEating[inarea][i] = Number[inarea][i];
  }
}

void StockPrey::Print(ofstream& outfile) const {
  Prey::Print(outfile);
  outfile << "\nStock prey\n";
  int i;
  for (i = 0; i < Alkeys.Size(); i++) {
    outfile << "\tAlkeys on internal area " << areas[i] << ":\n";
    Alkeys[i].printNumbers(outfile);
  }
}

void StockPrey::Reset() {
  Prey::Reset();
  int i;
  for (i = 0; i < Alkeys.Size(); i++)
    Alkeys[i].setToZero();
}
