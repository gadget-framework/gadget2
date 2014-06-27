#ifndef fleeteffortaggregator_h
#define fleeteffortaggregator_h

#include "areatime.h"
#include "doublematrixptrvector.h"
#include "fleetptrvector.h"
#include "stockptrvector.h"
#include "predatorptrvector.h"
#include "preyptrvector.h"
#include "intmatrix.h"

/**
 * \class FleetEffortAggregator
 * \brief This is the class used to aggregate fleet catch information
 */
class FleetEffortAggregator {
public:
  /**
   * \brief This is the FleetEffortAggregator constructor
   * \param Fleets is the FleetPtrVector of the fleets that will be aggregated
   * \param Stocks is the StockPtrVector of the stocks that will be aggregated
   * \param Areas is the IntMatrix of areas that the catch can take place on
   */
  FleetEffortAggregator(const FleetPtrVector& Fleets, const StockPtrVector& Stocks, const IntMatrix& Areas);
  /**
   * \brief This is the default FleetEffortAggregator destructor
   */
  ~FleetEffortAggregator();
  /**
   * \brief This function will reset the fleet catch information
   */
  void Reset();
  /**
   * \brief This function will aggregate the fleet catch data
   */
  void Sum();
  /**
   * \brief This will print the aggregator information to the ofstream specified
   * \param outfile is the ofstream to the file that the aggregator information gets sent to
   */
  void Print(ofstream &outfile) const;
  /**
   * \brief This will return the DoubleMatrixPtrVector containing the aggregated catch information
   * \return total, the DoubleMatrixPtrVector of aggregated catch information
   */
  const DoubleMatrixPtrVector& getSum() const { return total; };
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
   * \brief This is the IntMatrix used to store area information
   */
  IntMatrix areas;
  /**
   * \brief This is the IntMatrix used to store predation information
   * \note doescatch[i][j] is the flag to denote whether fleet i catches stock j
   */
  IntMatrix doescatch;
  /**
   * \brief This is the DoubleMatrixPtrVector used to store the aggregated catch
   */
  DoubleMatrixPtrVector total;
};

#endif
