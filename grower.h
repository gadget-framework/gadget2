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

class Grower : public LivesOnAreas {
public:
  Grower(CommentStream& infile, const LengthGroupDivision* const OtherLgrpDiv,
    const LengthGroupDivision* const GivenLgrpDiv, const intvector& Areas,
    const TimeClass* const TimeInfo, Keeper* const keeper, const char* refWeight,
    const AreaClass* const Area, const charptrvector& lenindex);
  ~Grower();
  void GrowthCalc(int area, const AreaClass* const Area, const TimeClass* const TimeInfo,
    const doublevector& FeedingLevel, const doublevector& Consumption);
  void GrowthCalc(int area, const AreaClass* const Area, const TimeClass* const TimeInfo);
  void GrowthImplement(int area, const popinfovector& NumberInArea,
    const LengthGroupDivision* const Lengths);
  void GrowthImplement(int area, const LengthGroupDivision* const Lengths);
  const doublematrix& LengthIncrease(int area) const;
  const doublematrix& WeightIncrease(int area) const;
  const doublevector& getWeight(int area) const;
  void Sum(const popinfovector& NumberInArea, int area);
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
  popinfomatrix GrEatNumber;
  ConversionIndex* CI;
  doublematrix InterpLgrowth;
  doublematrix InterpWgrowth;
  doublematrix CalcLgrowth;
  doublematrix CalcWgrowth;
  doublematrixptrvector lgrowth;
  doublematrixptrvector wgrowth;
  doublevector Fphi;
  GrowthImplementparameters* GrIPar;
  GrowthCalcBase* growthcalc;
  int growthtype;
  char* functionname;
  double power;
  Formulavector MeanVarianceParameters;
  int version;
  int maxlengthgroupgrowth;
  Formula beta;
  doublevector part1;  //n*(n-1)*....(n-x+1)/x!
  doublevector part2;  //gamma(n-x+beta)/gamma(beta)
  double part3;        //gamma(alpha+beta)/gamma(n+alpha+beta)
  doublevector part4;  //gamma(x+alpha)/gamma(x)
};

#endif
