#ifndef maturityb_h
#define maturityb_h

#include "maturity.h"
#include "timevariablevector.h"

class MaturityB;

class MaturityB : public Maturity {
public:
  MaturityB(CommentStream& infile, const TimeClass* const TimeInfo, Keeper* const keeper,
    int minage, const intvector& minabslength, const intvector& size,
    const intvector& areas, const LengthGroupDivision* const LgrpDiv);
  virtual ~MaturityB();
  virtual void Precalc(const TimeClass* const TimeInfo);
  virtual int IsMaturationStep(int area, const TimeClass* const TimeInfo);
  virtual double MaturationProbability(int age, int length, int Growth,
    const TimeClass* const TimeInfo, const AreaClass* const Area, int area);
  virtual void Print(ofstream& outfile) const;
protected:
  intvector maturitystep;
  TimeVariablevector maturitylength;
};

#endif
