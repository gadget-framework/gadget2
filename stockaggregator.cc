#include "stockaggregator.h"
#include "conversionindex.h"
#include "stock.h"
#include "mathfunc.h"

StockAggregator::StockAggregator(const StockPtrVector& Stocks,
  const LengthGroupDivision* const LgrpDiv, const IntMatrix& Areas, const IntMatrix& Ages)
  : stocks(Stocks), areas(Areas), ages(Ages), alptr(0) {

  int i;
  CI.resize(stocks.Size());
  for (i = 0; i < stocks.Size(); i++)
    CI[i] = new ConversionIndex(stocks[i]->returnLengthGroupDiv(), LgrpDiv);

  //Resize total using dummy variables tmppop and popmatrix.
  PopInfo tmppop;
  tmppop.N = 1.0;
  tmppop.W = 1.0;
  PopInfoMatrix popmatrix(ages.Nrow(), LgrpDiv->numLengthGroups(), tmppop);
  total.resize(areas.Nrow(), 0, 0, popmatrix);
  this->Reset();
}

StockAggregator::~StockAggregator() {
  int i;
  for (i = 0; i < CI.Size(); i++)
    delete CI[i];
}

void StockAggregator::Print(ofstream& outfile) const {
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

void StockAggregator::Reset() {
  int i, j, k;
  PopInfo nullpop;

  for (i = 0; i < total.Size(); i++)
    for (j = 0; j < total[i].Nrow(); j++)
      for (k = 0; k < total[i].maxLength(j); k++)
        total[i][j][k] = nullpop;
}

void StockAggregator::Sum() {
  int i, j, k;
  int aggrArea, aggrAge, area, age;

  this->Reset();
  //Sum over the appropriate stocks, areas, ages and length groups.
  for (i = 0; i < stocks.Size(); i++) {
    for (aggrArea = 0; aggrArea < areas.Nrow(); aggrArea++) {
      for (j = 0; j < areas.Ncol(aggrArea); j++) {
        area = areas[aggrArea][j];
        if (stocks[i]->isInArea(area)) {
          alptr = &stocks[i]->getAgeLengthKeys(area);
          for (aggrAge = 0; aggrAge < ages.Nrow(); aggrAge++) {
            for (k = 0; k < ages.Ncol(aggrAge); k++) {
              age = ages[aggrAge][k];
              if ((alptr->minAge() <= age) && (age <= alptr->maxAge()))
                total[aggrArea][aggrAge].Add((*alptr)[age], *CI[i]);
            }
          }
        }
      }
    }
  }
}
