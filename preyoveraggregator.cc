#include "preyoveraggregator.h"
#include "prey.h"
#include "mathfunc.h"
#include "conversion.h"
#include "checkconversion.h"

PreyOverAggregator::PreyOverAggregator(const PreyPtrVector& Preys,
  const IntMatrix& Areas, const LengthGroupDivision* const preyLgrpDiv)
  : preys(Preys), areas(Areas) {

  int i, j;
  for (i = 0; i < preys.Size(); i++)
    CheckLengthGroupIsFiner(preys[i]->ReturnLengthGroupDiv(),
      preyLgrpDiv, preys[i]->Name(), "prey overconsumption");

  for (i = 0; i < preys.Size(); i++) {
    preyConv.AddRows(1, preys[i]->NoLengthGroups());
    for (j = 0; j < preyConv.Ncol(i); j++)
      preyConv[i][j] = preyLgrpDiv->NoLengthGroup(preys[i]->Length(j));
  }

  //Now preyConv[p][l] is the number of length group in which length group l of prey p is;
  //Resize total and initialize to 0.
  total.AddRows(areas.Nrow(), preyLgrpDiv->NoLengthGroups(), 0);
}

void PreyOverAggregator::Sum() {
  int i, j, h, l;
  int area, preylength;

  //Initialize total to 0.
  for (i = 0; i < total.Nrow(); i++)
    for (j = 0; j < total.Ncol(i); j++)
      total[i][j] = 0;

  //Sum over the appropriate preys, areas, and lengths.
  for (h = 0; h < preys.Size(); h++)
    for (i = 0; i < areas.Nrow(); i++)
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

const DoubleMatrix& PreyOverAggregator::ReturnSum() const {
  return total;
}
