#include "predatoroveraggregator.h"
#include "predator.h"
#include "mathfunc.h"
#include "checkconversion.h"

PredatorOverAggregator::PredatorOverAggregator(const Predatorptrvector& preds,
  const intmatrix& Areas, const LengthGroupDivision* const predLgrpDiv)
  : predators(preds), areas(Areas) {

  int i, j;
  //First we check that the length group of every predator is finer
  //(not necessarily strictly finer) than that of predLgrpDiv;
  for (i = 0; i < predators.Size(); i++)
    CheckLengthGroupIsFiner(preds[i]->ReturnLengthGroupDiv(),
      predLgrpDiv, preds[i]->Name(), "predator overconsumption");

  for (i = 0; i < predators.Size(); i++) {
    predConv.AddRows(1, predators[i]->NoLengthGroups());
    for (j = 0; j < predConv.Ncol(i); j++)
      predConv[i][j] = predLgrpDiv->NoLengthGroup(predators[i]->Length(j));
  }

  total.AddRows(areas.Nrow(), predLgrpDiv->NoLengthGroups(), 0);
}

void PredatorOverAggregator::Sum() {
  int i, j, g, l;
  int area, predlength;

  //Initialize total to 0.
  for (i = 0; i < total.Nrow(); i++)
    for (j = 0; j < total.Ncol(i); j++)
      total[i][j] = 0;

  //Sum over the appropriate predators, areas, and lengths.
  for (g = 0; g < predators.Size(); g++)
    for (i = 0; i < areas.Nrow(); i++)
      for (j = 0; j < areas.Ncol(i); j++) {
        area = areas[i][j];
        if (predators[g]->IsInArea(area)) {
          const doublevector* dptr = &predators[g]->OverConsumption(area);
          for (l = 0; l < predConv.Ncol(g); l++) {
            predlength = predConv[g][l];
            if (predlength >= 0)
              total[i][predlength] += (*dptr)[l];
          }
        }
      }
}

const doublematrix& PredatorOverAggregator::ReturnSum() const {
  return total;
}
