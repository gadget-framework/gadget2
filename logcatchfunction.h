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
  virtual void CommandLinePrint(ofstream& outfile, const TimeClass& time, const PrintInfo& print);
  void PrintLikelihoodOnStep(ofstream&, const TimeClass& time, int print_type);
  const DoubleMatrix& getCalcBiomass(int area) const { return *(calc_biomass[area]); };
  const DoubleMatrix& getObsBiomass(int area) const { return *(obs_biomass[area]); };
  const DoubleMatrix& getAggCalcBiomass(int area) const { return *(agg_calc_biomass[area]); };
  const DoubleMatrix& getAggObsBiomass(int area) const { return *(agg_obs_biomass[area]); };
  int getMinStockAge() { return min_stock_age; };
  int getMaxStockAge() { return max_stock_age; };
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
  int minp;
  IntVector Years;
  IntVector Steps;
  DoubleMatrix weights;
  DoubleMatrixPtrVector calc_c;
  DoubleMatrixPtrVector obs_c;
  DoubleMatrixPtrVector calc_biomass; //calc_biomass[area][time][age]
  DoubleMatrixPtrVector obs_biomass; //obs_biomass[area][time][age]
  DoubleMatrixPtrVector agg_calc_biomass; //calc_biomass[area][time][age]
  DoubleMatrixPtrVector agg_obs_biomass; //obs_biomass[area][time][age]
  int agg_lev; //0: calculate likelihood on all time steps, or
               //1: calculate likelihood once a year
  int min_stock_age; //kgf 10/5 99
  int max_stock_age;//kgf 10/5 99
  int MinAge, MaxAge;
  double dl;
  int calcCSum; //For PrintLikelihoodOnStep
  int obsCSum;  //to collect C & C-hat
};

#endif
