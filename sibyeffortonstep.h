#ifndef sibyeffortonstep_h
#define sibyeffortonstep_h

#include "sionstep.h"
#include "fleeteffortaggregator.h"

/**
 * \class SIByEffortOnStep
 * \brief This is the class used to calculate a likelihood score by fitting the model population to effort based data
 */
class SIByEffortOnStep : public SIOnStep {
public:
  /**
   * \brief This is the SIByEffortOnStep constructor
   * \param infile is the CommentStream to read the survey index data from
   * \param areas is the IntMatrix of areas that the survey index can take place on
   * \param areaindex is the CharPtrVector of area identifier text strings
   * \param fleetindex is the CharPtrVector of fleet identifier text strings
   * \param TimeInfo is the TimeClass for the current model
   * \param datafilename is the name of the file containing the survey index data
   * \param givenname is the name of the SIByEffortOnStep likelihood component
   * \param bio is the flag to denote whether the index should be based on the biomass or not
   */
  SIByEffortOnStep(CommentStream& infile, const IntMatrix& areas,
    const CharPtrVector& areaindex, const CharPtrVector& fleetindex,
    const TimeClass* const TimeInfo, const char* datafilename, const char* givenname, int bio);
  /**
   * \brief This is the default SIByEffortOnStep destructor
   */
  virtual ~SIByEffortOnStep();
  /**
   * \brief This function will sum the survey index by fleet length data
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void Sum(const TimeClass* const TimeInfo);
  /**
   * \brief This will select the fleets and stocks required to calculate the survey index likelihood score
   * \param Fleets is the FleetPtrVector of all the available fleets
   * \param Stocks is the StockPtrVector of all the available stocks
   */
  virtual void setFleetsAndStocks(FleetPtrVector& Fleets, StockPtrVector& Stocks);
  /**
   * \brief This function will print the SIByEffortOnStep information
   * \param outfile is the ofstream that all the model information gets sent to
   */
  virtual void Print(ofstream& outfile) const { aggregator->Print(outfile); };
protected:
  /**
   * \brief This is the FleetEffortAggregator used to collect information about the relevant fleets for the survey index data
   */
  FleetEffortAggregator* aggregator;
  /**
   * \brief This is the DoubleMatrix used to temporarily store the information returned from the aggregatation function
   */
  const DoubleMatrix* dptr;
};

#endif
