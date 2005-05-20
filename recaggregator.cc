#include "recaggregator.h"
#include "areatime.h"
#include "errorhandler.h"
#include "readfunc.h"
#include "stock.h"
#include "stockprey.h"
#include "lengthpredator.h"
#include "fleet.h"
#include "readword.h"
#include "gadget.h"

RecAggregator::RecAggregator(const FleetPtrVector& Fleets,
  const StockPtrVector& Stocks, LengthGroupDivision* const Lgrpdiv,
  const IntMatrix& Areas, const IntMatrix& Ages, Tags* tag)
  : fleets(Fleets), stocks(Stocks), taggingExp(tag), LgrpDiv(Lgrpdiv),
    areas(Areas), ages(Ages), suitptr(0), alptr(0) {

  int i;
  CI.resize(stocks.Size());
  for (i = 0; i < stocks.Size(); i++)
    CI[i] = new ConversionIndex(stocks[i]->getPrey()->getLengthGroupDiv(), LgrpDiv);

  //Resize total using dummy variables tmppop and popmatrix.
  PopInfo tmppop;
  tmppop.N = 1.0;
  tmppop.W = 1.0;
  PopInfoMatrix popmatrix(ages.Nrow(), LgrpDiv->numLengthGroups(), tmppop);
  total.resize(areas.Nrow(), 0, 0, popmatrix);
  this->Reset();
}

RecAggregator::~RecAggregator() {
  int i;
  for (i = 0; i < CI.Size(); i++)
    delete CI[i];
}

void RecAggregator::Print(ofstream& outfile) const {
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

void RecAggregator::Reset() {
  int i, j, k;
  PopInfo nullpop;

  for (i = 0; i < total.Size(); i++)
    for (j = 0; j < total[i].Nrow(); j++)
      for (k = 0; k < total[i].maxLength(j); k++)
        total[i][j][k] = nullpop;
}

void RecAggregator::Sum(const TimeClass* const TimeInfo) {

  int i, j, k, f, h, z;
  int aggrArea, aggrAge, area, age;
  double fleetscale;

  this->Reset();
  for (f = 0; f < fleets.Size(); f++) {
    for (h = 0; h < stocks.Size(); h++) {
      for (aggrArea = 0; aggrArea < areas.Nrow(); aggrArea++) {
        for (j = 0; j < areas.Ncol(aggrArea); j++) {
          area = areas[aggrArea][j];
          if ((stocks[h]->getPrey()->isPreyArea(area)) &&
              (fleets[f]->isFleetStepArea(area, TimeInfo))) {

            fleetscale = fleets[f]->Amount(area, TimeInfo) * fleets[f]->getPredator()->Scaler(area);
            for (i = 0; i < fleets[f]->getPredator()->numPreys(); i++) {
              if (strcasecmp(stocks[h]->getPrey()->getName(), fleets[f]->getPredator()->getPrey(i)->getName()) == 0) {
                suitptr = &fleets[f]->getPredator()->Suitability(i)[0];
                alptr = &taggingExp->getNumberPriorToEating(area, stocks[h]->getName());
                for (aggrAge = 0; aggrAge < ages.Nrow(); aggrAge++) {
                  for (k = 0; k < ages.Ncol(aggrAge); k++) {
                    age = ages[aggrAge][k];
                    //JMB removed the overconsumption stuff
                    if ((alptr->minAge() <= age) && (age <= alptr->maxAge()))
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
