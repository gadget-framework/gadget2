#ifndef sibyageonstep_h
#define sibyageonstep_h

#include "sionstep.h"

/**
 * \class SIByAgeOnStep
 * \brief This is the class used to calculate a likelihood score by fitting the model population to age based survey index data
 */
class SIByAgeOnStep : public SIOnStep {
public:
  /**
   * \brief This is the SIByAgeOnStep constructor
   * \param infile is the CommentStream to read the survey index data from
   * \param areas is the IntMatrix of areas that the survey index can take place on
   * \param Ages is the IntMatrix of ages from the survey index data
   * \param areaindex is the CharPtrVector of area identifier text strings
   * \param ageindex is the CharPtrVector of age identifier text strings
   * \param TimeInfo is the TimeClass for the current model
   * \param datafilename is the name of the file containing the survey index data
   * \param name is the name of the SIByAgeOnStep likelihood component
   */
  SIByAgeOnStep(CommentStream& infile, const IntMatrix& areas,
    const IntMatrix& Ages, const CharPtrVector& areaindex, const CharPtrVector& ageindex,
    const TimeClass* const TimeInfo, const char* datafilename, const char* name);
  /**
   * \brief This is the default SIByAgeOnStep destructor
   */
  virtual ~SIByAgeOnStep();
  /**
   * \brief This function will sum the survey index by age data
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void Sum(const TimeClass* const TimeInfo);
  /**
   * \brief This will select the stocks required to calculate the survey index by age likelihood score
   * \param Stocks is the StockPtrVector of all the available stocks
   */
  virtual void setStocks(const StockPtrVector& Stocks);
protected:
  /**
   * \brief This is the StockAggregator used to collect information about the relevant stocks for the survey index data
   */
  StockAggregator* aggregator;
  /**
   * \brief This is the IntMatrix used to store age information
   */
  IntMatrix ages;
};

#endif
