#ifndef catchdistribution_h
#define catchdistribution_h

#include "likelihood.h"
#include "fleetpreyaggregator.h"
#include "doublematrixptrmatrix.h"
#include "actionattimes.h"

class CatchDistribution : public Likelihood {
public:
  CatchDistribution(CommentStream& infile, const AreaClass* const Area,
    const TimeClass* const TimeInfo, double w, const char* name);
  virtual ~CatchDistribution();
  virtual void AddToLikelihood(const TimeClass* const TimeInfo);
  virtual void Reset(const Keeper* const keeper);
  virtual void Print(ofstream& outfile) const;
  void SetFleetsAndStocks(FleetPtrVector& Fleets, StockPtrVector& Stocks);
  virtual void LikelihoodPrint(ofstream& outfile);
  virtual void PrintLikelihood(ofstream&, const TimeClass& time);
  virtual void PrintLikelihoodHeader(ofstream&);
  const char* Name() const { return cdname; };
  void aggregateBiomass();
  void timeIncrement() { timeindex++; };
  void calcBiomass(int timeindex);
private:
  void ReadDistributionData(CommentStream& infile, const TimeClass* TimeInfo,
    int numarea, int numage, int numlen);
  double LikMultinomial();
  double LikPearson(const TimeClass* const TimeInfo);
  double ModifiedMultinomial(const TimeClass* const TimeInfo); //kgf 13/11 98
  double ExperimentalLik(const TimeClass* const TimeInfo); //kgf 6/3 00
  double GammaLik(const TimeClass* const TimeInfo); //kgf 24/5 00
  double LikSumSquares();
  DoubleMatrixPtrMatrix AgeLengthData; //[time][area][age][length]
  DoubleMatrixPtrMatrix Proportions; //kgf 17/9 98
  DoubleMatrix Likelihoodvalues; //kgf 17/9 98
  FleetPreyAggregator* aggregator;
  CharPtrVector fleetnames;
  CharPtrVector stocknames;
  char* cdname;
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
  ActionAtTimes AAT;
  IntVector Years;
  IntVector Steps;
  int steps;
  int first;
  DoubleMatrixPtrVector calc_c;           //calc_c[area][age][length]
  DoubleMatrixPtrVector obs_c;            //obs_c[area][age][length]
  DoubleMatrixPtrVector calc_biomass;     //calc_biomass[area][time][age]
  DoubleMatrixPtrVector obs_biomass;      //obs_biomass[area][time][age]
  DoubleMatrixPtrVector agg_calc_biomass; //calc_biomass[area][time][age]
  DoubleMatrixPtrVector agg_obs_biomass;  //obs_biomass[area][time][age]
  DoubleMatrixPtrVector calc_catch;       //calc_catch[area][time][age]
  DoubleMatrixPtrVector obs_catch;        //obs_catch[area][time][age]
  int agg_lev; //0: calculate likelihood on all time steps, 1: calculate once a year
  int min_stock_age; //kgf 3/5 99
  int max_stock_age; //kgf 3/5 99
  IntVector mincol;
  IntVector maxcol;
  int minrow;
  int maxrow;
  LengthGroupDivision* LgrpDiv;
  double epsilon;
};

#endif
