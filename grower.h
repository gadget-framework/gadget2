#ifndef grower_h
#define grower_h

#include "areatime.h"
#include "conversionindex.h"
#include "commentstream.h"
#include "popinfomatrix.h"
#include "doublematrixptrvector.h"
#include "livesonareas.h"
#include "formulavector.h"
#include "keeper.h"
#include "growthcalc.h"

class Grower : protected LivesOnAreas {
public:
  Grower(CommentStream& infile, const LengthGroupDivision* const OtherLgrpDiv,
    const LengthGroupDivision* const GivenLgrpDiv, const IntVector& Areas,
    const TimeClass* const TimeInfo, Keeper* const keeper, const char* refWeight,
    const AreaClass* const Area, const CharPtrVector& lenindex);
  ~Grower();
  void calcGrowth(int area, const AreaClass* const Area, const TimeClass* const TimeInfo,
    const DoubleVector& FeedingLevel, const DoubleVector& Consumption);
  void calcGrowth(int area, const AreaClass* const Area, const TimeClass* const TimeInfo);
  void GrowthImplement(int area, const PopInfoVector& NumberInArea,
    const LengthGroupDivision* const Lengths);
  void GrowthImplement(int area, const LengthGroupDivision* const Lengths);
  const DoubleMatrix& getLengthIncrease(int area) const;
  const DoubleMatrix& getWeightIncrease(int area) const;
  const DoubleVector& getWeight(int area) const;
  void Sum(const PopInfoVector& NumberInArea, int area);
  void Reset();
  void Print(ofstream& outfile) const;
  int getGrowthType() { return functionnumber; };
  int getFixedWeights() { return fixedweights; };
  double getPowerValue();
  double getMultValue();
protected:
  LengthGroupDivision* LgrpDiv;
  PopInfoMatrix numGrow;
  ConversionIndex* CI;
  DoubleMatrix interpLengthGrowth;
  DoubleMatrix interpWeightGrowth;
  DoubleMatrix calcLengthGrowth;
  DoubleMatrix calcWeightGrowth;
  DoubleMatrixPtrVector lgrowth;
  DoubleMatrixPtrVector wgrowth;
  DoubleVector dummyfphi;
  GrowthCalcBase* growthcalc;
  int functionnumber;
  int maxlengthgroupgrowth;
  Formula beta;
  DoubleVector part1;  //n*(n-1)*....(n-x+1)/x!
  DoubleVector part2;  //gamma(n-x+beta)/gamma(beta)
  double part3;        //gamma(alpha+beta)/gamma(n+alpha+beta)
  DoubleVector part4;  //gamma(x+alpha)/gamma(x)
  int fixedweights;
};

#endif
