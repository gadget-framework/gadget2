#ifndef maturityc_h
#define maturityc_h

#include "maturitya.h"

class MaturityC;

class MaturityC : public MaturityA {
public:
  MaturityC(CommentStream& infile, const TimeClass* const TimeInfo, Keeper* const keeper,
    int minage, const IntVector& minabslength, const IntVector& size,
    const IntVector& areas, const LengthGroupDivision* const LgrpDiv, int NoMatconst);
  virtual ~MaturityC();
  virtual void Precalc(const TimeClass* const TimeInfo);
  virtual int IsMaturationStep(int area, const TimeClass* const TimeInfo);
  virtual double MaturationProbability(int age, int length, int Growth,
    const TimeClass* const TimeInfo, const AreaClass* const Area, int area);
protected:
  IntVector maturitystep;
};

#endif
