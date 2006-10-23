#ifndef recstatistics_h
#define recstatistics_h

#include "likelihood.h"
#include "commentstream.h"
#include "recaggregator.h"
#include "doublematrixptrvector.h"
#include "tagptrvector.h"
#include "popstatistics.h"

/**
 * \class RecStatistics
 * \brief This is the class used to calculate a likelihood score based on statistical data sampled from the tagging experiments
 *
 * This class calculates a likelihood score based on the difference between statistical data sampled from tagging experiments within the model and that caught by fleets, according to the landings data.  This is typically used to compare biological data, such as mean length at age  This class is an extension of the CatchStatistics class for tagged populations.  The model will calculate the mean length of the tagged stock that is caught according to the model parameters, and aggregate this into specified age groups.  This statistical data is then compared to the corresponding data calculated from the tagging experiment data.
 */
class RecStatistics : public Likelihood {
public:
  /**
   * \brief This is the RecStatistics constructor
   * \param infile is the CommentStream to read the RecStatistics data from
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   * \param weight is the weight for the likelihood component
   * \param Tag is the TagPtrVector of tagging experiments for the current model
   * \param name is the name for the likelihood component
   */
  RecStatistics(CommentStream& infile, const AreaClass* const Area,
    const TimeClass* const TimeInfo, double weight, TagPtrVector Tag, const char* name);
  /**
   * \brief This is the default RecStatistics destructor
   */
  virtual ~RecStatistics();
  /**
   * \brief This function will calculate the likelihood score for the RecStatistics component
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void addLikelihood(const TimeClass* const TimeInfo);
  /**
   * \brief This function will reset the RecStatistics likelihood information
   * \param keeper is the Keeper for the current model
   */
  virtual void Reset(const Keeper* const keeper);
  /**
   * \brief This function will print the summary RecStatistics likelihood information
   * \param outfile is the ofstream that all the model information gets sent to
   */
  virtual void Print(ofstream& outfile) const;
  /**
   * \brief This will select the fleets and stocks required to calculate the RecStatistics likelihood score
   * \param Fleets is the FleetPtrVector of all the available fleets
   * \param Stocks is the StockPtrVector of all the available stocks
   */
  void setFleetsAndStocks(FleetPtrVector& Fleets, StockPtrVector& Stocks);
  /**
   * \brief This function will print summary information from the RecStatistics likelihood calculation
   * \param outfile is the ofstream that all the model likelihood information gets sent to
   */
  virtual void printSummary(ofstream& outfile);
  /**
   * \brief This function will print information from each Recaptures calculation
   * \param outfile is the ofstream that all the model likelihood information gets sent to
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void printLikelihood(ofstream& outfile, const TimeClass* const TimeInfo);
private:
  /**
   * \brief This function will read the RecStatistics data from the input file
   * \param infile is the CommentStream to read the RecStatistics data from
   * \param TimeInfo is the TimeClass for the current model
   * \param numarea is the number of areas that the likelihood data covers
   * \param Tag is the TagPtrVector of the available tagging experiments for the current model
   */
  void readStatisticsData(CommentStream& infile, const TimeClass* TimeInfo,
    int numarea, TagPtrVector Tag);
  /**
   * \brief This function will calculate the likelihood score for the current timestep based on a sum of squares function
   * \return likelihood score
   */
  double calcLikSumSquares();
  /**
   * \brief This is the DoubleMatrixPtrVector used to store number information specified in the input file
   * \note The indices for this object are [tag][time][area]
   */
  DoubleMatrixPtrVector numbers;
  /**
   * \brief This is the DoubleMatrixPtrVector used to store mean length information specified in the input file
   * \note The indices for this object are [tag][time][area]
   */
  DoubleMatrixPtrVector obsMean;
  /**
   * \brief This is the DoubleMatrixPtrVector used to store mean length information calculated in the model
   * \note The indices for this object are [tag][time][area]
   */
  DoubleMatrixPtrVector modelMean;
  /**
   * \brief This is the DoubleMatrixPtrVector used to store the standard deviation of the length information specified in the input file
   * \note The indices for this object are [tag][time][area]
   */
  DoubleMatrixPtrVector obsStdDev;
  /**
   * \brief This is the DoubleMatrixPtrVector used to store the standard deviation of the length information calculated in the model
   * \note The indices for this object are [tag][time][area]
   */
  DoubleMatrixPtrVector modelStdDev;
  /**
   * \brief This is the RecAggregator used to collect information about the racptured tagged fish
   */
  RecAggregator** aggregator;
  /**
   * \brief This is the LengthGroupDivision used to store length information
   */
  LengthGroupDivision* LgrpDiv;
  /**
   * \brief This is the CharPtrVector of the names of the fleets that will be used to calculate the likelihood score
   */
  CharPtrVector fleetnames;
  /**
   * \brief This is the CharPtrVector of the names of the tagging experiments that will be used to calculate the likelihood score
   */
  CharPtrVector tagnames;
  /**
   * \brief This is the IntMatrix used to store area information
   */
  IntMatrix areas;
  /**
   * \brief This is the CharPtrVector of the names of the areas
   */
  CharPtrVector areaindex;
  /**
   * \brief This is the identifier of the function to be used to calculate the likelihood component
   */
  int functionnumber;
  /**
   * \brief This is the IntVector of indices of the timesteps for the likelihood component data
   */
  IntVector timeindex;
  /**
   * \brief This is the IntMatrix used to store information about the years when the likelihood score should be calculated
   */
  IntMatrix Years;
  /**
   * \brief This is the IntMatrix used to store information about the steps when the likelihood score should be calculated
   */
  IntMatrix Steps;
  /**
   * \brief This is the name of the function to be used to calculate the likelihood component
   */
  char* functionname;
  /**
   * \brief This is the TagPtrVector of the relevant tagging experiments
   */
  TagPtrVector tagvec;
  /**
   * \brief This is the PopStatistics used to calculate the statistics of the population
   */
  PopStatistics ps;
  /**
   * \brief This is the AgeBandMatrixPtrVector used to temporarily store the information returned from the aggregatation function
   */
  const AgeBandMatrixPtrVector* alptr;
};

#endif
