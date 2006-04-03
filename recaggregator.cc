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
  for (i = 0; i < stocks.Size(); i++)
    CI.resize(new ConversionIndex(stocks[i]->getPrey()->getLengthGroupDiv(), LgrpDiv));

  //Resize total using dummy variables tmppop and popmatrix.
  PopInfo tmppop;
  tmppop.N = 1.0;
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

void RecAggregator::Reset() {
  int i;
  for (i = 0; i < total.Size(); i++)
    total[i].setToZero();
}

void RecAggregator::Sum(const TimeClass* const TimeInfo) {

  int area, age, i, j, k, f, h;
  double fleetscale;

  this->Reset();
  for (f = 0; f < fleets.Size(); f++) {
    for (h = 0; h < stocks.Size(); h++) {
      for (area = 0; area < areas.Nrow(); area++) {
        for (j = 0; j < areas.Ncol(area); j++) {
          if ((stocks[h]->getPrey()->isPreyArea(areas[area][j])) &&
              (fleets[f]->isFleetStepArea(areas[area][j], TimeInfo))) {

            fleetscale = fleets[f]->getFleetAmount(areas[area][j], TimeInfo)
                          * fleets[f]->getPredator()->Scaler(areas[area][j]);

            for (i = 0; i < fleets[f]->getPredator()->numPreys(); i++) {
              if (strcasecmp(stocks[h]->getName(), fleets[f]->getPredator()->getPrey(i)->getName()) == 0) {
                suitptr = &fleets[f]->getPredator()->getSuitability(i)[0];
                alptr = &taggingExp->getNumberPriorToEating(areas[area][j], stocks[h]->getName());
                for (age = 0; age < ages.Nrow(); age++) {
                  for (k = 0; k < ages.Ncol(age); k++) {
                    //JMB removed the overconsumption stuff
                    if ((alptr->minAge() <= ages[age][k]) && (ages[age][k] <= alptr->maxAge()))
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
