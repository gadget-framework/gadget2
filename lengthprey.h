#ifndef lengthprey_h
#define lengthprey_h

#include "prey.h"

class LengthPrey;

class LengthPrey : public Prey {
public:
  LengthPrey(CommentStream& infile, const IntVector& areas,
    const char* givenname, Keeper* const keeper);
  LengthPrey(const DoubleVector& lengths, const IntVector& areas, const char* givenname);
  ~LengthPrey();
  virtual prey_type preyType() const { return LENGTHPREY_TYPE; };
  //AJ 07.06.00 Changing name of function Sum because already have
  //a virtual function Sum in class Prey
  void SumUsingPopInfo(const PopInfoVector& NumberInarea, int area, int NrofSubstep);
};

#endif
