#ifndef sibylengthonstep_h
#define sibylengthonstep_h

#include "sionstep.h"

class SIByLengthOnStep;
class TimeClass;
class StockAggregator;
class LengthGroupDivision;

class SIByLengthOnStep : public SIOnStep {
public:
  SIByLengthOnStep(CommentStream& infile, const intvector& areas,
    const doublevector& lengths, const char* arealabel, const charptrvector& lenindex,
    const TimeClass* const TimeInfo, const char* datafilename);
  virtual ~SIByLengthOnStep();
  virtual void Sum(const TimeClass* const TimeInfo);
  virtual void SetStocks(const Stockptrvector& Stocks);
protected:
  StockAggregator* aggregator;
  LengthGroupDivision* LgrpDiv;
  intvector Areas;
  int minage;
  int maxage;
};

#endif
