#ifndef stockaggregator_h
#define stockaggregator_h

#include "agebandmatrix.h"
#include "agebandmatrixptrvector.h"
#include "stockptrvector.h"
#include "conversionindexptrvector.h"
#include "intmatrix.h"

class StockAggregator;

class StockAggregator {
public:
  StockAggregator(const StockPtrVector& Stocks, const LengthGroupDivision* const LgrpDiv,
    const IntMatrix& Areas, const IntMatrix& Ages);
  ~StockAggregator();
  void Sum();
  void MeanSum();
  const AgeBandMatrixPtrVector& ReturnSum() const { return total; };
  const AgeBandMatrixPtrVector& ReturnMeanSum() const { return meanTotal; };
  const IntVector& getMinCol() const { return mincol; };
  const IntVector& getMaxCol() const { return maxcol; };
  int getMinRow() const { return minrow; };
  int getMaxRow() const { return maxrow; };
protected:
  StockPtrVector stocks;
  ConversionIndexPtrVector CI;
  IntMatrix areas;
  IntMatrix ages;
  IntVector AreaNr;
  IntVector AgeNr;
  AgeBandMatrixPtrVector total;
  AgeBandMatrixPtrVector meanTotal;
  IntVector maxcol;
  IntVector mincol;
  int minrow;
  int maxrow;
};

#endif
