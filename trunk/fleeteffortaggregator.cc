#include "fleeteffortaggregator.h"
#include "errorhandler.h"
#include "stock.h"
#include "stockprey.h"
#include "poppredator.h"
#include "fleet.h"
#include "gadget.h"
#include "global.h"

FleetEffortAggregator::FleetEffortAggregator(const FleetPtrVector& Fleets,
  const StockPtrVector& Stocks, const IntMatrix& Areas)
  : areas(Areas), doescatch(Fleets.Size(), Stocks.Size(), 0) {

  int i, j;
  //JMB its simpler to just store pointers to the predators
  //and preys rather than pointers to the fleets and stocks
  for (i = 0; i < Stocks.Size(); i++)
    preys.resize(Stocks[i]->getPrey());
  for (i = 0; i < Fleets.Size(); i++)
    predators.resize(Fleets[i]->getPredator());

  for (i = 0; i < predators.Size(); i++)
    for (j = 0; j < preys.Size(); j++)
      if (predators[i]->doesEat(preys[j]->getName()))
        doescatch[i][j] = 1;

  for (i = 0; i < areas.Nrow(); i++)
    total.resize(new DoubleMatrix(predators.Size(), preys.Size(), 0.0));
}

FleetEffortAggregator::~FleetEffortAggregator() {
  int i;
  for (i = 0; i < total.Size(); i++)
    delete total[i];
}

void FleetEffortAggregator::Print(ofstream& outfile) const {
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

void FleetEffortAggregator::Reset() {
  int i;
  for (i = 0; i < total.Size(); i++)
    (*total[i]).setToZero();
}

void FleetEffortAggregator::Sum() {

  int f, g, h, i, j;
  int predl = 0;  //JMB there is only ever one length group ...

  this->Reset();
  //Sum over the appropriate predators, preys and areas
  for (f = 0; f < predators.Size(); f++)
    for (h = 0; h < preys.Size(); h++)
      if (doescatch[f][h])
        for (i = 0; i < areas.Nrow(); i++)
          for (j = 0; j < areas.Ncol(i); j++)
            if ((preys[h]->isPreyArea(areas[i][j])) && (predators[f]->isInArea(areas[i][j])))
              for (g = 0; g < predators[f]->numPreys(); g++)
                if (strcasecmp(preys[h]->getName(), predators[f]->getPrey(g)->getName()) == 0)
                  (*total[i])[f][h] += predators[f]->getConsumptionRatio(areas[i][j], g, predl);
}
