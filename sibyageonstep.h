#ifndef sibyageonstep_h
#define sibyageonstep_h

#include "sionstep.h"

class SIByAgeOnStep;
class TimeClass;
class StockAggregator;
class LengthGroupDivision;

class SIByAgeOnStep : public SIOnStep {
public:
  SIByAgeOnStep(CommentStream& infile, const intvector& areas,
    const intmatrix& ages, const char* arealabel, const charptrvector& ageindex,
    const TimeClass* const TimeInfo, const char* datafilename);
  virtual ~SIByAgeOnStep();
  virtual void Sum(const TimeClass* const TimeInfo);
  virtual void SetStocks(const Stockptrvector& Stocks);
  virtual void LikelihoodPrint(ofstream& outfile) const;
protected:
  StockAggregator* aggregator;
  intmatrix Ages;
  intvector Areas;
};

#endif
