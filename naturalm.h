#ifndef naturalm_h
#define naturalm_h

#include "areatime.h"
#include "commentstream.h"
#include "timevariableindexvector.h"

class NaturalM;
class Keeper;

class NaturalM {
public:
  NaturalM(CommentStream& infile, int minage, int maxage,
    const TimeClass* const TimeInfo, Keeper* const keeper);
  NaturalM() {};
  const DoubleVector& ProportionSurviving(const TimeClass* const TimeInfo) const;
  const DoubleIndexVector& getMortality() const;
  void Reset(const TimeClass* const TimeInfo);
  void Print(ofstream& outfile);
protected:
  TimeVariableIndexVector mortality;
  DoubleVector proportion;
};

#endif
