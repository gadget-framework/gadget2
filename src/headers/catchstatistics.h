#ifndef catchstatistics_h
#define catchstatistics_h

#include "likelihood.h"
#include "commentstream.h"
#include "charptrvector.h"
#include "fleetpreyaggregator.h"
#include "doublematrixptrvector.h"
#include "popstatistics.h"
#include "actionattimes.h"

/**
 * \class CatchStatistics
 * \brief This is the class used to calculate a likelihood score based on statistical data sampled from the stocks caught by fleets
 *
 * This class calculates a likelihood score based on the difference between statistical data sampled from stocks caught according to the model and that caught by fleets, according to the landings data.  This is typically used to compare biological data, such as mean length at age or mean weight at age.  The model will calculate the mean length (or weight) of the stock that is caught according to the model parameters, and aggregate this into specified age groups.  This statistical data is then compared to the corresponding data calculated from a statistical sample of the landings data.
 */
class CatchStatistics : public Likelihood {
public:
  /**
   * \brief This is the CatchStatistics constructor
   * \param infile is the CommentStream to read the CatchStatistics data from
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   * \param weight is the weight for the likelihood component
   * \param name is the name for the likelihood component
   */
  CatchStatistics(CommentStream& infile, const AreaClass* const Area,
    const TimeClass* const TimeInfo, double weight, const char* name);
  /**
   * \brief This is the default CatchStatistics destructor
   */
  virtual ~CatchStatistics();
  /**
   * \brief This function will calculate the likelihood score for the CatchStatistics component
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void addLikelihood(const TimeClass* const TimeInfo);
  /**
   * \brief This function will reset the CatchStatistics likelihood information
   * \param keeper is the Keeper for the current model
   */
  virtual void Reset(const Keeper* const keeper);
  /**
   * \brief This function will print the summary CatchStatistics likelihood information
   * \param outfile is the ofstream that all the model information gets sent to
   */
  virtual void Print(ofstream& outfile) const;
  /**
   * \brief This will select the fleets and stocks required to calculate the CatchStatistics likelihood score
   * \param Fleets is the FleetPtrVector of all the available fleets
   * \param Stocks is the StockPtrVector of all the available stocks
   */
  void setFleetsAndStocks(FleetPtrVector& Fleets, StockPtrVector& Stocks);
  /**
   * \brief This function will print information from each CatchStatistics calculation
   * \param outfile is the ofstream that all the model likelihood information gets sent to
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void printLikelihood(ofstream& outfile, const TimeClass* const TimeInfo);
  /**
   * \brief This function will print summary information from each CatchStatistics likelihood calculation
   * \param outfile is the ofstream that all the model likelihood information gets sent to
   */
  virtual void printSummary(ofstream& outfile);
private:
  /**
   * \brief This function will read the CatchStatistics data from the input file
   * \param infile is the CommentStream to read the CatchStatistics data from
   * \param TimeInfo is the TimeClass for the current model
   * \param numarea is the number of areas that the likelihood data covers
   * \param numage is the number of age groups that the likelihood data covers
   * \param numlen is the number of length groups that the likelihood data covers
   */
  void readStatisticsData(CommentStream& infile, const TimeClass* TimeInfo,
			  int numarea, int numage, int numlen);
  /**
   * \brief This function will calculate the likelihood score for the current timestep based on a sum of squares function
   * \return likelihood score
   */
  double calcLikSumSquares();
  /**
   * \brief This is the DoubleMatrixPtrVector used to store number information specified in the input file
   * \note The indices for this object are [time][area][age]
   */
  DoubleMatrixPtrVector numbers;
  /**
   * \brief This is the DoubleMatrixPtrVector used to store the mean length/weight information specified in the input file
   * \note The indices for this object are [time][area][age]
   */
  DoubleMatrixPtrVector obsMean;
  /**
   * \brief This is the DoubleMatrixPtrVector used to store the mean length/weight information calculated in the model
   * \note The indices for this object are [time][area][age]
   */
  DoubleMatrixPtrVector modelMean;
  /**
   * \brief This is the DoubleMatrixPtrVector used to store the standard deviation of the length/weight information specified in the input file
   * \note The indices for this object are [time][area][age]
   */
  DoubleMatrixPtrVector obsStdDev;
  /**
   * \brief This is the DoubleMatrixPtrVector used to store the standard deviation of the length/weight information calculated in the model
   * \note The indices for this object are [time][area][age]
   */
  DoubleMatrixPtrVector modelStdDev;
  /**
   * \brief This is the DoubleMatrix used to store the calculated likelihood information
   * \note The indices for this object are [time][area]
   */
  DoubleMatrix likelihoodValues;
  /**
   * \brief This is the FleetPreyAggregator used to collect information about the fleets
   */
  FleetPreyAggregator* aggregator;
  /**
   * \brief This is the LengthGroupDivision used to store length information about all the stocks that will be used to calculate the likelihood score
   */
  LengthGroupDivision* LgrpDiv;
  /**
   * \brief This is the CharPtrVector of the names of the fleets that will be used to calculate the likelihood score
   */
  CharPtrVector fleetnames;
  /**
   * \brief This is the CharPtrVector of the names of the stocks that will be used to calculate the likelihood score
   */
  CharPtrVector stocknames;
  /**
   * \brief This is the IntMatrix used to store area information
   */
  IntMatrix areas;
  /**
   * \brief This is the IntMatrix used to store age information
   */
  IntMatrix ages;
  /**
   * \brief This is the CharPtrVector of the names of the areas
   */
  IntMatrix lengths;
  /**
   * \brief This is the CharPtrVector of the names of the length
   */
  CharPtrVector areaindex;
  /**
   * \brief This is the CharPtrVector of the names of the age groups
   */
  CharPtrVector ageindex;
  /**
   * \brief This is the flag to denote whether the likelihood calculation should take overconsumption into account or not
   */
  CharPtrVector lenindex;
  /**
   * \brief the names of the length groups
   */
  int overconsumption;
  /**
   * \brief This is the index of the timesteps for the likelihood component data
   */
  int timeindex;
  /**
   * \brief This is the identifier of the function to be used to calculate the likelihood component
   */
  int functionnumber;
  /**
   * \brief This ActionAtTimes stores information about when the likelihood score should be calculated
   */
  ActionAtTimes AAT;
  /**
   * \brief This is the IntVector used to store information about the years when the likelihood score should be calculated
   */
  IntVector Years;
  /**
   * \brief This is the IntVector used to store information about the steps when the likelihood score should be calculated
   */
  IntVector Steps;
  /**
   * \brief This is the name of the function to be used to calculate the likelihood component
   */
  char* functionname;
  /**
   * \brief This is the PopStatistics used to calculate the statistics of the population
   */
  PopStatistics ps;
  /**
   * \brief This is the AgeBandMatrixPtrVector used to temporarily store the information returned from the aggregatation function
   * \note The indices for this object are [area][age][length]
   */
  const AgeBandMatrixPtrVector* alptr;
};

#endif
