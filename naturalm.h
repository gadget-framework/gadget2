#ifndef naturalm_h
#define naturalm_h

#include "commentstream.h"
#include "timevariableindexvector.h"

class NaturalM;
class Keeper;
class TimeClass;

class NaturalM {
public:
  NaturalM(CommentStream& infile, int minage, int maxage,
    const TimeClass* const TimeInfo, Keeper* const keeper);
  const doublevector& ProportionSurviving(const TimeClass* const TimeInfo) const;
  const doubleindexvector& getMortality() const;
  void Reset(const TimeClass* const TimeInfo);
  void Print(ofstream& outfile);
protected:
  TimeVariableIndexVector mortality;
  doublevector lengthofsteps;
  doublevector proportion;
};

#endif
