#include "predatoraggregator.h"
#include "prey.h"
#include "predator.h"
#include "mathfunc.h"
#include "popinfovector.h"
#include "errorhandler.h"
#include "gadget.h"

extern ErrorHandler handle;

PredatorAggregator::PredatorAggregator(const PredatorPtrVector& Predators,
  const PreyPtrVector& Preys, const IntMatrix& Areas,
  const LengthGroupDivision* const predLgrpDiv, const LengthGroupDivision* const preyLgrpDiv)
  : predators(Predators), preys(Preys), areas(Areas),
    doeseat(Predators.Size(), Preys.Size(), 0), bptr(0) {

  int i, j;

  for (i = 0; i < predators.Size(); i++)
    checkLengthGroupIsFiner(predators[i]->getLengthGroupDiv(), predLgrpDiv);

  for (i = 0; i < preys.Size(); i++)
    checkLengthGroupIsFiner(preys[i]->getLengthGroupDiv(), preyLgrpDiv);

  for (i = 0; i < predators.Size(); i++) {
    predConv.AddRows(1, predators[i]->numLengthGroups(), 0);
    for (j = 0; j < predConv.Ncol(i); j++)
      predConv[i][j] = predLgrpDiv->numLengthGroup(predators[i]->meanLength(j));
  }
  for (i = 0; i < preys.Size(); i++) {
    preyConv.AddRows(1, preys[i]->numLengthGroups(), 0);
    for (j = 0; j < preyConv.Ncol(i); j++)
      preyConv[i][j] = preyLgrpDiv->numLengthGroup(preys[i]->meanLength(j));
  }

  for (i = 0; i < predators.Size(); i++)
    for (j = 0; j < preys.Size(); j++)
      if (predators[i]->doesEat(preys[j]->getName()))
        doeseat[i][j] = 1;

  DoubleMatrix dm(predLgrpDiv->numLengthGroups(), preyLgrpDiv->numLengthGroups(), 1.0);
  BandMatrix bm(dm, 0, 0);
  total.resize(areas.Nrow(), bm);
  this->Reset();
}

void PredatorAggregator::Reset() {
  int i, j, k;

  for (i = 0; i < total.Size(); i++)
    for (j = 0; j < total[i].Nrow(); j++)
      for (k = 0; k < total[i].Ncol(j); k++)
        total[i][j][k] = 0.0;
}

void PredatorAggregator::Sum() {
  int area, g, h, i, j, k, l;

  this->Reset();
  //Sum over the appropriate preys, predators, areas, and lengths.
  for (g = 0; g < predators.Size(); g++) {
    for (h = 0; h < preys.Size(); h++) {
      if (doeseat[g][h]) {
        for (l = 0; l < areas.Nrow(); l++) {
          for (j = 0; j < areas.Ncol(l); j++) {
            area = areas[l][j];
            if (predators[g]->isInArea(area) && preys[h]->isInArea(area)) {
              bptr = &predators[g]->getConsumption(area, preys[h]->getName());
              for (k = bptr->minRow(); k <= bptr->maxRow(); k++)
                if (predConv[g][k] >= 0)
                  for (i = bptr->minCol(k); i < bptr->maxCol(k); i++)
                    if (preyConv[h][i] >= 0)
                      total[l][predConv[g][k]][preyConv[h][i]] += (*bptr)[k][i];

            }
          }
        }
      }
    }
  }
}

//Sum the numbers (not biomass)
void PredatorAggregator::NumberSum() {
  int area, i, j, k, g, h, l;
  const PopInfoVector* preymeanw;

  this->Reset();
  //Sum over the appropriate preys, predators, areas, and lengths.
  for (g = 0; g < predators.Size(); g++) {
    for (h = 0; h < preys.Size(); h++) {
      if (doeseat[g][h]) {
        for (l = 0; l < areas.Nrow(); l++) {
          for (j = 0; j < areas.Ncol(l); j++) {
            area = areas[l][j];
            if (predators[g]->isInArea(area) && preys[h]->isInArea(area)) {
              bptr = &predators[g]->getConsumption(area, preys[h]->getName());
              preymeanw = &predators[g]->getNumberPriorToEating(area, preys[h]->getName());
              for (k = bptr->minRow(); k <= bptr->maxRow(); k++)
                if (predConv[g][k] >= 0)
                  for (i = bptr->minCol(k); i < bptr->maxCol(k); i++)
                    if (preyConv[h][i] >= 0 && (!(isZero((*preymeanw)[i].W))))
                      total[l][predConv[g][k]][preyConv[h][i]] += (*bptr)[k][i] / (*preymeanw)[i].W;

            }
          }
        }
      }
    }
  }
}
