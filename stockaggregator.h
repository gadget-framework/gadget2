#ifndef stockaggregator_h
#define stockaggregator_h

#include "agebandm.h"
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
  const Agebandmatrixvector& ReturnSum() const;
  const Agebandmatrixvector& ReturnMeanSum() const;
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
  Agebandmatrixvector total;
  Agebandmatrixvector meanTotal;
  int numlengths;
  intvector maxcol;
  intvector mincol;
  int minrow;
  int maxrow;
};

#endif
