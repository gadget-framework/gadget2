#ifndef stockpreyaggregator_h
#define stockpreyaggregator_h

#include "agebandmatrix.h"
#include "agebandmatrixptrvector.h"
#include "preyptrvector.h"
#include "conversionindexptrvector.h"
#include "intmatrix.h"

/**
 * \class StockPreyAggregator
 * \brief This is the class used to aggregate stock prey information
 */
class StockPreyAggregator {
public:
  /**
   * \brief This is the StockPreyAggregator constructor
   * \param Preys is the PreyPtrVector of the preys that will be aggregated
   * \param LgrpDiv is the LengthGroupDivision that the preys will be aggregated to
   * \param Areas is the IntMatrix of areas that the preys to be aggregated live on
   * \param Ages is the IntMatrix of ages of the preys to be aggregated
   */
  StockPreyAggregator(const PreyPtrVector& Preys, const LengthGroupDivision* const LgrpDiv,
    const IntMatrix& Areas, const IntMatrix& Ages);
  /**
   * \brief This is the default StockPreyAggregator destructor
   */
  ~StockPreyAggregator();
  /**
   * \brief This function will reset the stock prey data
   */
  void Reset();
  /**
   * \brief This function will aggregate the stock prey data
   */
  void Sum();
  /**
   * \brief This will return the AgeBandMatrixPtrVector containing the aggregated stock prey information
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
   * \brief This is the PreyPtrVector of the preys that will be aggregated
   */
  PreyPtrVector preys;
  /**
   * \brief This is the ConversionIndexPtrVector used to convert the length groups of the preys to be aggregated
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
   * \brief This is the AgeBandMatrixPtrVector used to store the aggregated stock prey information
   */
  AgeBandMatrixPtrVector total;
  /**
   * \brief This is the AgeBandMatrix used to temporarily store information when aggregating the preys
   */
  const AgeBandMatrix* alptr;
};

#endif
