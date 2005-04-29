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
      predConv[i][j] = LgrpDiv->numLengthGroup(predators[i]->meanLength(j));
  }
}

void PredatorOverAggregator::Reset() {
  int i, j;
  for (i = 0; i < total.Nrow(); i++)
    for (j = 0; j < total.Ncol(i); j++)
      total[i][j] = 0.0;
}

void PredatorOverAggregator::Sum() {
  int i, j, g, l;

  this->Reset();
  //Sum over the appropriate predators, areas, and lengths.
  for (g = 0; g < predators.Size(); g++)
    for (i = 0; i < areas.Nrow(); i++)
      for (j = 0; j < areas.Ncol(i); j++)
        if (predators[g]->isInArea(areas[i][j]))
          for (l = 0; l < predConv.Ncol(g); l++)
            if (predConv[g][l] >= 0)
              total[i][predConv[g][l]] += (predators[g]->getOverConsumption(areas[i][j]))[l];

}
