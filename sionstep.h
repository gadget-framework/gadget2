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

class SIOnStep {
public:
  SIOnStep(CommentStream& infile, const char* datafilename, const CharPtrVector& areaindex,
    const TimeClass* const TimeInfo, int numcols, const IntMatrix& areas,
    const CharPtrVector& index1, const CharPtrVector& index2, const char* name);
  SIOnStep(CommentStream& infile, const char* datafilename, const CharPtrVector& areaindex,
    const TimeClass* const TimeInfo, const IntMatrix& areas,
    const CharPtrVector& colindex, const char* name);
  virtual ~SIOnStep();
  virtual void Sum(const TimeClass* const TimeInfo) = 0;
  virtual void setStocks(const StockPtrVector& Stocks) = 0;
  virtual double Regression();
  virtual void Reset(const Keeper* const keeper);
  virtual void Print(ofstream& outfile) const;
  virtual void LikelihoodPrint(ofstream& outfile);
  virtual void PrintLikelihood(ofstream& outfile, const TimeClass& time, const char*) {};
  virtual void PrintLikelihoodHeader(ofstream& outfile, const char* name) {};
protected:
  void setError() { error = 1; };
  int IsToSum(const TimeClass* const TimeInfo) const;
  /**
   * \brief This will return a null terminated text string containing the name of the SIOnStep object
   * \return siname
   */
  const char* SIName() const { return siname; };
  void keepNumbers(const DoubleVector& numbers);
  IntMatrix Areas;
  IntVector Years;
  IntVector Steps;
  IntVector YearsInFile;
  ActionAtTimes AAT;
  DoubleMatrix Indices;
  DoubleMatrix abundance;
  FitType getFitType() { return fittype; };
private:
  void readSIData(CommentStream& infile, const CharPtrVector& areaindex,
    const CharPtrVector& index1, const CharPtrVector& index2, const TimeClass* const TimeInfo);
  void readSIData(CommentStream& infile, const CharPtrVector& areaindex,
    const CharPtrVector& colindex, const TimeClass* const TimeInfo);
  double Fit(const DoubleVector& stocksize, const DoubleVector& indices, int col);
  int NumberOfSums;
  FitType fittype;
  double slope;
  double intercept;
  //Additions to facilitate printing
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
