#ifndef stockdistribution_h
#define stockdistribution_h

#include "likelihood.h"
#include "commentstream.h"
#include "fleetpreyaggregator.h"
#include "doublematrixptrmatrix.h"
#include "actionattimes.h"

class StockDistribution : public Likelihood {
public:
  StockDistribution(CommentStream& infile, const AreaClass* const Area,
    const TimeClass* const TimeInfo, double w);
  virtual ~StockDistribution();
  virtual void AddToLikelihood(const TimeClass* const TimeInfo);
  virtual void Reset(const Keeper* const keeper);
  virtual void Print(ofstream& outfile) const;
  void SetFleetsAndStocks(FleetPtrVector& Fleets, StockPtrVector& Stocks);
private:
  void ReadStockData(CommentStream&, const TimeClass*, int, int, int);
  double LikMultinomial();
  double LikSumSquares();
  DoubleMatrixPtrMatrix AgeLengthData;
  DoubleMatrixPtrMatrix Proportions;
  FleetPreyAggregator** aggregator;
  CharPtrVector fleetnames;
  CharPtrVector stocknames;
  IntMatrix areas;
  IntMatrix ages;
  CharPtrVector areaindex;
  CharPtrVector ageindex;
  CharPtrVector lenindex;
  DoubleVector lengths;
  LengthGroupDivision* lgrpdiv;
  int overconsumption; //should we take overconsumption into account
  int functionnumber;
  char* functionname;
  int timeindex;
  ActionAtTimes AAT;
  double epsilon;
};

#endif
