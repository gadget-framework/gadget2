#ifndef stockdistribution_h
#define stockdistribution_h

#include "likelihood.h"
#include "commentstream.h"
#include "fleetpreyaggregator.h"
#include "doublematrixptrmatrix.h"
#include "actionattimes.h"

class Keeper;
class TimeClass;
class AreaClass;

class StockDistribution : public Likelihood {
public:
  StockDistribution(CommentStream& infile, const AreaClass* const Area,
    const TimeClass* const TimeInfo, double likweight);
  virtual ~StockDistribution();
  virtual void AddToLikelihood(const TimeClass* const TimeInfo);
  virtual void Reset(const Keeper* const keeper);
  virtual void Print(ofstream& outfile) const;
  virtual void LikelihoodPrint(ofstream& outfile) const {};
  void SetFleetsAndStocks(Fleetptrvector& Fleets, Stockptrvector& Stocks);
private:
  void ReadStockData(CommentStream&, const TimeClass*, int, int, int);
  double LikMultinomial();
  double LikSumSquares();
  doublematrixptrmatrix AgeLengthData;
  doublematrixptrmatrix Proportions;
  FleetPreyAggregator** aggregator;
  charptrvector fleetnames;
  charptrvector stocknames;
  intmatrix areas;
  intmatrix ages;
  charptrvector areaindex;
  charptrvector ageindex;
  charptrvector lenindex;
  doublevector lengths;
  LengthGroupDivision* lgrpdiv;
  int overconsumption; //should we take overconsumption into account
  int functionnumber;
  char* functionname;
  int timeindex;
  ActionAtTimes AAT;
  int minp;
};

#endif
