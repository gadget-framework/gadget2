#ifndef catchdistribution_h
#define catchdistribution_h

#include "likelihood.h"
#include "fleetpreyaggregator.h"
#include "doublematrixptrmatrix.h"
#include "actionattimes.h"
#include "formula.h"
#include "ludecomposition.h"
#include "formulavector.h"

class CatchDistribution : public Likelihood {
public:
  CatchDistribution(CommentStream& infile, const AreaClass* const Area,
    const TimeClass* const TimeInfo, Keeper* const keeper, double w, const char* name);
  virtual ~CatchDistribution();
  virtual void AddToLikelihood(const TimeClass* const TimeInfo);
  virtual void Reset(const Keeper* const keeper);
  virtual void Print(ofstream& outfile) const;
  void setFleetsAndStocks(FleetPtrVector& Fleets, StockPtrVector& Stocks);
  virtual void LikelihoodPrint(ofstream& outfile);
  virtual void PrintLikelihood(ofstream&, const TimeClass& time);
  virtual void PrintLikelihoodHeader(ofstream&);
private:
  void readDistributionData(CommentStream& infile, const TimeClass* TimeInfo,
    int numarea, int numage, int numlen);
  double LikMultinomial();
  double LikPearson(const TimeClass* const TimeInfo);
  double LikGamma(const TimeClass* const TimeInfo); //kgf 24/5 00
  double LikSumSquares();
  double LikMVNormal();
  double LikMVLogistic();
  void Correlation();
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
  DoubleMatrixPtrVector calc_c;           //calc_c[area][age][length]
  DoubleMatrixPtrVector obs_c;            //obs_c[area][age][length]
  int agg_lev; //0: calculate likelihood on all time steps, 1: calculate once a year
  int min_stock_age; //kgf 3/5 99
  int max_stock_age; //kgf 3/5 99
  IntVector mincol;
  IntVector maxcol;
  int minrow;
  int maxrow;
  LengthGroupDivision* LgrpDiv;
  double epsilon;
  Formula tau;
  Formula sigma;
  FormulaVector params;
  int illegal;
  int times;
  int lag;
  LUDecomposition LU;
};

#endif
