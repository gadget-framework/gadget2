#ifndef catchstatistics_h
#define catchstatistics_h

#include "likelihood.h"
#include "commentstream.h"
#include "fleetpreyaggregator.h"
#include "doublematrixptrvector.h"
#include "actionattimes.h"

class CatchStatistics : public Likelihood {
public:
  CatchStatistics(CommentStream& infile, const AreaClass* const Area,
    const TimeClass* const TimeInfo, double weight, const char* name);
  virtual ~CatchStatistics();
  virtual void addLikelihood(const TimeClass* const TimeInfo);
  virtual void Reset(const Keeper* const keeper);
  virtual void Print(ofstream& outfile) const;
  void setFleetsAndStocks(FleetPtrVector& Fleets, StockPtrVector& Stocks);
private:
  void readStatisticsData(CommentStream& infile, const TimeClass* TimeInfo,
    int numarea, int numage);
  double calcLikSumSquares();
  DoubleMatrixPtrVector numbers;
  DoubleMatrixPtrVector mean;
  DoubleMatrixPtrVector variance;
  FleetPreyAggregator* aggregator;
  LengthGroupDivision* LgrpDiv;
  CharPtrVector fleetnames;
  CharPtrVector stocknames;
  IntMatrix areas;
  IntMatrix ages;
  CharPtrVector areaindex;
  CharPtrVector ageindex;
  int overconsumption;    //should we take overconsumption into account
  int timeindex;          //index for mean etc.
  int functionnumber;
  ActionAtTimes AAT;
  char* csname;
  char* functionname;
};

#endif
