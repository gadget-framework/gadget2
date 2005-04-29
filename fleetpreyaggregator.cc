#include "fleetpreyaggregator.h"
#include "areatime.h"
#include "errorhandler.h"
#include "readfunc.h"
#include "stock.h"
#include "stockprey.h"
#include "lengthpredator.h"
#include "fleet.h"
#include "readword.h"
#include "gadget.h"

extern ErrorHandler handle;

FleetPreyAggregator::FleetPreyAggregator(const FleetPtrVector& Fleets,
  const StockPtrVector& Stocks, LengthGroupDivision* const Lgrpdiv,
  const IntMatrix& Areas, const IntMatrix& Ages, int overcons)
  : fleets(Fleets), stocks(Stocks), LgrpDiv(Lgrpdiv), areas(Areas),
    ages(Ages), overconsumption(overcons), suitptr(0), alptr(0) {

  int i;
  CI.resize(stocks.Size());
  for (i = 0; i < stocks.Size(); i++)
    CI[i] = new ConversionIndex(stocks[i]->returnPrey()->returnLengthGroupDiv(), LgrpDiv);

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

  int i, j, k, f, h, z;
  int aggrArea, aggrAge, area, age;
  double fleetscale;

  this->Reset();
  //Sum over the appropriate fleets, stocks, areas, ages and length groups.
  for (f = 0; f < fleets.Size(); f++) {
    LengthPredator* pred = fleets[f]->returnPredator();
    for (h = 0; h < stocks.Size(); h++) {
      StockPrey* prey = (StockPrey*)stocks[h]->returnPrey();
      for (aggrArea = 0; aggrArea < areas.Nrow(); aggrArea++) {
        for (j = 0; j < areas.Ncol(aggrArea); j++) {
          area = areas[aggrArea][j];
          if ((prey->isPreyArea(area)) && (fleets[f]->isFleetStepArea(area, TimeInfo))) {

            fleetscale = fleets[f]->Amount(area, TimeInfo) * pred->Scaler(area);
            if (fleets[f]->Type() == LINEARFLEET)
              fleetscale *= TimeInfo->LengthOfCurrent() / TimeInfo->LengthOfYear();

            for (i = 0; i < pred->numPreys(); i++) {
              if (strcasecmp(prey->getName(), pred->Preys(i)->getName()) == 0) {
                suitptr = &pred->Suitability(i)[0];
                alptr = &prey->AlkeysPriorToEating(area);
                for (aggrAge = 0; aggrAge < ages.Nrow(); aggrAge++) {
                  for (k = 0; k < ages.Ncol(aggrAge); k++) {
                    age = ages[aggrAge][k];
                    if ((alptr->minAge() <= age) && (age <= alptr->maxAge())) {
                      if (overconsumption) {
                        DoubleIndexVector Ratio = *suitptr;
                        for (z = Ratio.minCol(); z < Ratio.maxCol(); z++)
                          if (prey->Ratio(area, z) > 1)
                            Ratio[z] *= 1.0 / prey->Ratio(area, z);

                        total[aggrArea][aggrAge].Add((*alptr)[age], *CI[h], fleetscale, Ratio);
                      } else {
                        total[aggrArea][aggrAge].Add((*alptr)[age], *CI[h], fleetscale, *suitptr);
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
