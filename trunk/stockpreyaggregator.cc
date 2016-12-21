#include "stockpreyaggregator.h"
#include "conversionindex.h"
#include "stockprey.h"
#include "mathfunc.h"
#include "errorhandler.h"
#include "gadget.h"
#include "global.h"

StockPreyAggregator::StockPreyAggregator(const PreyPtrVector& Preys,
  const LengthGroupDivision* const LgrpDiv, const IntMatrix& Areas, const IntMatrix& Ages)
  : preys(Preys), areas(Areas), ages(Ages), alptr(0) {

  int i;
  for (i = 0; i < preys.Size(); i++) {
    CI.resize(new ConversionIndex(preys[i]->getLengthGroupDiv(), LgrpDiv));
    if (CI[i]->Error())
      handle.logMessage(LOGFAIL, "Error in stockpreyaggregator - error when checking length structure");

    //check that the prey is a stock
    if (preys[i]->getType() == LENGTHPREY)
      handle.logMessage(LOGFAIL, "Error in stockpreyaggregator - cannot aggregate prey", preys[i]->getName());
  }

  //Resize total using dummy variables tmppop and popmatrix.
  PopInfo tmppop;
  tmppop.N = 1.0;
  PopInfoMatrix popmatrix(ages.Nrow(), LgrpDiv->numLengthGroups(), tmppop);
  total.resize(areas.Nrow(), 0, 0, popmatrix);
  this->Reset();
}

StockPreyAggregator::~StockPreyAggregator() {
  int i;
  for (i = 0; i < CI.Size(); i++)
    delete CI[i];
}

void StockPreyAggregator::Print(ofstream& outfile) const {
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

void StockPreyAggregator::Reset() {
  int i;
  for (i = 0; i < total.Size(); i++)
    total[i].setToZero();
}

void StockPreyAggregator::Sum() {
  int area, age, i, j, k;

  this->Reset();
  //Sum over the appropriate preys, areas, ages and length groups.
  for (i = 0; i < preys.Size(); i++) {
    for (area = 0; area < areas.Nrow(); area++) {
      for (j = 0; j < areas.Ncol(area); j++) {
        if (preys[i]->isPreyArea(areas[area][j])) {
          alptr = &((StockPrey*)preys[i])->getConsumptionALK(areas[area][j]);
          for (age = 0; age < ages.Nrow(); age++) {
            for (k = 0; k < ages.Ncol(age); k++) {
              if ((alptr->minAge() <= ages[age][k]) && (ages[age][k] <= alptr->maxAge()))
                total[area][age].Add((*alptr)[ages[age][k]], *CI[i], preys[i]->getUseRatio(areas[area][j]));
            }
          }
        }
      }
    }
  }
}
