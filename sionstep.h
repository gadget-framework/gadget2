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

enum FitType { LOGLINEARFIT = 1, FIXEDSLOPELOGLINEARFIT, FIXEDLOGLINEARFIT, LINEARFIT, POWERFIT,
    FIXEDSLOPELINEARFIT, FIXEDLINEARFIT, FIXEDINTERCEPTLINEARFIT, FIXEDINTERCEPTLOGLINEARFIT };

/**
 * \class SIOnStep
 * \brief This is the base class used to calculate a likelihood score by fitting the model population to survey index data
  * \note This will always be overridden by the derived classes that actually calculate the survey indices and the likelihood score from the regression line
 */
class SIOnStep {
public:
  SIOnStep(CommentStream& infile, const char* datafilename,
    const CharPtrVector& areaindex, const TimeClass* const TimeInfo,
    const IntMatrix& areas, const CharPtrVector& colindex, const char* name);
  /**
   * \brief This is the default SIOnStep destructor
   */
  virtual ~SIOnStep();
  virtual void Sum(const TimeClass* const TimeInfo) = 0;
  virtual void setFleetsAndStocks(FleetPtrVector& Fleets, StockPtrVector& Stocks) = 0;
  virtual double Regression();
  virtual void Reset(const Keeper* const keeper);
  virtual void Print(ofstream& outfile) const;
  virtual void LikelihoodPrint(ofstream& outfile);
protected:
  void setError() { error++; };
  int isToSum(const TimeClass* const TimeInfo) const;
  /**
   * \brief This will return a null terminated text string containing the name of the SIOnStep object
   * \return siname
   */
  const char* SIName() const { return siname; };
  void keepNumbers(const DoubleVector& numbers);
  IntMatrix Areas;
  IntVector Years;
  IntVector Steps;
  ActionAtTimes AAT;
  DoubleMatrix Indices;
  DoubleMatrix abundance;
  FitType getFitType() { return fittype; };
private:
  void readSIData(CommentStream& infile, const CharPtrVector& areaindex,
    const CharPtrVector& colindex, const TimeClass* const TimeInfo);
  double Fit(const DoubleVector& stocksize, const DoubleVector& indices, int col);
  int numSums;
  FitType fittype;
  double slope;
  double intercept;
  DoubleVector slopes;
  DoubleVector intercepts;
  DoubleVector sse;
  int error;
  /**
   * \brief This is the name of the SIOnStep object
   */
  char* siname;
};

#endif
