#include "preyoveraggregator.h"
#include "prey.h"

PreyOverAggregator::PreyOverAggregator(const PreyPtrVector& Preys,
  const IntMatrix& Areas, const LengthGroupDivision* const LgrpDiv)
  : preys(Preys), areas(Areas) {

  int i, j;
  for (i = 0; i < preys.Size(); i++)
    checkLengthGroupIsFiner(preys[i]->returnLengthGroupDiv(),
      LgrpDiv, preys[i]->Name(), "prey overconsumption");

  total.AddRows(areas.Nrow(), LgrpDiv->NoLengthGroups(), 0.0);
  for (i = 0; i < preys.Size(); i++) {
    preyConv.AddRows(1, preys[i]->NoLengthGroups(), 0);
    for (j = 0; j < preyConv.Ncol(i); j++)
      preyConv[i][j] = LgrpDiv->NoLengthGroup(preys[i]->Length(j));
  }
}

void PreyOverAggregator::Sum() {
  int i, j, h, l;
  int area, preylength;

  for (i = 0; i < total.Nrow(); i++)
    for (j = 0; j < total.Ncol(i); j++)
      total[i][j] = 0.0;

  //Sum over the appropriate preys, areas, and lengths.
  for (h = 0; h < preys.Size(); h++) {
    for (i = 0; i < areas.Nrow(); i++) {
      for (j = 0; j < areas.Ncol(i); j++) {
        area = areas[i][j];
        if (preys[h]->IsInArea(area)) {
          const DoubleVector* dptr = &preys[h]->OverConsumption(area);
          for (l = 0; l < preyConv.Ncol(h); l++) {
          preylength = preyConv[h][l];
          if (preylength >= 0)
            total[i][preylength] += (*dptr)[l];
          }
        }
      }
    }
  }
}
