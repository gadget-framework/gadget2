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
  StockAggregator(const Stockptrvector& Stocks, const LengthGroupDivision* const LgrpDiv,
    const intmatrix& Areas, const intmatrix& Ages);
  ~StockAggregator();
  void Sum();
  void MeanSum();
  const agebandmatrixptrvector& ReturnSum() const;
  const agebandmatrixptrvector& ReturnMeanSum() const;
  const intvector& getMinCol() const { return mincol; };
  const intvector& getMaxCol() const { return maxcol; };
  int getMinRow() const { return minrow; };
  int getMaxRow() const { return maxrow; };
protected:
  Stockptrvector stocks;
  ConversionIndexptrvector CI;
  intmatrix areas;
  intmatrix ages;
  intvector AreaNr;
  intvector AgeNr;
  agebandmatrixptrvector total;
  agebandmatrixptrvector meanTotal;
  int numlengths;
  intvector maxcol;
  intvector mincol;
  int minrow;
  int maxrow;
};

#endif
