#ifndef sibylengthonstep_h
#define sibylengthonstep_h

#include "sionstep.h"

class SIByLengthOnStep;
class StockAggregator;

class SIByLengthOnStep : public SIOnStep {
public:
  SIByLengthOnStep(CommentStream& infile, const IntMatrix& areas,
    const DoubleVector& lengths, const CharPtrVector& areaindex, const CharPtrVector& lenindex,
    const TimeClass* const TimeInfo, const char* datafilename, const char* name);
  virtual ~SIByLengthOnStep();
  virtual void Sum(const TimeClass* const TimeInfo);
  virtual void SetStocks(const StockPtrVector& Stocks);
protected:
  StockAggregator* aggregator;
  LengthGroupDivision* LgrpDiv;
};

#endif
