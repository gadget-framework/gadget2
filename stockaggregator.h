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
  /**
   * \brief This is the StockAggregator constructor
   * \param Stocks is the StockPtrVector of the stocks that will be aggregated
   * \param LgrpDiv is the LengthGroupDivision that the stocks will be aggregated to
   * \param Areas is the IntMatrix of areas that the stocks to be aggregated live on
   * \param Ages is the IntMatrix of ages of the stocks to be aggregated
   */
  StockAggregator(const StockPtrVector& Stocks, const LengthGroupDivision* const LgrpDiv,
    const IntMatrix& Areas, const IntMatrix& Ages);
  /**
   * \brief This is the default StockAggregator destructor
   */
  ~StockAggregator();
  /**
   * \brief This function will aggregate the stock data
   */
  void Sum();
  void MeanSum();
  /**
   * \brief This will return the AgeBandMatrixPtrVector containing the aggregated stock information
   * \return total, the AgeBandMatrixPtrVector of aggregated stock information
   */
  const AgeBandMatrixPtrVector& returnSum() const { return total; };
  const AgeBandMatrixPtrVector& returnMeanSum() const { return meanTotal; };
  const IntVector& getMinCol() const { return mincol; };
  const IntVector& getMaxCol() const { return maxcol; };
  int getMinRow() const { return minrow; };
  int getMaxRow() const { return maxrow; };
protected:
  /**
   * \brief This is the StockPtrVector of the stocks that will be aggregated
   */
  StockPtrVector stocks;
  ConversionIndexPtrVector CI;
  /**
   * \brief This is the IntMatrix used to store area information
   */
  IntMatrix areas;
  /**
   * \brief This is the IntMatrix used to store age information
   */
  IntMatrix ages;
  /**
   * \brief This is the AgeBandMatrixPtrVector used to store the aggregated stock information
   */
  AgeBandMatrixPtrVector total;
  AgeBandMatrixPtrVector meanTotal;
  IntVector maxcol;
  IntVector mincol;
  int minrow;
  int maxrow;
};

#endif
