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
  virtual void Print(ofstream& outfile) const;
  void SetFleetsAndStocks(FleetPtrVector& Fleets, StockPtrVector& Stocks);
  virtual void LikelihoodPrint(ofstream& outfile) const;
  virtual void PrintLikelihood(ofstream&, const TimeClass& time);
  virtual void PrintLikelihoodHeader(ofstream&);
  //JMB const DoubleVector& getWeights(int year, int step, int lengthgroup) const;
  const DoubleMatrix& getCalcBiomass(int area) const { return *(calc_biomass[area]); };
  const DoubleMatrix& getObsBiomass(int area) const { return *(obs_biomass[area]); };
  const DoubleMatrix& getAggCalcBiomass(int area) const { return *(agg_calc_biomass[area]); };
  const DoubleMatrix& getAggObsBiomass(int area) const { return *(agg_obs_biomass[area]); };
  const char* Name() const { return cdname; };
  const IntMatrix& getAges() const { return ages; };
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
  const AgeBandMatrixPtrVector& getModCatch(const TimeClass* const TimeInfo); //kgf 21/1 99
  const DoubleMatrixPtrMatrix* getObsCatch() const { return &AgeLengthData; };
  const DoubleMatrixPtrMatrix* getCalcCatch() const { return &Proportions; };
  const DoubleMatrix& calcCatchByTimeAge(int area) const { return *(calc_catch[area]); };
  const DoubleMatrix& obsCatchByTimeAge(int area) const { return *(obs_catch[area]); };
  void timeIncrement() { timeindex++; };
  int getTimeSteps();
  int getAreas();
  IntVector getYears();
  IntVector getSteps();
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
