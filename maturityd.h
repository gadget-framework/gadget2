#ifndef maturityd_h
#define maturityd_h

#include "maturitya.h"

class MaturityD;

class MaturityD : public MaturityA {
public:
  MaturityD(CommentStream& infile, const TimeClass* const TimeInfo, Keeper* const keeper,
    int minage, const intvector& minabslength, const intvector& size,
    const intvector& areas, const LengthGroupDivision* const LgrpDiv, int NoMatconst);
  virtual ~MaturityD();
  virtual void Precalc(const TimeClass* const TimeInfo);
  virtual int IsMaturationStep(int area, const TimeClass* const TimeInfo);
  virtual double MaturationProbability(int age, int length, int Growth,
    const TimeClass* const TimeInfo, const AreaClass* const Area, int area);
protected:
  intvector maturitystep;
};

#endif

