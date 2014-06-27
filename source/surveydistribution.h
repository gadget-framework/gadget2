#ifndef surveydistribution_h
#define surveydistribution_h

#include "likelihood.h"
#include "stockaggregator.h"
#include "doublematrixptrmatrix.h"
#include "actionattimes.h"
#include "formulavector.h"
#include "modelvariablevector.h"
#include "suitfunc.h"

/**
 * \class SurveyDistribution
 * \brief This is the class used to calculate a likelihood score based on survey data compiled from the stocks caught by government surveys, including some form of a survey index calculation
 *
 * This class calculates a likelihood score based on the difference between survey data sampled from stocks caught according to the model and that caught by government surveys.  The model will calculate the survey data for the stock that is caught according to the model parameters, and aggregate this into the specified age and length groups.  This survey data is then compared to the corresponding data calculated from the observed data.
 *
 * \note This likelihood component assumes that catch-at-age data is available, and is used in the survey index calculations
 * \note This likelihood component is based on the \b current population, where as most likelihood components are based on the population at the beginning of the timestep.  It would be better if this likelihood component was changed to be based on the population at the beginning of the timestep.
 */
class SurveyDistribution : public Likelihood {
public:
  /**
   * \brief This is the SurveyDistribution constructor
   * \param infile is the CommentStream to read the SurveyDistribution data from
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   * \param keeper is the Keeper for the current model
   * \param weight is the weight for the likelihood component
   * \param name is the name for the likelihood component
   */
  SurveyDistribution(CommentStream& infile, const AreaClass* const Area,
    const TimeClass* const TimeInfo, Keeper* const keeper, double weight, const char* name);
  /**
   * \brief This is the default SurveyDistribution destructor
   */
  virtual ~SurveyDistribution();
  /**
   * \brief This function will calculate the likelihood score for the SurveyDistribution component
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void addLikelihood(const TimeClass* const TimeInfo);
  /**
   * \brief This function will reset the SurveyDistribution likelihood information
   * \param keeper is the Keeper for the current model
   */
  virtual void Reset(const Keeper* const keeper);
  /**
   * \brief This function will print the summary SurveyDistribution likelihood information
   * \param outfile is the ofstream that all the model information gets sent to
   */
  virtual void Print(ofstream& outfile) const;
  /**
   * \brief This will select the fleets and stocks required to calculate the SurveyDistribution likelihood score
   * \param Fleets is the FleetPtrVector of all the available fleets
   * \param Stocks is the StockPtrVector of all the available stocks
   */
  void setFleetsAndStocks(FleetPtrVector& Fleets, StockPtrVector& Stocks);
  /**
   * \brief This function will print information from each SurveyDistribution calculation
   * \param outfile is the ofstream that all the model likelihood information gets sent to
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void printLikelihood(ofstream& outfile, const TimeClass* const TimeInfo);
  /**
   * \brief This function will print summary information from each SurveyDistribution likelihood calculation
   * \param outfile is the ofstream that all the model likelihood information gets sent to
   */
  virtual void printSummary(ofstream& outfile);
private:
  /**
   * \brief This function will read the SurveyDistribution data from the input file
   * \param infile is the CommentStream to read the SurveyDistribution data from
   * \param TimeInfo is the TimeClass for the current model
   * \param numarea is the number of areas that the likelihood data covers
   * \param numage is the number of age groups that the likelihood data covers
   * \param numlen is the number of length groups that the likelihood data covers
   */
  void readDistributionData(CommentStream& infile, const TimeClass* TimeInfo,
    int numarea, int numage, int numlen);
  /**
   * \brief This function will calculate the survey index based on the modelled population for the current timetsep
   * \param TimeInfo is the TimeClass for the current model
   */
  void calcIndex(const TimeClass* const TimeInfo);
  /**
   * \brief This function will calculate the likelihood score for the current timestep based on a multinomial function
   * \return likelihood score
   */
  double calcLikMultinomial();
  /**
   * \brief This function will calculate the likelihood score for the current timestep based on a Pearson function
   * \return likelihood score
   */
  double calcLikPearson();
  /**
   * \brief This function will calculate the likelihood score for the current timestep based on a gamma function
   * \return likelihood score
   */
  double calcLikGamma();
  /**
   * \brief This function will calculate the likelihood score for the current timestep based on a logarithmic function
   * \return likelihood score
   */
  double calcLikLog();
  /**
   * \brief This is the StockAggregator used to collect information about the stocks
   */
  StockAggregator* aggregator;
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
   * \brief This is the LengthGroupDivision used to store length information
   */
  LengthGroupDivision* LgrpDiv;
  /**
   * \brief This is the name of the fit function to be used to calculate the index
   */
  char* fittype;
  /**
   * \brief This is the identifier of the fit function to be used to calculate the index
   */
  int fitnumber;
  /**
   * \brief This is the name of the likelihood function to be used to calculate the likelihood score
   */
  char* liketype;
  /**
   * \brief This is the identifier of the likelihood function to be used to calculate the likelihood score
   */
  int likenumber;
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
   * \brief This is the DoubleMatrixPtrMatrix used to store survey distribution information specified in the input file
   * \note The indices for this object are [time][area][age][length]
   */
  DoubleMatrixPtrMatrix obsDistribution;
  /**
   * \brief This is the DoubleMatrixPtrMatrix used to store survey distribution information calculated in the model
   * \note The indices for this object are [time][area][age][length]
   */
  DoubleMatrixPtrMatrix modelDistribution;
  /**
   * \brief This is the DoubleMatrix used to store the calculated likelihood information
   */
  DoubleMatrix likelihoodValues;
  /**
   * \brief This is the index of the timesteps for the likelihood component data
   */
  int timeindex;
  /**
   * \brief This is the value of epsilon used when calculating the likelihood score
   */
  double epsilon;
  /**
   * \brief This is the SuitFunc used when calculating the survey index
   */
  SuitFunc* suitfunction;
  /**
   * \brief This is the DoubleVector used to store the length dependent suitability factor when calculating the survey index
   */
  DoubleVector suit;
  /**
   * \brief This is the ModelVariableVector used to store the parameters when calculating the survey index
   */
  ModelVariableVector parameters;
  /**
   * \brief This is the AgeBandMatrixPtrVector used to temporarily store the information returned from the aggregatation function
   */
  const AgeBandMatrixPtrVector* alptr;
};

#endif
