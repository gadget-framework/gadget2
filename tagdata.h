#ifndef tagdata_h
#define tagdata_h

#include "likelihood.h"
#include "commentstream.h"
#include "fleetpreyaggregator.h"
#include "actionattimes.h"

//JMB - this is incomplete
class TagData : public Likelihood {
public:
  TagData(CommentStream& infile, const AreaClass* const Area,
    const TimeClass* const TimeInfo, double w);
  virtual ~TagData();
  virtual void AddToLikelihood(const TimeClass* const TimeInfo);
  virtual void Reset(const Keeper* const keeper);
  virtual void Print(ofstream& outfile) const {};
  virtual void SetFleetsAndStocks(FleetPtrVector& Fleets, StockPtrVector& Stocks);
private:
  void ReadRecaptureData(CommentStream&, const TimeClass*, CharPtrVector);
  double LikBinomial();
  FleetPreyAggregator* aggregator;
  CharPtrVector fleetnames;
  IntVector areas;
  int index;
  IntVector Years;
  IntVector Steps;
  IntVector recTime;
  IntVector recAreas;
  IntVector recAge;
  IntVector recFleet;
  IntVector recMaturity;
  DoubleVector tagLength;
  DoubleVector recLength;
};

#endif
