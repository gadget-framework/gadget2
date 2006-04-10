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
  : fleets(Fleets), stocks(Stocks), taggingExp(tag), LgrpDiv(Lgrpdiv), areas(Areas),
    ages(Ages), doescatch(Fleets.Size(), Stocks.Size(), 0), suitptr(0), alptr(0) {

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

void RecAggregator::Sum() {

  int f, g, h, i, j, k, r;
  int predl = 0;  //JMB there is only ever one length group ...
  double ratio;

  this->Reset();
  for (f = 0; f < fleets.Size(); f++) {
    for (h = 0; h < stocks.Size(); h++) {
      if (doescatch[f][h]) {
        for (r = 0; r < areas.Nrow(); r++) {
          for (j = 0; j < areas.Ncol(r); j++) {
            if ((stocks[h]->getPrey()->isPreyArea(areas[r][j])) &&
                (fleets[f]->getPredator()->isInArea(areas[r][j]))) {

              for (i = 0; i < fleets[f]->getPredator()->numPreys(); i++) {
                if (strcasecmp(stocks[h]->getName(), fleets[f]->getPredator()->getPrey(i)->getName()) == 0) {
                  suitptr = &fleets[f]->getPredator()->getSuitability(i)[predl];
                  alptr = &taggingExp->getNumberPriorToEating(areas[r][j], stocks[h]->getName());
                  ratio = fleets[f]->getPredator()->getConsumptionRatio(areas[r][j], i, predl);
                  for (g = 0; g < ages.Nrow(); g++) {
                    for (k = 0; k < ages.Ncol(g); k++) {
                      //JMB removed the overconsumption stuff
                      if ((alptr->minAge() <= ages[g][k]) && (ages[g][k] <= alptr->maxAge()))
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
