#ifndef sibyageonstep_h
#define sibyageonstep_h

#include "sionstep.h"

class SIByAgeOnStep;
class TimeClass;
class StockAggregator;
class LengthGroupDivision;

class SIByAgeOnStep : public SIOnStep {
public:
  SIByAgeOnStep(CommentStream& infile, const IntVector& areas,
    const IntMatrix& ages, const char* arealabel, const CharPtrVector& ageindex,
    const TimeClass* const TimeInfo, const char* datafilename);
  virtual ~SIByAgeOnStep();
  virtual void Sum(const TimeClass* const TimeInfo);
  virtual void SetStocks(const StockPtrVector& Stocks);
  virtual void LikelihoodPrint(ofstream& outfile) const;
protected:
  StockAggregator* aggregator;
  IntMatrix Ages;
  IntVector Areas;
};

#endif
