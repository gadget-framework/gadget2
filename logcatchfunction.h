#ifndef logcatchfunction_h
#define logcatchfunction_h

#include "likelihood.h"
#include "commentstream.h"
#include "fleetpreyaggregator.h"
#include "doublematrixptrmatrix.h"
#include "actionattimes.h"

class LogCatches : public Likelihood {
public:
  LogCatches(CommentStream& infile, const AreaClass* const Area,
    const TimeClass* const TimeInfo, double w);
  virtual ~LogCatches();
  virtual void AddToLikelihood(const TimeClass* const TimeInfo);
  virtual void Reset(const Keeper* const keeper);
  virtual void Print(ofstream& outfile) const {};
  void SetFleetsAndStocks(FleetPtrVector& Fleets, StockPtrVector& Stocks);
  virtual void LikelihoodPrint(ofstream& outfile);
private:
  void ReadLogCatchData(CommentStream&, const TimeClass*, int, int, int);
  void ReadLogWeightsData(CommentStream&, const TimeClass*, int);
  double LogLik(const TimeClass* const TimeInfo);
  DoubleMatrixPtrMatrix AgeLengthData;
  DoubleMatrixPtrMatrix Proportions;
  DoubleMatrix Likelihoodvalues;
  FleetPreyAggregator* aggregator;
  LengthGroupDivision* lgrpDiv;
  CharPtrVector fleetnames;
  CharPtrVector stocknames;
  IntMatrix areas;
  IntMatrix ages;
  DoubleVector lengths;
  CharPtrVector areaindex;
  CharPtrVector ageindex;
  CharPtrVector lenindex;
  int overconsumption;    //should we take overconsumption into account
  int functionnumber;
  char* functionname;
  int timeindex;
  int readWeights;
  ActionAtTimes AAT;
  IntVector Years;
  IntVector Steps;
  DoubleMatrix weights;
  DoubleMatrixPtrVector calc_c;
  DoubleMatrixPtrVector obs_c;
  int agg_lev; //0: calculate likelihood on all time steps, or
               //1: calculate likelihood once a year
  int min_stock_age; //kgf 10/5 99
  int max_stock_age;//kgf 10/5 99
};

#endif
