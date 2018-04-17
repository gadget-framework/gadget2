#include "selectfunc.h"
#include "gadget.h"

// ********************************************************
// Functions for base selection function
// ********************************************************
void SelectFunc::readConstants(CommentStream& infile,
  const TimeClass* const TimeInfo, Keeper* const keeper) {

  coeff.read(infile, TimeInfo, keeper);
  coeff.Update(TimeInfo);
}

void SelectFunc::updateConstants(const TimeClass* const TimeInfo) {
  coeff.Update(TimeInfo);
}

int SelectFunc::didChange(const TimeClass* const TimeInfo) {
  return coeff.didChange(TimeInfo);
}

// ********************************************************
// Functions for ConstSelectFunc selection function
// ********************************************************
ConstSelectFunc::ConstSelectFunc() : SelectFunc("ConstantSelectFunc") {
  coeff.setsize(1);
}

double ConstSelectFunc::calculate(double len) {
  return coeff[0];
}

// ********************************************************
// Functions for ExpSelectFunc selection function
// ********************************************************
ExpSelectFunc::ExpSelectFunc() : SelectFunc("ExponentialSelectFunc") {
  coeff.setsize(2);
}

double ExpSelectFunc::calculate(double len) {
  return (1.0 / (1.0 + exp(coeff[0] * (len - coeff[1]))));
}

// ********************************************************
// Functions for StraightSelectFunc selection function
// ********************************************************
StraightSelectFunc::StraightSelectFunc() : SelectFunc("StraightLineSelectFunc") {
  coeff.setsize(2);
}

double StraightSelectFunc::calculate(double len) {
  return (coeff[0] * len + coeff[1]);
}
