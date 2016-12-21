#include "predatoroveraggregator.h"
#include "predator.h"
#include "errorhandler.h"
#include "gadget.h"
#include "global.h"

PredatorOverAggregator::PredatorOverAggregator(const PredatorPtrVector& preds,
  const IntMatrix& Areas, const LengthGroupDivision* const LgrpDiv)
  : predators(preds), areas(Areas) {

  int i, j;
  for (i = 0; i < predators.Size(); i++) {
    if (!checkLengthGroupStructure(predators[i]->getLengthGroupDiv(), LgrpDiv))
      handle.logMessage(LOGFAIL, "Error in predatoroveraggregator - invalid length group structure");
    predConv.AddRows(1, predators[i]->getLengthGroupDiv()->numLengthGroups(), -1);
    for (j = 0; j < predConv.Ncol(i); j++)
      predConv[i][j] = LgrpDiv->numLengthGroup(predators[i]->getLengthGroupDiv()->meanLength(j));
  }
  total.AddRows(areas.Nrow(), LgrpDiv->numLengthGroups(), 0.0);
}

void PredatorOverAggregator::Reset() {
  total.setToZero();
}

void PredatorOverAggregator::Sum() {
  int i, j, k, l;

  this->Reset();
  //Sum over the appropriate predators, areas, and lengths.
  for (k = 0; k < predators.Size(); k++)
    for (i = 0; i < areas.Nrow(); i++)
      for (j = 0; j < areas.Ncol(i); j++)
        if ((predators[k]->isInArea(areas[i][j])) && (predators[k]->hasOverConsumption(areas[i][j])))
          for (l = 0; l < predConv.Ncol(k); l++)
            if (predConv[k][l] >= 0)
              total[i][predConv[k][l]] += (predators[k]->getOverConsumption(areas[i][j]))[l];

}
