#ifndef recstatistics_h
#define recstatistics_h

#include "likelihood.h"
#include "commentstream.h"
#include "recaggregator.h"
#include "doublematrixptrvector.h"
#include "tagptrvector.h"

class RecStatistics : public Likelihood {
public:
  RecStatistics(CommentStream& infile, const AreaClass* const Area,
    const TimeClass* const TimeInfo, double weight, TagPtrVector Tags, const char* name);
  virtual ~RecStatistics();
  virtual void AddToLikelihood(const TimeClass* const TimeInfo);
  virtual void Reset(const Keeper* const keeper);
  virtual void Print(ofstream& outfile) const;
  void setFleetsAndStocks(FleetPtrVector& Fleets, StockPtrVector& Stocks);
private:
  void readStatisticsData(CommentStream& infile, const TimeClass* TimeInfo,
    int numarea, TagPtrVector Tags);
  double SOSWeightOrLength();
  DoubleMatrixPtrVector numbers;
  DoubleMatrixPtrVector mean;
  DoubleMatrixPtrVector variance;
  RecAggregator** aggregator;
  LengthGroupDivision* lgrpDiv;
  CharPtrVector fleetnames;
  CharPtrVector tagnames;
  IntMatrix areas;
  CharPtrVector areaindex;
  int functionnumber;
  IntVector timeindex;
  IntMatrix Years;
  IntMatrix Steps;
  char* rsname;
  char* functionname;
  TagPtrVector tagvec;
};

#endif
