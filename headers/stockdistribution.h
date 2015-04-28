#ifndef stockdistribution_h
#define stockdistribution_h

#include "likelihood.h"
#include "commentstream.h"
#include "charptrvector.h"
#include "fleetpreyaggregator.h"
#include "doublematrixptrmatrix.h"
#include "multinomial.h"
#include "actionattimes.h"

/**
 * \class StockDistribution
 * \brief This is the class used to calculate a likelihood score based on distribution data for different stocks sampled from the stocks caught by fleets
 *
 * This class calculates a likelihood score based on the difference between distribution data sampled from different stocks caught according to the model and that caught by fleets, according to the landings data.  This is typically used to compare Gadget stocks that are based on the same species, but have differing biological properties (eg. immature and mature fish).  The distribution data can either be aggregated into age groups (giving a distribution of length groups for each age), length groups (giving a distribution of age groups for each length) or into age-length groups.  The model will calculate the distribution data for the stocks that are caught according to the model parameters, and aggregate this into the specified age and/or length groups.  This distribution data is then compared to the corresponding data calculated from the landings data.
 */
class StockDistribution : public Likelihood {
public:
  /**
   * \brief This is the StockDistribution constructor
   * \param infile is the CommentStream to read the StockDistribution data from
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   * \param weight is the weight for the likelihood component
   * \param name is the name for the likelihood component
   */
  StockDistribution(CommentStream& infile, const AreaClass* const Area,
    const TimeClass* const TimeInfo, double weight, const char* name);
  /**
   * \brief This is the default StockDistribution destructor
   */
  virtual ~StockDistribution();
  /**
   * \brief This function will calculate the likelihood score for the StockDistribution component
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void addLikelihood(const TimeClass* const TimeInfo);
  /**
   * \brief This function will reset the StockDistribution likelihood information
   * \param keeper is the Keeper for the current model
   */
  virtual void Reset(const Keeper* const keeper);
  /**
   * \brief This function will print the summary StockDistribution likelihood information
   * \param outfile is the ofstream that all the model information gets sent to
   */
  virtual void Print(ofstream& outfile) const;
  /**
   * \brief This function will print summary information from each StockDistribution likelihood calculation
   * \param outfile is the ofstream that all the model likelihood information gets sent to
   */
  virtual void printSummary(ofstream& outfile);
  /**
   * \brief This function will print information from each StockDistribution calculation
   * \param outfile is the ofstream that all the model likelihood information gets sent to
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void printLikelihood(ofstream& outfile, const TimeClass* const TimeInfo);
  /**
   * \brief This will select the fleets and stocks required to calculate the StockDistribution likelihood score
   * \param Fleets is the FleetPtrVector of all the available fleets
   * \param Stocks is the StockPtrVector of all the available stocks
   */
  void setFleetsAndStocks(FleetPtrVector& Fleets, StockPtrVector& Stocks);
private:
  /**
   * \brief This function will read the StockDistribution data from the input file
   * \param infile is the CommentStream to read the StockDistribution data from
   * \param TimeInfo is the TimeClass for the current model
   * \param numarea is the number of areas that the likelihood data covers
   * \param numage is the number of age groups that the likelihood data covers
   * \param numlen is the number of length groups that the likelihood data covers
   */
  void readStockData(CommentStream& infile, const TimeClass* TimeInfo,
    int numarea, int numage, int numlen);
  /**
   * \brief This function will calculate the likelihood score for the current timestep based on a multinomial function
   * \return likelihood score
   */
  double calcLikMultinomial();
  /**
   * \brief This function will calculate the likelihood score for the current timestep based on a sum of squares function
   * \param TimeInfo is the TimeClass for the current model
   * \return likelihood score
   */
  double calcLikSumSquares(const TimeClass* const TimeInfo);
  /**
   * \brief This is the DoubleMatrixPtrMatrix used to store age-length distribution information specified in the input file
   * \note The indices for this object are [time][area][stock][id] where id = age+(numage*length)
   */
  DoubleMatrixPtrMatrix obsDistribution;
  /**
   * \brief This is the DoubleMatrixPtrMatrix used to store age-length distribution information calculated in the model
   * \note The indices for this object are [time][area][stock][id] where id = age+(numage*length)
   */
  DoubleMatrixPtrMatrix modelDistribution;
  /**
   * \brief This is the DoubleMatrix used to store the calculated likelihood information
   * \note The indices for this object are [time][area]
   */
  DoubleMatrix likelihoodValues;
  /**
   * \brief This is the FleetPreyAggregator used to collect information about the fleets
   */
  FleetPreyAggregator** aggregator;
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
   * \brief This is the IntVector used to store information about the years when the likelihood score should be calculated
   */
  IntVector Years;
  /**
   * \brief This is the IntVector used to store information about the steps when the likelihood score should be calculated
   */
  IntVector Steps;
  /**
   * \brief This is the CharPtrVector of the names of the areas
   */
  CharPtrVector areaindex;
  /**
   * \brief This is the CharPtrVector of the names of the age groups
   */
  CharPtrVector ageindex;
  /**
   * \brief This is the CharPtrVector of the names of the length groups
   */
  CharPtrVector lenindex;
  /**
   * \brief This is the DoubleVector used to store length information
   */
  DoubleVector lengths;
  /**
   * \brief This is the LengthGroupDivision used to store length information
   */
  LengthGroupDivision* LgrpDiv;
  /**
   * \brief This is the DoubleMatrixPtrVector used to aggreate the age-length distribution information from the model data over a year, if the likelihood calculation is based on the aggregated data
   * \note The indices for this object are [area][stock][id] where id = age+(numage*length)
   */
  DoubleMatrixPtrVector modelYearData;
  /**
   * \brief This is the DoubleMatrixPtrVector used to aggreate the age-length distribution information from the observed data over a year, if the likelihood calculation is based on the aggregated data
   * \note The indices for this object are [area][stock][id] where id = age+(numage*length)
   */
  DoubleMatrixPtrVector obsYearData;
  /**
   * \brief This is the flag to denote whether the likelihood calculation should aggregate data over a whole year
   * \note The default value is 0, which calculates the likelihood score on each timestep
   */
  int yearly;
  /**
   * \brief This is the flag to denote whether the likelihood calculation should take overconsumption into account or not
   */
  int overconsumption;
  /**
   * \brief This is the identifier of the function to be used to calculate the likelihood component
   */
  int functionnumber;
  /**
   * \brief This is the name of the function to be used to calculate the likelihood component
   */
  char* functionname;
  /**
   * \brief This is the index of the timesteps for the likelihood component data
   */
  int timeindex;
  /**
   * \brief This ActionAtTimes stores information about when the likelihood score should be calculated
   */
  ActionAtTimes AAT;
  /**
   * \brief This is the value of epsilon used when calculating the likelihood score
   */
  double epsilon;
  /**
   * \brief This is the Multinomial that can be used when calculating the likelihood score, if the multinomial function has been selected
   * \note This is set to zero, and not used, if the multinomial function is not used
   */
  Multinomial MN;
  /**
   * \brief This is the AgeBandMatrixPtrVector used to temporarily store the information returned from the aggregatation function
   * \note The indices for this object are [area][age][length]
   */
  const AgeBandMatrixPtrVector* alptr;
};

#endif
