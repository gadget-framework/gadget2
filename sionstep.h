#ifndef sionstep_h
#define sionstep_h

#include "doublematrix.h"
#include "intmatrix.h"
#include "charptrvector.h"
#include "commentstream.h"
#include "actionattimes.h"
#include "keeper.h"

class SIOnStep;
class TimeClass;

class SIOnStep {
public:
  SIOnStep(CommentStream& infile, const char* datafilename, const char* arealabel,
    const TimeClass* const TimeInfo, const charptrvector& lenindex, const charptrvector& ageindex);
  SIOnStep(CommentStream& infile, const char* datafilename, const char* arealabel,
    const TimeClass* const TimeInfo, const charptrvector& colindex);
  virtual ~SIOnStep();
  virtual void Sum(const TimeClass* const TimeInfo) = 0;
  virtual void SetStocks(const Stockptrvector& Stocks) = 0;
  int Error() const { return error; };
  void Clear() { error = 0; };
  virtual double Regression();
  virtual void Reset(const Keeper* const keeper);
  virtual void Print(ofstream& outfile) const;
  virtual void LikelihoodPrint(ofstream& outfile) const;
  virtual void printMoreLikInfo(ofstream& outfile) const {};
  virtual void printHeader(ofstream&, const PrintInfo& print, const char* name) {};
  virtual void print(ofstream&, const TimeClass& time, const PrintInfo&) {};
  virtual void PrintLikelihood(ofstream&, const TimeClass& time, const char*) {};
  virtual void PrintLikelihoodHeader(ofstream&, const char*) {};
protected:
  void SetError() { error = 1; };
  int IsToSum(const TimeClass* const TimeInfo) const;
  void KeepNumbers(const doublevector& numbers);
  intvector Years;
  intvector Steps;
  intvector YearsInFile;
  ActionAtTimes AAT;
  //JMB - made Indices and abundance protected
  doublematrix Indices;
  doublematrix abundance;
  enum FitType { LogLinearFit = 0, FixedSlopeLogLinearFit, FixedLogLinearFit, LinearFit, PowerFit, FixedSlopeLinearFit, FixedLinearFit };
  FitType getFitType() { return fittype; };
private:
  void ReadSIData(CommentStream&, const char*, const charptrvector&, const charptrvector&, const TimeClass*);
  void ReadSIData(CommentStream&, const char*, const charptrvector&, const TimeClass*);
  double Fit(const doublevector& stocksize, const doublevector& indices, int col);
  int NumberOfSums;
  FitType fittype;
  double slope;
  double intercept;
  //Additions to facilitate printing
  doublevector slopes;
  doublevector intercepts;
  doublevector sse;
  int error;
};

#endif
