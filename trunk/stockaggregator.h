#ifndef stockaggregator_h
#define stockaggregator_h

#include "agebandmatrix.h"
#include "agebandmatrixptrvector.h"
#include "stockptrvector.h"
#include "conversionindexptrvector.h"
#include "intmatrix.h"

/**
 * \class StockAggregator
 * \brief This is the class used to aggregate stock information
 */
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
   * \brief This function will reset the stock data
   */
  void Reset();
  /**
   * \brief This function will aggregate the stock data
   */
  void Sum();
  /**
   * \brief This will return the AgeBandMatrixPtrVector containing the aggregated stock information
   * \return total, the AgeBandMatrixPtrVector of aggregated stock information
   */
  const AgeBandMatrixPtrVector& getSum() const { return total; };
  /**
   * \brief This will print the aggregator information to the ofstream specified
   * \param outfile is the ofstream to the file that the aggregator information gets sent to
   */
  void Print(ofstream &outfile) const;
protected:
  /**
   * \brief This is the StockPtrVector of the stocks that will be aggregated
   */
  StockPtrVector stocks;
  /**
   * \brief This is the ConversionIndexPtrVector used to convert the length groups of the stocks to be aggregated
   */
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
  /**
   * \brief This is the AgeBandMatrix used to temporarily store information when aggregating the stocks
   */
  const AgeBandMatrix* alptr;
};

#endif
