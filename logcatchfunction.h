#ifndef logcatchfunction_h
#define logcatchfunction_h

#include "likelihood.h"
#include "commentstream.h"
#include "fleetpreyaggregator.h"
#include "doublematrixptrmatrix.h"
#include "actionattimes.h"

class Keeper;
class TimeClass;
class AreaClass;

class LogCatches : public Likelihood {
public:
  LogCatches(CommentStream& infile, const AreaClass* const Area,
    const TimeClass* const TimeInfo, double likweightw);
  virtual ~LogCatches();
  virtual void AddToLikelihood(const TimeClass* const TimeInfo);
  virtual void Reset(const Keeper* const keeper);
  virtual void Print(ofstream& outfile) const {};
  void SetFleetsAndStocks(Fleetptrvector& Fleets, Stockptrvector& Stocks);
  virtual void LikelihoodPrint(ofstream& outfile) const;
  void printHeader(ofstream& surveyfile, const PrintInfo& print);
  virtual void print(ofstream& surveyfile, const TimeClass& time, const PrintInfo& print);
  void PrintLikelihoodOnStep(ofstream&, const TimeClass& time, int print_type);
  const doublematrix& getCalcBiomass(int area) const { return *(calc_biomass[area]); };
  const doublematrix& getObsBiomass(int area) const { return *(obs_biomass[area]); };
  const doublematrix& getAggCalcBiomass(int area) const { return *(agg_calc_biomass[area]); };
  const doublematrix& getAggObsBiomass(int area) const { return *(agg_obs_biomass[area]); };
  int getMinStockAge() { return min_stock_age; };
  int getMaxStockAge() { return max_stock_age; };
private:
  void ReadLogCatchData(CommentStream&, const TimeClass*, int, int, int);
  void ReadLogWeightsData(CommentStream&, const TimeClass*, int);
  double LogLik(const TimeClass* const TimeInfo);
  doublematrixptrmatrix AgeLengthData;
  doublematrixptrmatrix Proportions;
  doublematrix Likelihoodvalues;
  FleetPreyAggregator* aggregator;
  LengthGroupDivision* lgrpDiv;
  charptrvector fleetnames;
  charptrvector stocknames;
  intmatrix areas;
  intmatrix ages;
  doublevector lengths;
  charptrvector areaindex;
  charptrvector ageindex;
  charptrvector lenindex;
  int overconsumption;    //should we take overconsumption into account
  int functionnumber;
  char* functionname;
  int timeindex;
  int readWeights;
  ActionAtTimes AAT;
  int minp;
  intvector Years;
  intvector Steps;
  doublematrix weights;
  doublematrixptrvector calc_c;
  doublematrixptrvector obs_c;
  doublematrixptrvector calc_biomass; //calc_biomass[area][time][age]
  doublematrixptrvector obs_biomass; //obs_biomass[area][time][age]
  doublematrixptrvector agg_calc_biomass; //calc_biomass[area][time][age]
  doublematrixptrvector agg_obs_biomass; //obs_biomass[area][time][age]
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
