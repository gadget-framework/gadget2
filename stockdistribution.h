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
    const TimeClass* const TimeInfo, double w, const char* name);
  virtual ~StockDistribution();
  virtual void AddToLikelihood(const TimeClass* const TimeInfo);
  virtual void Reset(const Keeper* const keeper);
  virtual void Print(ofstream& outfile) const;
  virtual void LikelihoodPrint(ofstream& outfile);
  void setFleetsAndStocks(FleetPtrVector& Fleets, StockPtrVector& Stocks);
private:
  void readStockData(CommentStream&, const TimeClass*, int, int, int);
  double LikMultinomial();
  double LikSumSquares();
  DoubleMatrixPtrMatrix AgeLengthData;
  DoubleMatrixPtrMatrix Proportions;
  FleetPreyAggregator** aggregator;
  CharPtrVector fleetnames;
  CharPtrVector stocknames;
  IntMatrix areas;
  IntMatrix ages;
  IntVector Years;
  IntVector Steps;
  CharPtrVector areaindex;
  CharPtrVector ageindex;
  CharPtrVector lenindex;
  DoubleVector lengths;
  LengthGroupDivision* lgrpdiv;
  int overconsumption; //should we take overconsumption into account
  int functionnumber;
  char* functionname;
  char* sdname;
  int timeindex;
  ActionAtTimes AAT;
  double epsilon;
};

#endif
