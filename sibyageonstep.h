#ifndef sibyageonstep_h
#define sibyageonstep_h

#include "sionstep.h"

class SIByAgeOnStep;
class StockAggregator;

class SIByAgeOnStep : public SIOnStep {
public:
  SIByAgeOnStep(CommentStream& infile, const IntMatrix& areas,
    const IntMatrix& ages, const CharPtrVector& areaindex, const CharPtrVector& ageindex,
    const TimeClass* const TimeInfo, const char* datafilename, const char* name);
  virtual ~SIByAgeOnStep();
  virtual void Sum(const TimeClass* const TimeInfo);
  virtual void SetStocks(const StockPtrVector& Stocks);
protected:
  StockAggregator* aggregator;
  IntMatrix Ages;
};

#endif
