#include "predatoraggregator.h"
#include "prey.h"
#include "predator.h"
#include "stockpredator.h"
#include "mathfunc.h"
#include "popinfovector.h"
#include "errorhandler.h"
#include "gadget.h"
#include "global.h"

PredatorAggregator::PredatorAggregator(const PredatorPtrVector& Predators,
  const PreyPtrVector& Preys, const IntMatrix& Areas,
  const LengthGroupDivision* const predLgrpDiv, const LengthGroupDivision* const preyLgrpDiv)
  : predators(Predators), preys(Preys), areas(Areas),
    doeseat(Predators.Size(), Preys.Size(), 0), dptr(0), alk(0), usepredages(0) {

  int i, j;
  for (i = 0; i < predators.Size(); i++) {
    if (!checkLengthGroupStructure(predators[i]->getLengthGroupDiv(), predLgrpDiv))
      handle.logMessage(LOGFAIL, "Error in predatoraggregator - invalid predator length group structure");
    predConv.AddRows(1, predators[i]->getLengthGroupDiv()->numLengthGroups(), -1);
    for (j = 0; j < predConv.Ncol(i); j++)
      predConv[i][j] = predLgrpDiv->numLengthGroup(predators[i]->getLengthGroupDiv()->meanLength(j));
  }

  for (i = 0; i < preys.Size(); i++) {
    if (!checkLengthGroupStructure(preys[i]->getLengthGroupDiv(), preyLgrpDiv))
      handle.logMessage(LOGFAIL, "Error in predatoraggregator - invalid prey length group structure");
    preyConv.AddRows(1, preys[i]->getLengthGroupDiv()->numLengthGroups(), -1);
    for (j = 0; j < preyConv.Ncol(i); j++)
      preyConv[i][j] = preyLgrpDiv->numLengthGroup(preys[i]->getLengthGroupDiv()->meanLength(j));
  }

  for (i = 0; i < predators.Size(); i++)
    for (j = 0; j < preys.Size(); j++)
      if (predators[i]->doesEat(preys[j]->getName()))
        doeseat[i][j] = 1;

  for (i = 0; i < areas.Nrow(); i++)
    total.resize(new DoubleMatrix(predLgrpDiv->numLengthGroups(), preyLgrpDiv->numLengthGroups(), 0.0));
}

PredatorAggregator::PredatorAggregator(const PredatorPtrVector& Predators,
  const PreyPtrVector& Preys, const IntMatrix& Areas,
  const IntMatrix& predAges, const LengthGroupDivision* const preyLgrpDiv)
  : predators(Predators), preys(Preys), areas(Areas),
    doeseat(Predators.Size(), Preys.Size(), 0), dptr(0), alk(0), usepredages(1) {

  int i, j, k, l, minage, maxage;
  for (i = 0; i < predators.Size(); i++) {
    if (predators[i]->getType() != STOCKPREDATOR)
      handle.logMessage(LOGFAIL, "Error in predatoraggregator - predator is not age structured", predators[i]->getName());

    minage = ((StockPredator*)predators[i])->minAge();
    maxage = ((StockPredator*)predators[i])->maxAge();
    predConv.AddRows(1, maxage + 1, -1);
    for (j = minage; j <= maxage; j++)
      for (k = 0; k < predAges.Nrow(); k++)
        for (l = 0; l < predAges.Ncol(k); l++)
          if (j == predAges[k][l])
             predConv[i][j] = k;
  }

  for (i = 0; i < preys.Size(); i++) {
    if (!checkLengthGroupStructure(preys[i]->getLengthGroupDiv(), preyLgrpDiv))
      handle.logMessage(LOGFAIL, "Error in predatoraggregator - invalid prey length group structure");
    preyConv.AddRows(1, preys[i]->getLengthGroupDiv()->numLengthGroups(), -1);
    for (j = 0; j < preyConv.Ncol(i); j++)
      preyConv[i][j] = preyLgrpDiv->numLengthGroup(preys[i]->getLengthGroupDiv()->meanLength(j));
  }

  for (i = 0; i < predators.Size(); i++)
    for (j = 0; j < preys.Size(); j++)
      if (predators[i]->doesEat(preys[j]->getName()))
        doeseat[i][j] = 1;

  for (i = 0; i < areas.Nrow(); i++)
    total.resize(new DoubleMatrix(predAges.Nrow(), preyLgrpDiv->numLengthGroups(), 0.0));
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
  int i;
  for (i = 0; i < total.Size(); i++)
    (*total[i]).setToZero();
}

void PredatorAggregator::Sum() {
  int g, h, i, j, k, l, m;
  double consum;
  DoubleVector agesum, lensum;

  this->Reset();
  //sum over the appropriate preys, predators, areas and lengths
  for (g = 0; g < predators.Size(); g++) {
    for (h = 0; h < preys.Size(); h++) {
      if (doeseat[g][h]) {
        for (l = 0; l < areas.Nrow(); l++) {
          for (j = 0; j < areas.Ncol(l); j++) {
            if (predators[g]->isInArea(areas[l][j]) && preys[h]->isPreyArea(areas[l][j])) {
              dptr = &predators[g]->getConsumption(areas[l][j], preys[h]->getName());

              if (usepredages) {
                //need to convert from length groups to age groups
                alk = &((StockPredator*)predators[g])->getCurrentALK(areas[l][j]);

                //first calculate how many predators there are in each age and length group
                agesum.Reset();
                lensum.Reset();
                agesum.resize(alk->maxAge() + 1, 0.0);
                lensum.resize(predators[g]->getLengthGroupDiv()->numLengthGroups(), 0.0);
                for (k = alk->minAge(); k <= alk->maxAge(); k++) {
                  for (m = alk->minLength(k); m < alk->maxLength(k); m++) {
                    agesum[k] += (*alk)[k][m].N;
                    lensum[m] += (*alk)[k][m].N;
                  }
                }

                //then calculate the total consumption by the predators
                for (k = alk->minAge(); k <= alk->maxAge(); k++) {
                  if (predConv[g][k] >= 0) {
                    for (i = 0; i < dptr->Ncol(k); i++) {
                      if (preyConv[h][i] >= 0) {
                        consum = 0.0;
                        for (m = 0; m < dptr->Nrow(); m++)
                          if (!isZero(lensum[m]))
                            consum += (*dptr)[m][i] * (*alk)[k][m].N / lensum[m];

                        (*total[l])[predConv[g][k]][preyConv[h][i]] += consum;
                      }
                    }
                  }
                }

                //finally convert this total consumption to a per-predator consumption
                for (k = alk->minAge(); k <= alk->maxAge(); k++)
                  if ((predConv[g][k] >= 0) && (!isZero(agesum[k])))
                    for (i = 0; i < (*total[l])[predConv[g][k]].Size(); i++)
                      (*total[l])[predConv[g][k]][i] /= agesum[k];

              } else {
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
}

void PredatorAggregator::NumberSum() {
  if (usepredages)
    handle.logMessage(LOGFAIL, "Error in predatoraggregator - cannot sum numbers for age structured predators");

  int g, h, i, j, k, l;
  const PopInfoVector* preymeanw;

  this->Reset();
  for (g = 0; g < predators.Size(); g++) {
    for (h = 0; h < preys.Size(); h++) {
      if (doeseat[g][h]) {
        for (l = 0; l < areas.Nrow(); l++) {
          for (j = 0; j < areas.Ncol(l); j++) {
            if (predators[g]->isInArea(areas[l][j]) && preys[h]->isPreyArea(areas[l][j])) {
              dptr = &predators[g]->getConsumption(areas[l][j], preys[h]->getName());
              preymeanw = &predators[g]->getConsumptionPopInfo(areas[l][j], preys[h]->getName());
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
