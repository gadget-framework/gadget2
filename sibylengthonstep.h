#ifndef sibylengthonstep_h
#define sibylengthonstep_h

#include "sionstep.h"

class SIByLengthOnStep;
class StockAggregator;

class SIByLengthOnStep : public SIOnStep {
public:
  SIByLengthOnStep(CommentStream& infile, const IntVector& areas,
    const DoubleVector& lengths, const char* arealabel, const CharPtrVector& lenindex,
    const TimeClass* const TimeInfo, const char* datafilename);
  virtual ~SIByLengthOnStep();
  virtual void Sum(const TimeClass* const TimeInfo);
  virtual void SetStocks(const StockPtrVector& Stocks);
protected:
  StockAggregator* aggregator;
  LengthGroupDivision* LgrpDiv;
  IntVector Areas;
};

#endif
