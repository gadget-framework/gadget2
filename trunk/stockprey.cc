#include "stockprey.h"

StockPrey::StockPrey(CommentStream& infile, const IntVector& Areas,
  const char* givenname, int minage, int numage,
  const TimeClass* const TimeInfo, Keeper* const keeper)
  : Prey(infile, Areas, givenname, TimeInfo, keeper) {

  int i;
  type = STOCKPREY;
  IntVector lower(numage, 0);
  IntVector agesize(numage, LgrpDiv->numLengthGroups());
  preyAlkeys.resize(areas.Size(), minage, lower, agesize);
  for (i = 0; i < preyAlkeys.Size(); i++)
    preyAlkeys[i].setToZero();
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
  int area;
  Prey::Print(outfile);
  outfile << "\n\tStock prey age length keys\n";
  for (area = 0; area < areas.Size(); area++) {
    outfile << "\tInternal area " << areas[area] << "\n\tNumber\n";
    preyAlkeys[area].printNumbers(outfile);
    outfile << "\tMean weight\n";
    preyAlkeys[area].printWeights(outfile);
  }
}
