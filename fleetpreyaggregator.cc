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
  double ratio;

  this->Reset();
  //Sum over the appropriate fleets, stocks, areas, ages and length groups.
  for (f = 0; f < fleets.Size(); f++) {
    for (h = 0; h < stocks.Size(); h++) {
      if (doescatch[f][h]) {
        for (r = 0; r < areas.Nrow(); r++) {
          for (j = 0; j < areas.Ncol(r); j++) {
            if ((stocks[h]->getPrey()->isPreyArea(areas[r][j])) &&
                (fleets[f]->getPredator()->isInArea(areas[r][j]))) {

              for (i = 0; i < fleets[f]->getPredator()->numPreys(); i++) {
                if (strcasecmp(stocks[h]->getPrey()->getName(), fleets[f]->getPredator()->getPrey(i)->getName()) == 0) {
                  suitptr = &fleets[f]->getPredator()->getSuitability(i)[predl];
                  alptr = &((StockPrey*)stocks[h]->getPrey())->getALKPriorToEating(areas[r][j]);
                  ratio = fleets[f]->getPredator()->getConsumptionRatio(areas[r][j], i, predl);
                  for (g = 0; g < ages.Nrow(); g++) {
                    for (k = 0; k < ages.Ncol(g); k++) {
                      if ((alptr->minAge() <= ages[g][k]) && (ages[g][k] <= alptr->maxAge())) {
                        if (overconsumption) {
                          DoubleVector usesuit = *suitptr;
                          DoubleVector preyratio = stocks[h]->getPrey()->getRatio(r);
                          for (z = 0; z < usesuit.Size(); z++)
                            if (preyratio[z] > 1.0)
                              usesuit[z] *= 1.0 / preyratio[z];

                          total[r][g].Add((*alptr)[ages[g][k]], *CI[h], usesuit, ratio);
                        } else {
                          total[r][g].Add((*alptr)[ages[g][k]], *CI[h], *suitptr, ratio);
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
