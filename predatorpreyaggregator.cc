#include "predatorpreyaggregator.h"
#include "errorhandler.h"
#include "stock.h"
#include "stockprey.h"
#include "poppredator.h"
#include "mathfunc.h"
#include "gadget.h"
#include "global.h"

PredatorPreyAggregator::PredatorPreyAggregator(const PredatorPtrVector& Predators,
  const PreyPtrVector& Preys, LengthGroupDivision* const Lgrpdiv,
  const IntMatrix& Areas, const IntMatrix& Ages)
  : predators(Predators), preys(Preys), LgrpDiv(Lgrpdiv), areas(Areas), ages(Ages),
    doeseat(Predators.Size(), Preys.Size(), 0), suitptr(0), alptr(0) {

  int i, j;
  for (i = 0; i < predators.Size(); i++)
    for (j = 0; j < preys.Size(); j++)
      if (predators[i]->doesEat(preys[j]->getName()))
        doeseat[i][j] = 1;

  for (i = 0; i < preys.Size(); i++) {
    CI.resize(new ConversionIndex(preys[i]->getLengthGroupDiv(), LgrpDiv));
    if (CI[i]->Error())
      handle.logMessage(LOGFAIL, "Error in predatorpreyaggregator - error when checking length structure");

    //check that the prey is a stock
    if (preys[i]->getType() == LENGTHPREY)
      handle.logMessage(LOGFAIL, "Error in predatorpreyaggregator - cannot aggregate prey", preys[i]->getName());
  }

  //resize objects to store the agggregated information
  for (i = 0; i < areas.Nrow(); i++)
    mortality.resize(new DoubleMatrix(ages.Nrow(), LgrpDiv->numLengthGroups(), 0.0));
  PopInfo tmppop;
  tmppop.N = 1.0;
  PopInfoMatrix popmatrix(ages.Nrow(), LgrpDiv->numLengthGroups(), tmppop);
  total.resize(areas.Nrow(), 0, 0, popmatrix);
  consume.resize(areas.Nrow(), 0, 0, popmatrix);
  this->Reset();
}

PredatorPreyAggregator::~PredatorPreyAggregator() {
  int i;
  for (i = 0; i < CI.Size(); i++)
    delete CI[i];
  for (i = 0; i < mortality.Size(); i++)
    delete mortality[i];
}

void PredatorPreyAggregator::Reset() {
  int i;
  for (i = 0; i < mortality.Size(); i++) {
    total[i].setToZero();
    consume[i].setToZero();
    (*mortality[i]).setToZero();
  }
}

void PredatorPreyAggregator::Sum(const TimeClass* const TimeInfo) {

  int f, g, h, i, j, k, l, m;
  double ratio;

  this->Reset();
  //Sum over the appropriate predators, preys, areas, ages and length groups
  //First calculate the prey population that is consumed by the predation
  for (f = 0; f < predators.Size(); f++) {
    for (g = 0; g < preys.Size(); g++) {
      if (doeseat[f][g]) {
        for (i = 0; i < areas.Nrow(); i++) {
          for (j = 0; j < areas.Ncol(i); j++) {
            if ((preys[g]->isPreyArea(areas[i][j])) && (predators[f]->isInArea(areas[i][j]))) {
              for (k = 0; k < predators[f]->numPreys(); k++) {
                if (strcasecmp(preys[g]->getName(), predators[f]->getPrey(k)->getName()) == 0) {
                  alptr = &((StockPrey*)preys[g])->getConsumptionALK(areas[i][j]);
                  for (h = 0; h < predators[f]->getLengthGroupDiv()->numLengthGroups(); h++) {
                    //suitptr = &predators[f]->getSuitability(k)[h];
                    suitptr = &((PopPredator*)predators[f])->getUseSuitability(areas[i][j], k)[h];
                    ratio = predators[f]->getConsumptionRatio(areas[i][j], k, h);
                    for (l = 0; l < ages.Nrow(); l++)
                      for (m = 0; m < ages.Ncol(l); m++)
                        if ((alptr->minAge() <= ages[l][m]) && (ages[l][m] <= alptr->maxAge()))
                          consume[i][l].Add((*alptr)[ages[l][m]], *CI[g], *suitptr, ratio);
                  }
                }
              }
            }
          }
        }
      }
    }
  }

  //Then calculate the prey population before predation
  for (g = 0; g < preys.Size(); g++) {
    for (i = 0; i < areas.Nrow(); i++) {
      for (j = 0; j < areas.Ncol(i); j++) {
        if (preys[g]->isPreyArea(areas[i][j])) {
          alptr = &((StockPrey*)preys[g])->getConsumptionALK(areas[i][j]);
          for (l = 0; l < ages.Nrow(); l++)
            for (m = 0; m < ages.Ncol(l); m++)
              if ((alptr->minAge() <= ages[l][m]) && (ages[l][m] <= alptr->maxAge()))
                total[i][l].Add((*alptr)[ages[l][m]], *CI[g]);

        }
      }
    }
  }

  //Finally calculate the mortality caused by the predation
  ratio = 1.0 / TimeInfo->getTimeStepSize();
  for (i = 0; i < mortality.Size(); i++)
    for (j = 0; j < (*mortality[i]).Nrow(); j++)
      for (k = 0; k < (*mortality[i]).Ncol(j); k++)
        (*mortality[i])[j][k] = calcMortality(consume[i][j][k].N, total[i][j][k].N, ratio);
}
