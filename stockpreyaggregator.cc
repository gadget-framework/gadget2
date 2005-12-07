#include "stockpreyaggregator.h"
#include "conversionindex.h"
#include "stockprey.h"
#include "mathfunc.h"

StockPreyAggregator::StockPreyAggregator(const PreyPtrVector& Preys,
  const LengthGroupDivision* const LgrpDiv, const IntMatrix& Areas, const IntMatrix& Ages)
  : preys(Preys), areas(Areas), ages(Ages), alptr(0) {

  int i;
  for (i = 0; i < preys.Size(); i++)
    CI.resize(new ConversionIndex(preys[i]->getLengthGroupDiv(), LgrpDiv));

  //Resize total using dummy variables tmppop and popmatrix.
  PopInfo tmppop;
  tmppop.N = 1.0;
  tmppop.W = 1.0;
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
  int i, j, k;

  for (i = 0; i < total.Size(); i++) {
    outfile << "\tInternal areas " << areas[i][0] << endl;
    for (j = 0; j < total[i].Nrow(); j++) {
      outfile << TAB;
      for (k = 0; k < total[i].maxLength(j); k++)
        outfile << setw(smallwidth) << total[i][j][k].N << sep;
      outfile << endl;
    }
  }
  outfile.flush();
}

void StockPreyAggregator::Reset() {
  int i, j, k;
  PopInfo nullpop;

  for (i = 0; i < total.Size(); i++)
    for (j = 0; j < total[i].Nrow(); j++)
      for (k = 0; k < total[i].maxLength(j); k++)
        total[i][j][k] = nullpop;
}

void StockPreyAggregator::Sum() {
  int area, age, i, j, k;
  DoubleVector dv;

  this->Reset();
  //Sum over the appropriate preys, areas, ages and length groups.
  for (i = 0; i < preys.Size(); i++) {
    for (area = 0; area < areas.Nrow(); area++) {
      for (j = 0; j < areas.Ncol(area); j++) {
        if (preys[i]->isPreyArea(areas[area][j])) {
          alptr = &((StockPrey*)preys[i])->getALKPriorToEating(areas[area][j]);
          dv = preys[i]->getRatio(areas[area][j]);
          for (age = 0; age < ages.Nrow(); age++) {
            for (k = 0; k < ages.Ncol(age); k++) {
              if ((alptr->minAge() <= ages[age][k]) && (ages[age][k] <= alptr->maxAge()))
                total[area][age].Add((*alptr)[ages[age][k]], *CI[i], dv);
            }
          }
        }
      }
    }
  }
}
