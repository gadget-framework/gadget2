#ifndef catchstatistics_h
#define catchstatistics_h

#include "likelihood.h"
#include "commentstream.h"
#include "fleetpreyaggregator.h"
#include "doublematrixptrvector.h"
#include "actionattimes.h"

class Keeper;
class TimeClass;
class AreaClass;

class CatchStatistics : public Likelihood {
public:
  CatchStatistics(CommentStream& infile, const AreaClass* const Area,
    const TimeClass* const TimeInfo, double likweight);
  virtual ~CatchStatistics();
  virtual void AddToLikelihood(const TimeClass* const TimeInfo);
  virtual void Reset(const Keeper* const keeper);
  virtual void Print(ofstream& outfile) const;
  virtual void LikelihoodPrint(ofstream& outfile) const {};
  void SetFleetsAndStocks(Fleetptrvector& Fleets, Stockptrvector& Stocks);
private:
  void ReadStatisticsData(CommentStream&, const TimeClass*, int, int);
  double SOSWeightOrLength();
  doublematrixptrvector numbers;
  doublematrixptrvector mean;
  doublematrixptrvector variance;
  FleetPreyAggregator* aggregator;
  charptrvector fleetnames;
  charptrvector stocknames;
  intmatrix areas;
  intmatrix ages;
  charptrvector areaindex;
  charptrvector ageindex;
  int overconsumption;    //should we take overconsumption into account
  int timeindex;          //index for mean etc.
  int functionnumber;
  ActionAtTimes AAT;
};

#endif
