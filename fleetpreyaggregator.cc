#include "fleetpreyaggregator.h"
#include "errorhandler.h"
#include "stock.h"
#include "stockprey.h"
#include "lengthpredator.h"
#include "fleet.h"
#include "gadget.h"

extern ErrorHandler handle;

FleetPreyAggregator::FleetPreyAggregator(const FleetPtrVector& Fleets,
  const StockPtrVector& Stocks, LengthGroupDivision* const Lgrpdiv,
  const IntMatrix& Areas, const IntMatrix& Ages, int overcons)
  : fleets(Fleets), stocks(Stocks), LgrpDiv(Lgrpdiv), areas(Areas), ages(Ages),
    doescatch(Fleets.Size(), Stocks.Size(), 0), overconsumption(overcons), suitptr(0), alptr(0) {

  int i, j;
  for (i = 0; i < stocks.Size(); i++)
    CI.resize(new ConversionIndex(stocks[i]->getPrey()->getLengthGroupDiv(), LgrpDiv));

  for (i = 0; i < fleets.Size(); i++)
    for (j = 0; j < stocks.Size(); j++)
      if (fleets[i]->getPredator()->doesEat(stocks[j]->getPrey()->getName()))
        doescatch[i][j] = 1;

  //Resize total using dummy variables tmppop and popmatrix.
  PopInfo tmppop;
  tmppop.N = 1.0;
  PopInfoMatrix popmatrix(ages.Nrow(), LgrpDiv->numLengthGroups(), tmppop);
  total.resize(areas.Nrow(), 0, 0, popmatrix);
  this->Reset();
}

FleetPreyAggregator::~FleetPreyAggregator() {
  int i;
  for (i = 0; i < CI.Size(); i++)
    delete CI[i];
}

void FleetPreyAggregator::Print(ofstream& outfile) const {
  int i, j;
  for (i = 0; i < total.Size(); i++) {
    outfile << "\tInternal areas";
    for (j = 0; j < areas.Ncol(i); j++)
      outfile << sep << areas[i][j];
    outfile << endl;
    total[i].printNumbers(outfile);
  }
  outfile.flush();
}

void FleetPreyAggregator::Reset() {
  int i;
  for (i = 0; i < total.Size(); i++)
    total[i].setToZero();
}

int FleetPreyAggregator::checkCatchData() {
  int i, j, k;
  double check = 0.0;

  for (i = 0; i < total.Size(); i++)
    for (j = 0; j < total[i].Nrow(); j++)
      for (k = 0; k < total[i].maxLength(j); k++)
        check += total[i][j][k].N;

  if (isZero(check))
    return 1;
  return 0;
}

void FleetPreyAggregator::Sum() {

  int f, g, h, i, j, k, r, z;
  int predl = 0;  //JMB there is only ever one length group ...
  double scale;

  this->Reset();
  //Sum over the appropriate fleets, stocks, areas, ages and length groups.
  for (f = 0; f < fleets.Size(); f++) {
    LengthPredator* pred = fleets[f]->getPredator();
    for (h = 0; h < stocks.Size(); h++) {
      if (doescatch[f][h]) {
        StockPrey* prey = (StockPrey*)stocks[h]->getPrey();
        for (r = 0; r < areas.Nrow(); r++) {
          for (j = 0; j < areas.Ncol(r); j++) {
            if ((prey->isPreyArea(areas[r][j])) && (pred->isInArea(areas[r][j]))) {
              scale = pred->getPredatorNumber(areas[r][j])[predl].N * pred->Scaler(areas[r][j]);
              if (!(isZero(scale))) {
                for (i = 0; i < pred->numPreys(); i++) {
                  if (strcasecmp(prey->getName(), pred->getPrey(i)->getName()) == 0) {
                    suitptr = &pred->getSuitability(i)[predl];
                    alptr = &prey->getALKPriorToEating(areas[r][j]);
                    for (g = 0; g < ages.Nrow(); g++) {
                      for (k = 0; k < ages.Ncol(g); k++) {
                        if ((alptr->minAge() <= ages[g][k]) && (ages[g][k] <= alptr->maxAge())) {
                          if (overconsumption) {
                            DoubleVector usesuit = *suitptr;
                            DoubleVector ratio = prey->getRatio(r);
                            for (z = 0; z < usesuit.Size(); z++)
                              if (ratio[z] > 1.0)
                                usesuit[z] *= 1.0 / ratio[z];

                            total[r][g].Add((*alptr)[ages[g][k]], *CI[h], usesuit, scale);
                          } else {
                            total[r][g].Add((*alptr)[ages[g][k]], *CI[h], *suitptr, scale);
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
    }
  }
}
