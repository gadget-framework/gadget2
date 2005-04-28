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
  virtual void Print(ofstream& outfile) const;
  /**
   * \brief This function will print information from each SIOnStep regression calculation
   * \param outfile is the ofstream that all the model likelihood information gets sent to
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void LikelihoodPrint(ofstream& outfile, const TimeClass* const TimeInfo);
  virtual double calcRegression();
protected:
  /**
   * \brief This will return a null terminated text string containing the name of the SIOnStep object
   * \return siname
   */
  const char* getSIName() const { return siname; };
  int isToSum(const TimeClass* const TimeInfo) const;
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
  FitType getFitType() { return fittype; };
  /**
   * \brief This is the index of the timesteps for the survey index data
   */
  int timeindex;
private:
  /**
   * \brief This function will increase the internal error count by 1
   */
  void setError() { error++; };
  /**
   * \brief This function will read the SIOnStep data from the input file
   * \param infile is the CommentStream to read the SIOnStep data from
   * \param TimeInfo is the TimeClass for the current model
   */
  void readSIData(CommentStream& infile, const TimeClass* const TimeInfo);
  double fitRegression(const DoubleVector& stocksize, const DoubleVector& indices, int col);
  /**
   * \brief This is the number of errors that occured when fitting the regression line (usually caused by comparing a non-zero number of zero)
   */
  int error;
  double slope;
  double intercept;
  DoubleVector slopes;
  DoubleVector intercepts;
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
  FitType fittype;
};

#endif
