#ifndef maturitya_h
#define maturitya_h

#include "maturity.h"
#include "timevariablevector.h"

class MaturityA;

class MaturityA : public Maturity {
public:
  MaturityA(CommentStream& infile, const TimeClass* const TimeInfo,
    Keeper* const keeper, int minage, const IntVector& minabslength,
    const IntVector& size, const IntVector& areas,
    const LengthGroupDivision* const LgrpDiv, int NoMatconst);
  virtual ~MaturityA();
  virtual void Precalc(const TimeClass* const TimeInfo);
  virtual int IsMaturationStep(int area, const TimeClass* const TimeInfo);
  virtual double MaturationProbability(int age, int length, int Growth,
    const TimeClass* const TimeInfo, const AreaClass* const Area, int area);
  virtual void Print(ofstream& outfile) const;
protected:
  BandMatrix PrecalcMaturation;
  TimeVariableVector Coefficient;
  int MinMatureAge;
};

#endif
