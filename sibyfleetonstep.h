#ifndef sibyfleetonstep_h
#define sibyfleetonstep_h

#include "sionstep.h"
#include "fleetpreyaggregator.h"

/**
 * \class SIByFleetOnStep
 * \brief This is the class used to calculate a likelihood score by fitting the model population to fleet based survey index data
 */
class SIByFleetOnStep : public SIOnStep {
public:
  /**
   * \brief This is the SIByFleetOnStep constructor
   * \param infile is the CommentStream to read the survey index data from
   * \param areas is the IntMatrix of areas that the survey index can take place on
   * \param lengths is the DoubleVector of lengths from the survey index data
   * \param areaindex is the CharPtrVector of area identifier text strings
   * \param lenindex is the CharPtrVector of length identifier text strings
   * \param TimeInfo is the TimeClass for the current model
   * \param datafilename is the name of the file containing the survey index data
   * \param overcons is a flag to determine whether to calculate overconsumption of the stocks
   * \param name is the name of the SIByFleetOnStep likelihood component
   */
  SIByFleetOnStep(CommentStream& infile, const IntMatrix& areas,
    const DoubleVector& lengths, const CharPtrVector& areaindex,
    const CharPtrVector& lenindex, const TimeClass* const TimeInfo,
    const char* datafilename, int overcons, const char* name);
  /**
   * \brief This is the default SIByFleetOnStep destructor
   */
  virtual ~SIByFleetOnStep();
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
protected:
  /**
   * \brief This is the FleetPreyAggregator used to collect information about the relevant fleets for the survey index data
   */
  FleetPreyAggregator* aggregator;
  /**
   * \brief This is the IntMatrix used to store age information
   */
  IntMatrix Ages;
  /**
   * \brief This is the LengthGroupDivision used to store length information
   */
  LengthGroupDivision* LgrpDiv;
  /**
   * \brief This is a flag to denote whether the likelihood calculation should take overconsumption into account or not
   */
  int overconsumption;
};

#endif
