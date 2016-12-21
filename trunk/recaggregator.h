#ifndef recaggregator_h
#define recaggregator_h

#include "areatime.h"
#include "agebandmatrixptrvector.h"
#include "conversionindexptrvector.h"
#include "fleetptrvector.h"
#include "stockptrvector.h"
#include "predatorptrvector.h"
#include "preyptrvector.h"
#include "intmatrix.h"
#include "tags.h"

/**
 * \class RecAggregator
 * \brief This is the class used to aggregate the recaptured tagged stock information
 */
class RecAggregator {
public:
  /**
   * \brief This is the RecAggregator constructor
   * \param Stocks is the StockPtrVector of the stocks that will be recaptured
   * \param Fleets is the FleetPtrVector of the fleets that will be recapturing the tagged fish
   * \param Lgrpdiv is the LengthGroupDivision of the recaptured fish
   * \param Areas is the IntMatrix of areas that the recaptures can take place on
   * \param Ages is the IntMatrix of ages of the recaptures
   * \param tag is the Tags of the tagging experiments
   */
  RecAggregator(const FleetPtrVector& Fleets, const StockPtrVector& Stocks,
    LengthGroupDivision* const Lgrpdiv, const IntMatrix& Areas,
    const IntMatrix& Ages, Tags* tag);
  /**
   * \brief This is the default RecAggregator destructor
   */
  ~RecAggregator();
  /**
   * \brief This function will return the LengthGroupDivision of the aggregator
   * \return LgrpDiv, the LengthGroupDivision of the aggregator
   */
  LengthGroupDivision* getLengthGroupDiv() const { return LgrpDiv; };
  /**
   * \brief This function will reset the recapture data
   */
  void Reset();
  /**
   * \brief This function will aggregate the recapture data
   */
  void Sum();
  /**
   * \brief This will print the aggregator information to the ofstream specified
   * \param outfile is the ofstream to the file that the aggregator information gets sent to
   */
  void Print(ofstream &outfile) const;
  /**
   * \brief This will return the AgeBandMatrixPtrVector containing the aggregated recaptures information
   * \return total, the AgeBandMatrixPtrVector of aggregated recaptures information
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
   * \brief This is the Tags containing details of the tagging experiment that will be aggregated
   */
  Tags* taggingExp;
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
   * \brief This is the IntMatrix used to store predation information
   * \note doescatch[i][j] is the flag to denote whether fleet i catches stock j
   */
  IntMatrix doescatch;
  /**
   * \brief This is the ConversionIndexPtrVector used to convert the length groups of the stocks to be aggregated
   */
  ConversionIndexPtrVector CI;
  /**
   * \brief This is the AgeBandMatrixPtrVector used to store the aggregated recaptures
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
