#ifndef grower_h
#define grower_h

#include "commentstream.h"
#include "popinfomatrix.h"
#include "doublematrixptrvector.h"
#include "growthimplement.h"
#include "livesonareas.h"
#include "formulavector.h"

class Grower;
class AreaClass;
class TimeClass;
class Keeper;
class LengthGroupDivision;
class ConversionIndex;
class GrowthCalcBase;

class Grower : protected LivesOnAreas {
public:
  Grower(CommentStream& infile, const LengthGroupDivision* const OtherLgrpDiv,
    const LengthGroupDivision* const GivenLgrpDiv, const IntVector& Areas,
    const TimeClass* const TimeInfo, Keeper* const keeper, const char* refWeight,
    const AreaClass* const Area, const CharPtrVector& lenindex);
  ~Grower();
  void GrowthCalc(int area, const AreaClass* const Area, const TimeClass* const TimeInfo,
    const DoubleVector& FeedingLevel, const DoubleVector& Consumption);
  void GrowthCalc(int area, const AreaClass* const Area, const TimeClass* const TimeInfo);
  void GrowthImplement(int area, const PopInfoVector& NumberInArea,
    const LengthGroupDivision* const Lengths);
  void GrowthImplement(int area, const LengthGroupDivision* const Lengths);
  const DoubleMatrix& LengthIncrease(int area) const;
  const DoubleMatrix& WeightIncrease(int area) const;
  const DoubleVector& getWeight(int area) const;
  void Sum(const PopInfoVector& NumberInArea, int area);
  void Reset();
  void Print(ofstream& outfile) const;
  //From GrowthImplementParameters.
  int MaxLengthgroupGrowth() const { return maxlengthgroupgrowth; };
  int MinLengthgroupGrowth() const { return 0; };
  int getGrowthType() { return growthtype; };
  int Version() const { return version; };
  double getPowerValue();
  double getMultValue();
protected:
  LengthGroupDivision* LgrpDiv;
  PopInfoMatrix GrEatNumber;
  ConversionIndex* CI;
  DoubleMatrix InterpLgrowth;
  DoubleMatrix InterpWgrowth;
  DoubleMatrix CalcLgrowth;
  DoubleMatrix CalcWgrowth;
  DoubleMatrixPtrVector lgrowth;
  DoubleMatrixPtrVector wgrowth;
  DoubleVector Fphi;
  GrowthImplementParameters* GrIPar;
  GrowthCalcBase* growthcalc;
  int growthtype;
  char* functionname;
  double power;
  FormulaVector MeanVarianceParameters;
  int version;
  int maxlengthgroupgrowth;
  Formula beta;
  DoubleVector part1;  //n*(n-1)*....(n-x+1)/x!
  DoubleVector part2;  //gamma(n-x+beta)/gamma(beta)
  double part3;        //gamma(alpha+beta)/gamma(n+alpha+beta)
  DoubleVector part4;  //gamma(x+alpha)/gamma(x)
};

#endif
