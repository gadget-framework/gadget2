#ifndef sionstep_h
#define sionstep_h

#include "areatime.h"
#include "doublematrixptrvector.h"
#include "intmatrix.h"
#include "charptrvector.h"
#include "conversionindex.h"
#include "commentstream.h"
#include "actionattimes.h"
#include "agebandmatrix.h"
#include "stockaggregator.h"
#include "regressionline.h"
#include "hasname.h"
#include "gadget.h"

enum FitType { LINEARFIT = 1, LOGLINEARFIT, FIXEDSLOPELINEARFIT, FIXEDSLOPELOGLINEARFIT,
  FIXEDINTERCEPTLINEARFIT, FIXEDINTERCEPTLOGLINEARFIT, FIXEDLINEARFIT, FIXEDLOGLINEARFIT };
enum SIType { SILENGTH = 1, SIAGE, SIFLEET };

/**
 * \class SIOnStep
 * \brief This is the base class used to calculate a likelihood score by fitting a regression line to the model population and survey index data
 * \note This will always be overridden by the derived classes that actually calculate the survey indices and the likelihood score from the regression line
 */
class SIOnStep : public HasName {
public:
  /**
   * \brief This is the SIOnStep constructor
   * \param infile is the CommentStream to read the SIOnStep data from
   * \param datafilename is the name of the file to read the index data from
   * \param aindex is the CharPtrVector of the names of the areas for the survey indices
   * \param TimeInfo is the TimeClass for the current model
   * \param areas is the IntMatrix of the areas that the survey indices are calculated on
   * \param charindex is the CharPtrVector of the names of the column index for the survey indices
   * \param givenname is the name for the SIOnStep component
   * \param bio is the flag to denote whether the index should be based on the biomass or not
   */
  SIOnStep(CommentStream& infile, const char* datafilename,
    const CharPtrVector& aindex, const TimeClass* const TimeInfo,
    const IntMatrix& areas, const CharPtrVector& charindex, const char* givenname, int bio);
  /**
   * \brief This is the default SIOnStep destructor
   */
  virtual ~SIOnStep();
  /**
   * \brief This function will sum the survey index data
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void Sum(const TimeClass* const TimeInfo) = 0;
  /**
   * \brief This will select the fleets and stocks required to calculate the regression line
   * \param Fleets is the FleetPtrVector of all the available fleets
   * \param Stocks is the StockPtrVector of all the available stocks
   */
  virtual void setFleetsAndStocks(FleetPtrVector& Fleets, StockPtrVector& Stocks) = 0;
  /**
   * \brief This function will print the SIOnStep information
   * \param outfile is the ofstream that all the model information gets sent to
   */
  virtual void Print(ofstream& outfile) const = 0;
  /**
   * \brief This function will print information from each SIOnStep regression calculation
   * \param outfile is the ofstream that all the model likelihood information gets sent to
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void printLikelihood(ofstream& outfile, const TimeClass* const TimeInfo);
  /**
   * \brief This function will print summary information from each SIOnStep regression calculation
   * \param outfile is the ofstream that all the model likelihood information gets sent to
   * \param weight is the weight of the likelihood component
   */
  virtual void printSummary(ofstream& outfile, const double weight);
  /**
   * \brief This function will calculate the SSE from the regression line
   * \return SSE from the regession line
   */
  virtual double calcSSE();
  /**
   * \brief This will return the fit type for the regression line
   * \return fittype
   */
  const FitType getType() const { return fittype; };
  /**
   * \brief This will return the type of survey index data to be used
   * \return sitype
   */
  const SIType getSIType() const { return sitype; };
protected:
  /**
   * \brief This is the IntMatrix used to store information about the areas that the survey index should be calculated on
   */
  IntMatrix Areas;
  /**
   * \brief This is the IntVector used to store information about the years when the survey index should be calculated
   */
  IntVector Years;
  /**
   * \brief This is the IntVector used to store information about the steps when the survey index should be calculated
   */
  IntVector Steps;
  /**
   * \brief This is the DoubleVector used to store the calculated likelihood information
   */
  DoubleVector likelihoodValues;
  /**
   * \brief This ActionAtTimes stores information about when the survey index should be calculated
   */
  ActionAtTimes AAT;
  /**
   * \brief This is the DoubleMatrixPtrVector used to store the survey indices specified in the input file
   * \note The indices for this object are [time][area][index]
   */
  DoubleMatrixPtrVector obsIndex;
  /**
   * \brief This is the DoubleMatrixPtrVector used to store the survey indices calculated in the model
   * \note The indices for this object are [time][area][index]
   */
  DoubleMatrixPtrVector modelIndex;
  /**
   * \brief This is the AgeBandMatrixPtrVector used to temporarily store the information returned from the aggregatation function
   */
  const AgeBandMatrixPtrVector* alptr;
  /**
   * \brief This denotes what type of survey index data is to be used
   */
  SIType sitype;
  /**
   * \brief This is the index of the timesteps for the survey index data
   */
  int timeindex;
  /**
   * \brief This is the flag to denote whether the index should be based on the biomass or not
   * \note The default value for this is 0, which means that the index is based on the number of the population
   */
  int biomass;
private:
  /**
   * \brief This function will read the SIOnStep data from the input file
   * \param infile is the CommentStream to read the SIOnStep data from
   * \param TimeInfo is the TimeClass for the current model
   */
  void readSIData(CommentStream& infile, const TimeClass* const TimeInfo);
  /**
   * \brief This is used to fix the slope of the regression lines if specified by the user
   */
  double slope;
  /**
   * \brief This is used to fix the intercept of the regression lines if specified by the user
   */
  double intercept;
  /**
   * \brief This is the DoubleMatrix used to store information about the slope of the regression lines
   */
  DoubleMatrix slopes;
  /**
   * \brief This is the DoubleMatrix used to store information about the intercept of the regression lines
   */
  DoubleMatrix intercepts;
  /**
   * \brief This is the DoubleMatrix used to store information about the sse of the regression lines
   */
  DoubleMatrix sse;
  /**
   * \brief This is the DoubleVector of the stocksize, used when calculating the fit to the regression line
   */
  DoubleVector stocksize;
  /**
   * \brief This is the DoubleVector of the indices, used when calculating the fit to the regression line
   */
  DoubleVector indices;
  /**
   * \brief This is the CharPtrVector of the names of the areas
   */
  CharPtrVector areaindex;
  /**
   * \brief This is the CharPtrVector of the names of the indices
   */
  CharPtrVector colindex;
  /**
   * \brief This denotes what type of fit is to be used for the linear regression line
   */
  FitType fittype;
  /**
   * \brief This is a Regression that stores the regression line used when calculating the likelihood score
   */
  Regression* LR;
};

#endif
