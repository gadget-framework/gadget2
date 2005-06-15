#ifndef sionstep_h
#define sionstep_h

#include "areatime.h"
#include "doublematrix.h"
#include "intmatrix.h"
#include "charptrvector.h"
#include "conversionindex.h"
#include "commentstream.h"
#include "actionattimes.h"
#include "keeper.h"
#include "agebandmatrix.h"
#include "stockaggregator.h"
#include "gadget.h"

enum FitType { LINEARFIT = 1, LOGLINEARFIT, FIXEDSLOPELINEARFIT, FIXEDSLOPELOGLINEARFIT,
  FIXEDINTERCEPTLINEARFIT, FIXEDINTERCEPTLOGLINEARFIT, FIXEDLINEARFIT, FIXEDLOGLINEARFIT };

/**
 * \class SIOnStep
 * \brief This is the base class used to calculate a likelihood score by fitting a regression line to the model population and survey index data
 * \note This will always be overridden by the derived classes that actually calculate the survey indices and the likelihood score from the regression line
 */
class SIOnStep {
public:
  /**
   * \brief This is the SIOnStep constructor
   * \param infile is the CommentStream to read the SIOnStep data from
   * \param datafilename is the name of the file to read the index data from
   * \param aindex is the CharPtrVector of the names of the areas for the survey indices
   * \param TimeInfo is the TimeClass for the current model
   * \param areas is the IntMatrix of the areas that the survey indices are calculated on
   * \param charindex is the CharPtrVector of the names of the column index for the survey indices
   * \param name is the name for the SIOnStep component
   */
  SIOnStep(CommentStream& infile, const char* datafilename,
    const CharPtrVector& aindex, const TimeClass* const TimeInfo,
    const IntMatrix& areas, const CharPtrVector& charindex, const char* name);
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
   * \brief This function will reset the SIOnStep information
   * \param keeper is the Keeper for the current model
   */
  virtual void Reset(const Keeper* const keeper);
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
   * \brief This function will calculate the SSE from the regression line
   * \return SSE from the regession line
   */
  virtual double calcSSE();
protected:
  /**
   * \brief This will return a null terminated text string containing the name of the SIOnStep object
   * \return siname
   */
  const char* getSIName() const { return siname; };
  /**
   * \brief This function will return the number of regression lines to be calculated
   * \return number of regression lines
   */
  int numIndex() const { return colindex.Size(); };
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
   * \brief This ActionAtTimes stores information about when the survey index should be calculated
   */
  ActionAtTimes AAT;
  /**
   * \brief This is the DoubleMatrix used to store the survey indices specified in the input file
   */
  DoubleMatrix obsIndex;
  /**
   * \brief This is the DoubleMatrix used to store the survey indices calculated in the model
   */
  DoubleMatrix modelIndex;
  /**
   * \brief This is the AgeBandMatrix used to temporarily store the information returned from the aggregatation function
   */
  const AgeBandMatrix* alptr;
  /**
   * \brief This is the index of the timesteps for the survey index data
   */
  int timeindex;
private:
  /**
   * \brief This function will read the SIOnStep data from the input file
   * \param infile is the CommentStream to read the SIOnStep data from
   * \param TimeInfo is the TimeClass for the current model
   */
  void readSIData(CommentStream& infile, const TimeClass* const TimeInfo);
  /**
   * \brief This function will calculate the regression line used to fit the modelled data to the observed data
   * \param stocksize is the DoubleVector containing the index based on the modelled data
   * \param indices is the DoubleVector containing the index based on the observed data
   * \param i is the current index
   * \return SSE from the regession line
   */
  double calcRegression(const DoubleVector& stocksize, const DoubleVector& indices, int i);
  /**
   * \brief This is used to fix the slope of the regression lines if specified by the user
   */
  double slope;
  /**
   * \brief This is used to fix the intercept of the regression lines if specified by the user
   */
  double intercept;
  /**
   * \brief This is the DoubleVector used to store information about the slope of the regression lines
   */
  DoubleVector slopes;
  /**
   * \brief This is the DoubleVector used to store information about the intercept of the regression lines
   */
  DoubleVector intercepts;
  /**
   * \brief This is the DoubleVector used to store information about the sse of the regression lines
   */
  DoubleVector sse;
  /**
   * \brief This is the CharPtrVector of the names of the areas
   */
  CharPtrVector areaindex;
  /**
   * \brief This is the CharPtrVector of the names of the index
   */
  CharPtrVector colindex;
  /**
   * \brief This is the name of the SIOnStep object
   */
  char* siname;
  /**
   * \brief This denotes what type of fit is to be used for the linear regression line
   */
  FitType fittype;
};

#endif
