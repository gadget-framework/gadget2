#include "predatoroveraggregator.h"
#include "predator.h"

PredatorOverAggregator::PredatorOverAggregator(const PredatorPtrVector& preds,
  const IntMatrix& Areas, const LengthGroupDivision* const LgrpDiv)
  : predators(preds), areas(Areas) {

  int i, j;
  for (i = 0; i < predators.Size(); i++)
    checkLengthGroupIsFiner(preds[i]->returnLengthGroupDiv(), LgrpDiv);

  total.AddRows(areas.Nrow(), LgrpDiv->numLengthGroups(), 0.0);
  for (i = 0; i < predators.Size(); i++) {
    predConv.AddRows(1, predators[i]->numLengthGroups(), 0);
    for (j = 0; j < predConv.Ncol(i); j++)
      predConv[i][j] = LgrpDiv->numLengthGroup(predators[i]->Length(j));
  }
}

void PredatorOverAggregator::Sum() {
  int i, j, g, l;
  int area, predlength;

  for (i = 0; i < total.Nrow(); i++)
    for (j = 0; j < total.Ncol(i); j++)
      total[i][j] = 0.0;

  //Sum over the appropriate predators, areas, and lengths.
  for (g = 0; g < predators.Size(); g++) {
    for (i = 0; i < areas.Nrow(); i++) {
      for (j = 0; j < areas.Ncol(i); j++) {
        area = areas[i][j];
        if (predators[g]->IsInArea(area)) {
          const DoubleVector* dptr = &predators[g]->OverConsumption(area);
          for (l = 0; l < predConv.Ncol(g); l++) {
            predlength = predConv[g][l];
            if (predlength >= 0)
              total[i][predlength] += (*dptr)[l];
          }
        }
      }
    }
  }
}
