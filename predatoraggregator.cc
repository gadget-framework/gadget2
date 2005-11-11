#include "predatoraggregator.h"
#include "prey.h"
#include "predator.h"
#include "stockpredator.h"
#include "mathfunc.h"
#include "popinfovector.h"
#include "errorhandler.h"
#include "gadget.h"

extern ErrorHandler handle;

PredatorAggregator::PredatorAggregator(const PredatorPtrVector& Predators,
  const PreyPtrVector& Preys, const IntMatrix& Areas,
  const LengthGroupDivision* const predLgrpDiv, const LengthGroupDivision* const preyLgrpDiv)
  : predators(Predators), preys(Preys), areas(Areas),
    doeseat(Predators.Size(), Preys.Size(), 0), dptr(0) {

  int i, j;

  for (i = 0; i < predators.Size(); i++) {
    checkLengthGroupIsFiner(predators[i]->getLengthGroupDiv(), predLgrpDiv);
    predConv.AddRows(1, predators[i]->getLengthGroupDiv()->numLengthGroups(), -1);
    for (j = 0; j < predConv.Ncol(i); j++)
      predConv[i][j] = predLgrpDiv->numLengthGroup(predators[i]->getLengthGroupDiv()->meanLength(j));
  }

  for (i = 0; i < preys.Size(); i++) {
    checkLengthGroupIsFiner(preys[i]->getLengthGroupDiv(), preyLgrpDiv);
    preyConv.AddRows(1, preys[i]->getLengthGroupDiv()->numLengthGroups(), -1);
    for (j = 0; j < preyConv.Ncol(i); j++)
      preyConv[i][j] = preyLgrpDiv->numLengthGroup(preys[i]->getLengthGroupDiv()->meanLength(j));
  }

  for (i = 0; i < predators.Size(); i++)
    for (j = 0; j < preys.Size(); j++)
      if (predators[i]->doesEat(preys[j]->getName()))
        doeseat[i][j] = 1;

  total.resize(areas.Nrow(), new DoubleMatrix(predLgrpDiv->numLengthGroups(),  preyLgrpDiv->numLengthGroups(), 0.0));
  this->Reset();
}

PredatorAggregator::PredatorAggregator(const PredatorPtrVector& Predators,
  const PreyPtrVector& Preys, const IntMatrix& Areas,
  const IntMatrix& predAges, const LengthGroupDivision* const preyLgrpDiv)
  : predators(Predators), preys(Preys), areas(Areas),
    doeseat(Predators.Size(), Preys.Size(), 0), dptr(0) {

  int i, j, k, l;
  int minage, maxage;

  for (i = 0; i < predators.Size(); i++) {
    minage = ((StockPredator*)(predators[i]))->minAge();
    maxage = ((StockPredator*)(predators[i]))->maxAge();
    predConv.AddRows(1, maxage + 1, -1);
    for (j = minage; j <= maxage; j++)
      for (k = 0; k < predAges.Nrow(); k++)
        for (l = 0; l < predAges.Ncol(k); l++)
          if (j == predAges[k][l])
             predConv[i][j] = k;
  }

  for (i = 0; i < preys.Size(); i++) {
    checkLengthGroupIsFiner(preys[i]->getLengthGroupDiv(), preyLgrpDiv);
    preyConv.AddRows(1, preys[i]->getLengthGroupDiv()->numLengthGroups(), -1);
    for (j = 0; j < preyConv.Ncol(i); j++)
      preyConv[i][j] = preyLgrpDiv->numLengthGroup(preys[i]->getLengthGroupDiv()->meanLength(j));
  }

  for (i = 0; i < predators.Size(); i++)
    for (j = 0; j < preys.Size(); j++)
      if (predators[i]->doesEat(preys[j]->getName()))
        doeseat[i][j] = 1;

  total.resize(areas.Nrow(), new DoubleMatrix(predAges.Nrow(),  preyLgrpDiv->numLengthGroups(), 0.0));
  this->Reset();
}

PredatorAggregator::~PredatorAggregator() {
  int i;
  for (i = 0; i < total.Size(); i++)
    delete total[i];
}

void PredatorAggregator::Print(ofstream& outfile) const {
  int i, j, k;

  for (i = 0; i < total.Size(); i++) {
    outfile << "\t\tInternal areas " << i << endl;
    for (j = 0; j < total[i]->Nrow(); j++) {
      outfile << TAB << TAB;
      for (k = 0; k < total[i]->Ncol(j); k++)
        outfile << setw(smallwidth) << (*total[i])[j][k] << sep;
      outfile << endl;
    }
  }
  outfile.flush();
}

void PredatorAggregator::Reset() {
  int i, j, k;

  for (i = 0; i < total.Size(); i++)
    for (j = 0; j < total[i]->Nrow(); j++)
      for (k = 0; k < total[i]->Ncol(j); k++)
        (*total[i])[j][k] = 0.0;
}

void PredatorAggregator::Sum() {
  int g, h, i, j, k, l;

  this->Reset();
  //Sum over the appropriate preys, predators, areas and lengths.
  for (g = 0; g < predators.Size(); g++) {
    for (h = 0; h < preys.Size(); h++) {
      if (doeseat[g][h]) {
        for (l = 0; l < areas.Nrow(); l++) {
          for (j = 0; j < areas.Ncol(l); j++) {
            if (predators[g]->isInArea(areas[l][j]) && preys[h]->isInArea(areas[l][j])) {
              dptr = &predators[g]->getConsumption(areas[l][j], preys[h]->getName());
              for (k = 0; k < dptr->Nrow(); k++)
                if (predConv[g][k] >= 0)
                  for (i = 0; i < dptr->Ncol(k); i++)
                    if (preyConv[h][i] >= 0)
                      (*total[l])[predConv[g][k]][preyConv[h][i]] += (*dptr)[k][i];

            }
          }
        }
      }
    }
  }
}

//Sum the biomass for age-based predators
void PredatorAggregator::AgeSum() {
  int g, h, i, j, k, l, m;
  double sum, consum;
  const AgeBandMatrix* alk;

  this->Reset();
  //Sum over the appropriate preys, predators, areas, ages and lengths.
  for (g = 0; g < predators.Size(); g++) {
    for (h = 0; h < preys.Size(); h++) {
      if (doeseat[g][h]) {
        for (l = 0; l < areas.Nrow(); l++) {
          for (j = 0; j < areas.Ncol(l); j++) {
            if (predators[g]->isInArea(areas[l][j]) && preys[h]->isInArea(areas[l][j])) {
              alk = &((StockPredator*)(predators[g]))->getAgeLengthKeys(areas[l][j]);
              dptr = &predators[g]->getConsumption(areas[l][j], preys[h]->getName());
              //Need to convert from length groups (in dptr) to age groups (in alk)
              for (k = alk->minAge(); k <= alk->maxAge(); k++) {
                if (predConv[g][k] >= 0) {
                  for (i = 0; i < dptr->Ncol(); i++) {
                    if (preyConv[h][i] >= 0) {
                      sum = 0.0;
                      consum = 0.0;
                      for (m = 0; m < dptr->Nrow(); m++) {
                        sum += (*alk)[k][m].N;
                        consum += (*dptr)[m][i] * (*alk)[k][m].N;
                      }
                      if (!isZero(sum))
                        (*total[l])[predConv[g][k]][preyConv[h][i]] += consum / sum;
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
}

//Sum the numbers (not biomass)
void PredatorAggregator::NumberSum() {
  int g, h, i, j, k, l;
  const PopInfoVector* preymeanw;

  this->Reset();
  //Sum over the appropriate preys, predators, areas and lengths.
  for (g = 0; g < predators.Size(); g++) {
    for (h = 0; h < preys.Size(); h++) {
      if (doeseat[g][h]) {
        for (l = 0; l < areas.Nrow(); l++) {
          for (j = 0; j < areas.Ncol(l); j++) {
            if (predators[g]->isInArea(areas[l][j]) && preys[h]->isInArea(areas[l][j])) {
              dptr = &predators[g]->getConsumption(areas[l][j], preys[h]->getName());
              preymeanw = &predators[g]->getNumberPriorToEating(areas[l][j], preys[h]->getName());
              for (k = 0; k < dptr->Nrow(); k++)
                if (predConv[g][k] >= 0)
                  for (i = 0; i < dptr->Ncol(k); i++)
                    if (preyConv[h][i] >= 0 && (!(isZero((*preymeanw)[i].W))))
                      (*total[l])[predConv[g][k]][preyConv[h][i]] += (*dptr)[k][i] / (*preymeanw)[i].W;

            }
          }
        }
      }
    }
  }
}
