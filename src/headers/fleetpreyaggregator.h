#ifndef fleetpreyaggregator_h
#define fleetpreyaggregator_h

#include "areatime.h"
#include "agebandmatrixptrvector.h"
#include "conversionindexptrvector.h"
#include "lengthgroup.h"
#include "fleetptrvector.h"
#include "stockptrvector.h"
#include "predatorptrvector.h"
#include "preyptrvector.h"
#include "intmatrix.h"

/**
 * \class FleetPreyAggregator
 * \brief This is the class used to aggregate fleet catch information
 */
class FleetPreyAggregator {
public:
  /**
   * \brief This is the FleetPreyAggregator constructor
   * \param Fleets is the FleetPtrVector of the fleets that will be aggregated
   * \param Stocks is the StockPtrVector of the stocks that will be aggregated
   * \param Lgrpdiv is the LengthGroupDivision of the stocks
   * \param Areas is the IntMatrix of areas that the catch can take place on
   * \param Ages is the IntMatrix of ages of the stocks
   * \param overcons is the flag to determine whether to calculate overconsumption of the stocks
   */
  FleetPreyAggregator(const FleetPtrVector& Fleets, const StockPtrVector& Stocks,
    LengthGroupDivision* const Lgrpdiv, const IntMatrix& Areas,
    const IntMatrix& Ages, int overcons);
  /**
   * \brief This is the default FleetPreyAggregator destructor
   */
  ~FleetPreyAggregator();
  /**
   * \brief This function will reset the fleet catch information
   */
  void Reset();
  /**
   * \brief This function will aggregate the fleet catch data
   */
  void Sum();
  /**
   * \brief This function will check the fleet catch data
   * \return 1 if there is no catch data found, 0 otherwise
   */
  int checkCatchData();
  /**
   * \brief This will print the aggregator information to the ofstream specified
   * \param outfile is the ofstream to the file that the aggregator information gets sent to
   */
  void Print(ofstream &outfile) const;
  /**
   * \brief This will return the AgeBandMatrixPtrVector containing the aggregated catch information
   * \return total, the AgeBandMatrixPtrVector of aggregated catch information
   */
  const AgeBandMatrixPtrVector& getSum() const { return total; };
private:
  /**
   * \brief This is the PredatorPtrVector of the fleets that will be aggregated
   */
  PredatorPtrVector predators;
  /**
   * \brief This is the PreyPtrVector of the stocks that will be aggregated
   */
  PreyPtrVector preys;
  /**
   * \brief This is the LengthGroupDivision of the aggregated stocks
   */
  LengthGroupDivision* LgrpDiv;
  /**
   * \brief This is the IntMatrix used to store area information
   */
  IntMatrix areas;
  /**
   * \brief This is the IntMatrix used to store age information
   */
  IntMatrix ages;
  /**
   * \brief This is the flag used to denote whether to calculate overconsumption of the stocks or not
   */
  int overconsumption;
  /**
   * \brief This is the IntMatrix used to store predation information
   * \note doescatch[i][j] is the flag to denote whether fleet i catches stock j
   */
  IntMatrix doescatch;
  /**
   * \brief This is the ConversionIndexPtrVector used to convert the length groups of the stocks to be aggregated
   */
  ConversionIndexPtrVector CI;
  /**
   * \brief This is the AgeBandMatrixPtrVector used to store the aggregated catch
   */
  AgeBandMatrixPtrVector total;
  /**
   * \brief This is the DoubleVector used to temporarily store suitability information when aggregating the catch
   */
  const DoubleVector* suitptr;
  /**
   * \brief This is the AgeBandMatrix used to temporarily store information when aggregating the catch
   */
  const AgeBandMatrix* alptr;
};

#endif
