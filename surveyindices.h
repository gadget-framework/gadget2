#ifndef surveyindices_h
#define surveyindices_h

#include "likelihood.h"
#include "commentstream.h"
#include "sionstep.h"

class SurveyIndices;
class TimeClass;
class AreaClass;

class SurveyIndices : public Likelihood {
public:
  SurveyIndices(CommentStream& infile, const AreaClass* const Area,
    const TimeClass* const TimeInfo, Keeper* const keeper, double w, const char* name);
  virtual ~SurveyIndices();
  virtual void AddToLikelihood(const TimeClass* const TimeInfo);
  void SetStocks(StockPtrVector& Stocks);
  virtual void Reset(const Keeper* const keeper);
  virtual void Print(ofstream& outfile) const;
  virtual void LikelihoodPrint(ofstream& outfile) const;
  virtual void printMoreLikInfo(ofstream& outfile) const {
    SI->printMoreLikInfo(outfile); };
  virtual void printHeader(ofstream& outfile, const PrintInfo& print) {
    SI->printHeader(outfile, print, surveyname); };
  virtual void print(ofstream& outfile, const TimeClass& time, const PrintInfo& print) {
    SI->print(outfile, time, print); };
  virtual void PrintLikelihood(ofstream& outfile, const TimeClass& time) {
    SI->PrintLikelihood(outfile, time, surveyname); };
  virtual void PrintLikelihoodHeader(ofstream& outfile) {
    SI->PrintLikelihoodHeader(outfile, surveyname); };
protected:
  SIOnStep* SI;
  IntVector areas;
  CharPtrVector stocknames;
  char* surveyname;
};

#endif
