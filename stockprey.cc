#include "stockprey.h"

StockPrey::StockPrey(CommentStream& infile, const IntVector& Areas,
  const char* givenname, int minage, int maxage)
  : Prey(infile, Areas, givenname) {

  type = STOCKPREYTYPE;
  IntVector size(maxage - minage + 1, LgrpDiv->numLengthGroups());
  IntVector minlength(maxage - minage + 1, 0);
  Alkeys.resize(areas.Size(), minage, minlength, size);
}

void StockPrey::Sum(const AgeBandMatrix& stock, int area) {
  int i, inarea = this->areaNum(area);

  Alkeys[inarea].setToZero();
  Alkeys[inarea].Add(stock, *CI);
  Alkeys[inarea].sumColumns(preynumber[inarea]);

  PopInfo sum;
  for (i = 0; i < preynumber.Ncol(inarea); i++) {
    sum += preynumber[inarea][i];
    biomass[inarea][i] = preynumber[inarea][i].N * preynumber[inarea][i].W;
    cons[inarea][i] = 0.0;
  }
  total[inarea] = sum.N * sum.W;
  isoverconsumption[inarea] = 0;
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
