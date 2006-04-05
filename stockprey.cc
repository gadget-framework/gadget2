#include "stockprey.h"

StockPrey::StockPrey(CommentStream& infile, const IntVector& Areas,
  const char* givenname, int minage, int maxage)
  : Prey(infile, Areas, givenname) {

  type = STOCKPREYTYPE;
  IntVector size(maxage - minage + 1, LgrpDiv->numLengthGroups());
  IntVector minlength(maxage - minage + 1, 0);
  preyAlkeys.resize(areas.Size(), minage, minlength, size);
}

void StockPrey::Sum(const AgeBandMatrix& stockAlkeys, int area) {
  int i, inarea = this->areaNum(area);
  preyAlkeys[inarea].setToZero();
  preyAlkeys[inarea].Add(stockAlkeys, *CI);
  preyAlkeys[inarea].sumColumns(preynumber[inarea]);
  total[inarea] = 0.0;
  for (i = 0; i < preynumber.Ncol(inarea); i++) {
    biomass[inarea][i] = preynumber[inarea][i].N * preynumber[inarea][i].W;
    total[inarea] += biomass[inarea][i];
    cons[inarea][i] = 0.0;
  }
}

void StockPrey::Print(ofstream& outfile) const {
  Prey::Print(outfile);
  outfile << "\nStock prey\n";
  int i;
  for (i = 0; i < preyAlkeys.Size(); i++) {
    outfile << "\tAlkeys on internal area " << areas[i] << ":\n";
    preyAlkeys[i].printNumbers(outfile);
  }
}
