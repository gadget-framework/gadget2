#ifndef totalpredator_h
#define totalpredator_h

#include "lengthpredator.h"

class TotalPredator;

class TotalPredator : public LengthPredator {
public:
  TotalPredator(CommentStream& infile, const char* givenname, const intvector& areas,
    const LengthGroupDivision* const OtherLgrpDiv,
    const LengthGroupDivision* const GivenLgrpDiv,
    const TimeClass* const TimeInfo, Keeper* const keeper, double Multiplicative);
  virtual ~TotalPredator();
  virtual void Eat(int area, double LengthOfStep, double Temperature,
    double Areasize, int CurrentSubstep, int NrOfSubsteps);
  virtual void AdjustConsumption(int area, int NrOfSubsteps, int CurrentSubstep);
  virtual void Print(ofstream& outfile) const;
  const popinfovector& NumberPriortoEating(int area, const char* preyname) const;
};

#endif
