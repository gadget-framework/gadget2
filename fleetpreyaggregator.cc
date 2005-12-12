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
  : fleets(Fleets), stocks(Stocks), LgrpDiv(Lgrpdiv), areas(Areas),
    ages(Ages), overconsumption(overcons), suitptr(0), alptr(0) {

  int i;
  for (i = 0; i < stocks.Size(); i++)
    CI.resize(new ConversionIndex(stocks[i]->getPrey()->getLengthGroupDiv(), LgrpDiv));

  //Resize total using dummy variables tmppop and popmatrix.
  PopInfo tmppop;
  tmppop.N = 1.0;
  tmppop.W = 1.0;
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
  int i, j, k;

  for (i = 0; i < total.Size(); i++) {
    outfile << "\tInternal areas " << i << endl;
    for (j = 0; j < total[i].Nrow(); j++) {
      outfile << TAB;
      for (k = 0; k < total[i].maxLength(j); k++)
        outfile << setw(smallwidth) << total[i][j][k].N << sep;
      outfile << endl;
    }
  }
  outfile.flush();
}

void FleetPreyAggregator::Reset() {
  int i, j, k;
  PopInfo nullpop;

  for (i = 0; i < total.Size(); i++)
    for (j = 0; j < total[i].Nrow(); j++)
      for (k = 0; k < total[i].maxLength(j); k++)
        total[i][j][k] = nullpop;
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

void FleetPreyAggregator::Sum(const TimeClass* const TimeInfo) {

  int area, age, i, j, k, f, h, z;
  double fleetscale;

  this->Reset();
  //Sum over the appropriate fleets, stocks, areas, ages and length groups.
  for (f = 0; f < fleets.Size(); f++) {
    LengthPredator* pred = fleets[f]->getPredator();
    for (h = 0; h < stocks.Size(); h++) {
      StockPrey* prey = (StockPrey*)stocks[h]->getPrey();
      for (area = 0; area < areas.Nrow(); area++) {
        for (j = 0; j < areas.Ncol(area); j++) {
          if ((prey->isPreyArea(areas[area][j]))
              && (fleets[f]->isFleetStepArea(areas[area][j], TimeInfo))) {

            fleetscale = fleets[f]->getFleetAmount(areas[area][j], TimeInfo)
                          * pred->Scaler(areas[area][j]);

            for (i = 0; i < pred->numPreys(); i++) {
              if (strcasecmp(prey->getName(), pred->getPrey(i)->getName()) == 0) {
                suitptr = &pred->getSuitability(i)[0];
                alptr = &prey->getALKPriorToEating(areas[area][j]);
                for (age = 0; age < ages.Nrow(); age++) {
                  for (k = 0; k < ages.Ncol(age); k++) {
                    if ((alptr->minAge() <= ages[age][k]) && (ages[age][k] <= alptr->maxAge())) {
                      if (overconsumption) {
                        DoubleVector usesuit = *suitptr;
                        DoubleVector ratio = prey->getRatio(area);
                        for (z = 0; z < usesuit.Size(); z++)
                          if (ratio[z] > 1.0)
                            usesuit[z] *= 1.0 / ratio[z];

                        total[area][age].Add((*alptr)[ages[age][k]], *CI[h], usesuit, fleetscale);
                      } else {
                        total[area][age].Add((*alptr)[ages[age][k]], *CI[h], *suitptr, fleetscale);
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
