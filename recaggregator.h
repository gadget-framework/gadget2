#ifndef recaggregator_h
#define recaggregator_h

#include "areatime.h"
#include "conversionindex.h"
#include "agebandmatrix.h"
#include "agebandmatrixptrvector.h"
#include "charptrvector.h"
#include "fleetptrvector.h"
#include "stockptrvector.h"
#include "intmatrix.h"
#include "conversionindexptrvector.h"
#include "tags.h"

class RecAggregator;

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
   * \brief This function will return the number of length groups of the aggregator
   * \return number of length groups
   */
  int NoLengthGroups() const { return LgrpDiv->NoLengthGroups(); };
  /**
   * \brief This function will return the number of age groups of the aggregator
   * \return number of age groups
   */
  int NoAgeGroups() const { return ages.Nrow(); };
  /**
   * \brief This function will return the LengthGroupDivision of the aggregator
   * \return LgrpDiv, the LengthGroupDivision of the aggregator
   */
  LengthGroupDivision* returnLengthGroupDiv() const { return LgrpDiv; };
  /**
   * \brief This function will aggregate the recapture data
   * \param TimeInfo is the TimeClass for the current model
   */
  void Sum(const TimeClass* const TimeInfo);
  /**
   * \brief This will print the aggregator information to the ofstream specified
   * \param outfile is the ofstream to the file that the aggregator information gets sent to
   */
  void Print(ofstream &outfile) const;
  /**
   * \brief This will return the AgeBandMatrixPtrVector containing the aggregated recaptures information
   * \return total, the AgeBandMatrixPtrVector of aggregated recaptures information
   */
  const AgeBandMatrixPtrVector& returnSum() const { return total; };
private:
  /**
   * \brief This is the FleetPtrVector of the fleets that will be aggregated
   */
  FleetPtrVector fleets;
  /**
   * \brief This is the StockPtrVector of the stocks that will be aggregated
   */
  StockPtrVector stocks;
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
  ConversionIndexPtrVector CI;
  /**
   * \brief This is the AgeBandMatrixPtrVector used to store the aggregated recaptures
   */
  AgeBandMatrixPtrVector total;
};

#endif
