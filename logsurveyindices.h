#ifndef logsurveyindices_h
#define logsurveyindices_h

#include "commentstream.h"
#include "charptrvector.h"
#include "sionstep.h"

class LogSurveyIndices;
class TimeClass;
class AreaClass;

class LogSurveyIndices : public Likelihood {
public:
  LogSurveyIndices(CommentStream& infile, const AreaClass* const Area,
    const TimeClass* const TimeInfo, Keeper* const keeper, double w, const char* name);
  virtual ~LogSurveyIndices();
  virtual void AddToLikelihood(const TimeClass* const TimeInfo);
  void SetStocks(Stockptrvector& Stocks);
  virtual void Reset(const Keeper* const keeper);
  virtual void Print(ofstream& outfile) const;
  virtual void LikelihoodPrint(ofstream& outfile) const;
  virtual void printMoreLikInfo(ofstream& outfile) const {
    SI->printMoreLikInfo(outfile); };
  void printHeader(ofstream& surveyfile, const PrintInfo& print);
  virtual void print(ofstream& surveyfile, const TimeClass& time, const PrintInfo& print);
protected:
  SIOnStep* SI;
  intvector areas;
  charptrvector stocknames;
  int timeindex;
  int minage;
  int maxage;
  double dl;
  double min_length;
  double max_length;
  const char* surveyname;
};

#endif
