#ifndef catchdistribution_h
#define catchdistribution_h

#include "likelihood.h"
#include "fleetpreyaggregator.h"
#include "doublematrixptrmatrix.h"
#include "actionattimes.h"
#include "ludecomposition.h"
#include "multinomial.h"
#include "formulavector.h"

class CatchDistribution : public Likelihood {
public:
  /**
   * \brief This is the CatchDistribution constructor
   * \param infile is the CommentStream to read the CatchDistribution data from
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   * \param keeper is the Keeper for the current model
   * \param weight is the weight for the likelihood component
   * \param name is the name for the likelihood component
   */
  CatchDistribution(CommentStream& infile, const AreaClass* const Area,
    const TimeClass* const TimeInfo, Keeper* const keeper, double weight, const char* name);
  /**
   * \brief This is the default CatchDistribution destructor
   */
  virtual ~CatchDistribution();
  /**
   * \brief This function will calculate the likelihood score for the CatchDistribution component
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void addLikelihood(const TimeClass* const TimeInfo);
  /**
   * \brief This function will reset the CatchDistribution likelihood information
   * \param keeper is the Keeper for the current model
   */
  virtual void Reset(const Keeper* const keeper);
  /**
   * \brief This function will print the summary CatchDistribution likelihood information
   * \param outfile is the ofstream that all the model information gets sent to
   */
  virtual void Print(ofstream& outfile) const;
  /**
   * \brief This will select the fleets and stocks required to calculate the CatchDistribution likelihood score
   * \param Fleets is the FleetPtrVector of all the available fleets
   * \param Stocks is the StockPtrVector of all the available stocks
   */
  void setFleetsAndStocks(FleetPtrVector& Fleets, StockPtrVector& Stocks);
  /**
   * \brief This function will print information from each CatchDistribution calculation
   * \param outfile is the ofstream that all the model likelihood information gets sent to
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void LikelihoodPrint(ofstream& outfile, const TimeClass* const TimeInfo);
  /**
   * \brief This function will print summary information from each CatchDistribution likelihood calculation
   * \param outfile is the ofstream that all the model likelihood information gets sent to
   */
  virtual void SummaryPrint(ofstream& outfile);
private:
  /**
   * \brief This function will read the CatchDistribution data from the input file
   * \param infile is the CommentStream to read the CatchDistribution data from
   * \param TimeInfo is the TimeClass for the current model
   * \param numarea is the number of areas that the likelihood data covers
   * \param numage is the number of age groups that the likelihood data covers
   * \param numlen is the number of length groups that the likelihood data covers
   */
  void readDistributionData(CommentStream& infile, const TimeClass* TimeInfo,
    int numarea, int numage, int numlen);
  /**
   * \brief This function will calculate the likelihood score for the current timestep based on a multinomial function
   * \return likelihood score
   */
  double calcLikMultinomial();
  /**
   * \brief This function will calculate the likelihood score for the current timestep based on a Pearson function
   * \param TimeInfo is the TimeClass for the current model
   * \return likelihood score
   */
  double calcLikPearson(const TimeClass* const TimeInfo);
  /**
   * \brief This function will calculate the likelihood score for the current timestep based on a gamma function
   * \param TimeInfo is the TimeClass for the current model
   * \return likelihood score
   */
  double calcLikGamma(const TimeClass* const TimeInfo);
  /**
   * \brief This function will calculate the likelihood score for the current timestep based on a sum of squares function
   * \return likelihood score
   */
  double calcLikSumSquares();
  /**
   * \brief This function will calculate the likelihood score for the current timestep based on a multivariate normal function
   * \return likelihood score
   */
  double calcLikMVNormal();
  /**
   * \brief This function will calculate the likelihood score for the current timestep based on a multivariate logistic function
   * \return likelihood score
   */
  double calcLikMVLogistic();
  /**
   * \brief This function will calculate the likelihood score for the current timestep based on a logarithmic function
   * \param TimeInfo is the TimeClass for the current model
   * \return likelihood score
   */
  double calcLikLog(const TimeClass* const TimeInfo);
  void calcCorrelation();
  /**
   * \brief This is the DoubleMatrixPtrMatrix used to store age-length distribution information specified in the input file
   * \note the indices for this object are [time][area][age][length]
   */
  DoubleMatrixPtrMatrix obsDistribution;
  /**
   * \brief This is the DoubleMatrixPtrMatrix used to store age-length distribution information calculated in the model
   * \note the indices for this object are [time][area][age][length]
   */
  DoubleMatrixPtrMatrix modelDistribution;
  /**
   * \brief This is the DoubleMatrix used to store the calculated likelihood information
   * \note the indices for this object are [time][area]
   */
  DoubleMatrix likelihoodValues;
  /**
   * \brief This is the FleetPreyAggregator used to collect information about the fleets
   */
  FleetPreyAggregator* aggregator;
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
   * \brief This is the DoubleVector used to store length information
   */
  DoubleVector lengths;
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
   * \brief This is a flag to denote whether the likelihood calculation should take overconsumption into account or not
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
   * \brief This is the IntVector used to store information about the years when the likelihood score should be calculated
   */
  IntVector Years;
  /**
   * \brief This is the IntVector used to store information about the steps when the likelihood score should be calculated
   */
  IntVector Steps;
  DoubleMatrixPtrVector calc_c;           //calc_c[area][age][length]
  DoubleMatrixPtrVector obs_c;            //obs_c[area][age][length]
  /**
   * \brief This is a flag to denote whether the likelihood calculation should aggregate data over a whole year.
   * \note the default value is 0, which calculates the likelihood score on each timestep
   */
  int yearly;
  /**
   * \brief This is the LengthGroupDivision used to store length information
   */
  LengthGroupDivision* LgrpDiv;
  /**
   * \brief This is the value of epsilon used when calculating the likelihood score
   */
  double epsilon;
  Formula sigma;
  FormulaVector params;
  int illegal;
  int lag;
  /**
   * \brief This is the LUDecomposition that can be used when calculating the likelihood score, if the multivariate normal function has been selected
   * \note This is set to zero, and not used, if the multivariate normal function is not used
   */
  LUDecomposition LU;
  /**
   * \brief This is the Multinomial that can be used when calculating the likelihood score, if the multinomial function has been selected
   * \note This is set to zero, and not used, if the multinomial function is not used
   */
  Multinomial MN;
  /**
   * \brief This is the AgeBandMatrixPtrVector used to temporarily store the information returned from aggregatation function
   */
  const AgeBandMatrixPtrVector* alptr;
};

#endif
