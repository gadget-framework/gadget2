#ifndef catchdistribution_h
#define catchdistribution_h

#include "likelihood.h"
#include "fleetpreyaggregator.h"
#include "doublematrixptrmatrix.h"
#include "actionattimes.h"

class Keeper;
class TimeClass;
class AreaClass;

class CatchDistribution : public Likelihood {
public:
  CatchDistribution(CommentStream& infile, const AreaClass* const Area,
    const TimeClass* const TimeInfo, double likweight, const char* name);
  virtual ~CatchDistribution();
  virtual void AddToLikelihood(const TimeClass* const TimeInfo);
  virtual void Reset(const Keeper* const keeper);
  virtual void Init(const Ecosystem* eco);
  virtual void Print(ofstream& outfile) const;
  void SetFleetsAndStocks(Fleetptrvector& Fleets, Stockptrvector& Stocks);
  virtual void LikelihoodPrint(ofstream& outfile) const;
  virtual void PrintLikelihood(ofstream&, const TimeClass& time);
  virtual void PrintLikelihoodHeader(ofstream&);
  //JMB const doublevector& getWeights(int year, int step, int lengthgroup) const;
  const doublematrix& getCalcBiomass(int area) const { return *(calc_biomass[area]); };
  const doublematrix& getObsBiomass(int area) const { return *(obs_biomass[area]); };
  const doublematrix& getAggCalcBiomass(int area) const { return *(agg_calc_biomass[area]); };
  const doublematrix& getAggObsBiomass(int area) const { return *(agg_obs_biomass[area]); };
  const char* Name() const { return name; };
  const intmatrix& getAges() const { return ages; };
  int aggLevel() const { return agg_lev; };
  void aggregateBiomass();
  int maxAge() const {
    int max = 0;
    int i, j;
    for (i = 0; i < ages.Nrow(); i++)
      for (j = 0; j < ages[i].Size(); j++)
        if (ages[i][j] > max)
          max = ages[i][j];
    return max;
  };
  int ageCols() const {
    if (ages.Nrow() < 1)
      return 0;
    int col = ages[0].Size();
    int i;
    for (i = 1; i < ages.Nrow(); i++)
      if (ages[i].Size() != col)
        return 0;
    return col;
  };
  const Agebandmatrixvector& getModCatch(const TimeClass* const TimeInfo); //kgf 21/1 99
  const doublematrixptrmatrix* getObsCatch() const { return &AgeLengthData; };
  const doublematrixptrmatrix* getCalcCatch() const { return &Proportions; };
  const doublematrix& calcCatchByTimeAge(int area) const { return *(calc_catch[area]); };
  const doublematrix& obsCatchByTimeAge(int area) const { return *(obs_catch[area]); };
  void timeIncrement() { timeindex++; };
  int getTimeSteps();
  int getAreas();
  intvector getYears();
  intvector getSteps();
  int getMinStockAge() { return min_stock_age; };
  int getMaxStockAge() { return max_stock_age; };
  int minRow() { return minrow; };
  int maxRow() { return maxrow; };
  int minCol(int row) { return mincol[row]; };
  int maxCol(int row) { return maxcol[row]; };
  void calcBiomass(int timeindex);
private:
  void ReadDistributionData(CommentStream&, const TimeClass*, int, int, int);
  double LikMultinomial();
  double LikPearson(const TimeClass* const TimeInfo);
  double ModifiedMultinomial(const TimeClass* const TimeInfo); //kgf 13/11 98
  double ExperimentalLik(const TimeClass* const TimeInfo); //kgf 6/3 00
  double GammaLik(const TimeClass* const TimeInfo); //kgf 24/5 00
  doublematrixptrmatrix AgeLengthData; //[time][area][age][length]
  doublematrixptrmatrix Proportions; //kgf 17/9 98
  doublematrix Likelihoodvalues; //kgf 17/9 98
  FleetPreyAggregator* aggregator;
  charptrvector fleetnames;
  charptrvector stocknames;
  const char* name;
  intmatrix areas;
  intmatrix ages;
  doublevector lengths;
  charptrvector areaindex;
  charptrvector ageindex;
  charptrvector lenindex;
  int overconsumption;    //should we take overconsumption into account
  int functionnumber;
  int timeindex;
  ActionAtTimes AAT;
  int minp;
  intvector Years;
  intvector Steps;
  int steps;
  int first;
  //JMB int readWeights;
  //JMB doublematrix weights;
  doublematrixptrvector calc_c;           //calc_c[area][age][length]
  doublematrixptrvector obs_c;            //obs_c[area][age][length]
  doublematrixptrvector calc_biomass;     //calc_biomass[area][time][age]
  doublematrixptrvector obs_biomass;      //obs_biomass[area][time][age]
  doublematrixptrvector agg_calc_biomass; //calc_biomass[area][time][age]
  doublematrixptrvector agg_obs_biomass;  //obs_biomass[area][time][age]
  doublematrixptrvector calc_catch;       //calc_catch[area][time][age]
  doublematrixptrvector obs_catch;        //obs_catch[area][time][age]
  int agg_lev; //0: calculate likelihood on all time steps, 1: calculate once a year
  int min_stock_age; //kgf 3/5 99
  int max_stock_age; //kgf 3/5 99
  int MinAge;
  int MaxAge;
  int min_data_age;
  int max_data_age;
  intvector mincol;
  intvector maxcol;
  int minrow;
  int maxrow;
  //AJ 02.06.00 Changing int dl to double dl
  double dl;
  LengthGroupDivision* LgrpDiv;
};

#endif
