#include "suitfunc.h"
#include "errorhandler.h"
#include "gadget.h"
#include "global.h"

// ********************************************************
// Functions for base suitability function
// ********************************************************
void SuitFunc::readConstants(CommentStream& infile,
  const TimeClass* const TimeInfo, Keeper* const keeper) {

  coeff.read(infile, TimeInfo, keeper);
  coeff.Update(TimeInfo);
}

void SuitFunc::setPredLength(double length) {
  handle.logMessage(LOGWARN, "Warning in suitability - trying to set predator length for", this->getName());
}

void SuitFunc::setPreyLength(double length) {
  handle.logMessage(LOGWARN, "Warning in suitability - trying to set prey length for", this->getName());
}

const ModelVariableVector& SuitFunc::getConstants() const {
  return coeff;
}

void SuitFunc::updateConstants(const TimeClass* const TimeInfo) {
  coeff.Update(TimeInfo);
}

int SuitFunc::didChange(const TimeClass* const TimeInfo) {
  return coeff.didChange(TimeInfo);
}

// ********************************************************
// Functions for ExpSuitFuncA suitability function
// ********************************************************
ExpSuitFuncA::ExpSuitFuncA() : SuitFunc("ExponentialSuitFunc") {
  coeff.setsize(4);
  preyLength = -1.0;
  predLength = -1.0;
}

double ExpSuitFuncA::calculate() {
  double check = 0.0;

  if (coeff[0] < 0.0 && coeff[1] < 0.0)
    check = coeff[3] / (1.0 + exp(-(coeff[0] - (coeff[1] * preyLength) + (coeff[2] * predLength))));
  else if (coeff[0] > 0.0 && coeff[1] > 0.0)
    check = coeff[3] / (1.0 + exp(-(-coeff[0] + (coeff[1] * preyLength) + (coeff[2] * predLength))));
  else
    check = coeff[3] / (1.0 + exp(-(coeff[0] + (coeff[1] * preyLength) + (coeff[2] * predLength))));

  if (check != check) { //check for NaN
    handle.logMessageNaN(LOGWARN, "exponential suitability function");
    return 0.0;
  }

  if (check < 0.0) {
    handle.logMessage(LOGWARN, "Warning in suitability - function outside bounds", check);
    return 0.0;
  } else if (check > 1.0) {
    handle.logMessage(LOGWARN, "Warning in suitability - function outside bounds", check);
    return 1.0;
  } else
    return check;
}

// ********************************************************
// Functions for ConstSuitFunc suitability function
// ********************************************************
ConstSuitFunc::ConstSuitFunc() : SuitFunc("ConstantSuitFunc") {
  coeff.setsize(1);
}

double ConstSuitFunc::calculate() {
  if (coeff[0] < 0.0) {
    handle.logMessage(LOGWARN, "Warning in suitability - function outside bounds", coeff[0]);
    return 0.0;
  } else if (coeff[0] > 1.0) {
    handle.logMessage(LOGWARN, "Warning in suitability - function outside bounds", coeff[0]);
    return 1.0;
  } else
    return coeff[0];
}

// ********************************************************
// Functions for AndersenSuitFunc suitability function
// ********************************************************
AndersenSuitFunc::AndersenSuitFunc() : SuitFunc("AndersenSuitFunc") {
  coeff.setsize(5);
  preyLength = -1.0;
  predLength = -1.0;
}

double AndersenSuitFunc::calculate() {
  double l = log(predLength / preyLength);
  double e, q, check;

  q = 0.0;
  check = 0.0;
  if (l > coeff[1])
    q = coeff[3];
  else
    q = coeff[4];

  if (isZero(q))
    q = 1.0;
  if (q < 0.0)
    q = -q;

  e = (l - coeff[1]) * (l - coeff[1]);
  check = coeff[0] + coeff[2] * exp(-e / q);
  if (check < 0.0) {
    handle.logMessage(LOGWARN, "Warning in suitability - function outside bounds", check);
    return 0.0;
  } else if (check > 1.0) {
    handle.logMessage(LOGWARN, "Warning in suitability - function outside bounds", check);
    return 1.0;
  } else
    return check;
}

// ********************************************************
// Functions for ExpSuitFuncL50 suitability function
// ********************************************************
ExpSuitFuncL50::ExpSuitFuncL50() : SuitFunc("ExponentialL50SuitFunc") {
  coeff.setsize(2);
  preyLength = -1.0;
}

double ExpSuitFuncL50::calculate() {
  double check = 1.0 / (1.0 + exp(-1.0 * coeff[0] * (preyLength - coeff[1])));

  if (check != check) { //check for NaN
    handle.logMessageNaN(LOGWARN, "exponential l50 suitability function");
    return 0.0;
  }

  if (check < 0.0) {
    handle.logMessage(LOGWARN, "Warning in suitability - function outside bounds", check);
    return 0.0;
  } else if (check > 1.0) {
    handle.logMessage(LOGWARN, "Warning in suitability - function outside bounds", check);
    return 1.0;
  } else
    return check;
}

// ********************************************************
// Functions for StraightSuitFunc suitability function
// ********************************************************
StraightSuitFunc::StraightSuitFunc() : SuitFunc("StraightLineSuitFunc") {
  coeff.setsize(2);
  preyLength = -1.0;
}

double StraightSuitFunc::calculate() {
  double check = coeff[0] * preyLength + coeff[1];
  if (check < 0.0) {
    handle.logMessage(LOGWARN, "Warning in suitability - function outside bounds", check);
    return 0.0;
  } else if (check > 1.0) {
    handle.logMessage(LOGWARN, "Warning in suitability - function outside bounds", check);
    return 1.0;
  } else
    return check;
}

// ********************************************************
// Functions for InverseSuitFunc suitability function
// ********************************************************
InverseSuitFunc::InverseSuitFunc() : SuitFunc("InverseSuitFunc") {
  coeff.setsize(2);
  preyLength = -1.0;
}

double InverseSuitFunc::calculate() {
  double check = 1.0 / (1.0 + exp(-1.0 * coeff[0] * (preyLength - coeff[1])));

  if (check != check) { //check for NaN
    handle.logMessageNaN(LOGWARN, "inverse suitability function");
    return 0.0;
  }

  // make this value 1 - check to switch the direction of the slope
  check = 1.0 - check;

  if (check < 0.0) {
    handle.logMessage(LOGWARN, "Warning in suitability - function outside bounds", check);
    return 0.0;
  } else if (check > 1.0) {
    handle.logMessage(LOGWARN, "Warning in suitability - function outside bounds", check);
    return 1.0;
  } else
    return check;
}

// ********************************************************
// Functions for StraightUnboundedSuitFunc suitability function
// ********************************************************
StraightUnboundedSuitFunc::StraightUnboundedSuitFunc() : SuitFunc("StraightLineUnboundedSuitFunc") {
  coeff.setsize(2);
  preyLength = -1.0;
}

double StraightUnboundedSuitFunc::calculate() {
  double check = coeff[0] * preyLength + coeff[1];
  if (check < 0.0) {
    handle.logMessage(LOGWARN, "Warning in suitability - function outside bounds", check);
    return 0.0;
  } else
    return check;
}

// ********************************************************
// Functions for Richards suitability function
// ********************************************************
RichardsSuitFunc::RichardsSuitFunc() : SuitFunc("RichardsSuitFunc") {
  coeff.setsize(5);
  preyLength = -1.0;
  predLength = -1.0;
}

double RichardsSuitFunc::calculate() {

  if (isZero(coeff[4])) {
    handle.logMessage(LOGWARN, "Warning in suitability - divide by zero error");
    return 1.0;
  }

  double check = 0.0;
  if (coeff[0] < 0.0 && coeff[1] < 0.0)
    check = pow(coeff[3] / (1.0 + exp(-(coeff[0] - coeff[1] * preyLength + coeff[2] * predLength))), (1.0 / coeff[4]));
  else if (coeff[0] > 0.0 && coeff[1] > 0.0)
    check = pow(coeff[3] / (1.0 + exp(-(-coeff[0] + coeff[1] * preyLength + coeff[2] * predLength))), (1.0 / coeff[4]));
  else
    check = pow(coeff[3] / (1.0 + exp(-(coeff[0] + coeff[1] * preyLength + coeff[2] * predLength))), (1.0 / coeff[4]));

  if (check != check) { //check for NaN
    handle.logMessageNaN(LOGWARN, "richards suitability function");
    return 0.0;
  }

  if (check < 0.0) {
    handle.logMessage(LOGWARN, "Warning in suitability - function outside bounds", check);
    return 0.0;
  } else if (check > 1.0) {
    handle.logMessage(LOGWARN, "Warning in suitability - function outside bounds", check);
    return 1.0;
  } else
    return check;
}

// ********************************************************
// Functions for GammaSuitFunc suitability function
// ********************************************************
GammaSuitFunc::GammaSuitFunc() : SuitFunc("GammaSuitFunc") {
  coeff.setsize(3);
  preyLength = -1.0;
}

double GammaSuitFunc::calculate() {

  if (isZero(coeff[1]) || (isZero(coeff[2])) || (isEqual(coeff[0], 1.0))) {
    handle.logMessage(LOGWARN, "Warning in suitability - divide by zero error");
    return 1.0;
  }

  double check = exp(coeff[0] - 1.0 - (preyLength / (coeff[1] * coeff[2])));
  check *= pow(preyLength / ((coeff[0] - 1.0) * coeff[1] * coeff[2]), (coeff[0] - 1.0));

  if (check != check) { //check for NaN
    handle.logMessageNaN(LOGWARN, "gamma suitability function");
    return 0.0;
  }

  if (check < 0.0) {
    handle.logMessage(LOGWARN, "Warning in suitability - function outside bounds", check);
    return 0.0;
  } else if (check > 1.0) {
    handle.logMessage(LOGWARN, "Warning in suitability - function outside bounds", check);
    return 1.0;
  } else
    return check;
}

// ********************************************************
// Functions for AndersenFleetSuitFunc suitability function
// ********************************************************
AndersenFleetSuitFunc::AndersenFleetSuitFunc() : SuitFunc("AndersenFleetSuitFunc") {
  coeff.setsize(6);
  preyLength = -1.0;
}

double AndersenFleetSuitFunc::calculate() {
  double l = log(coeff[5] / preyLength);
  double e, q, check;

  q = 0.0;
  check = 0.0;
  if (l > coeff[1])
    q = coeff[3];
  else
    q = coeff[4];

  if (isZero(q))
    q = 1.0;
  if (q < 0.0)
    q = -q;

  e = (l - coeff[1]) * (l - coeff[1]);
  check = coeff[0] + coeff[2] * exp(-e / q);
  if (check < 0.0) {
    handle.logMessage(LOGWARN, "Warning in suitability - function outside bounds", check);
    return 0.0;
  } else if (check > 1.0) {
    handle.logMessage(LOGWARN, "Warning in suitability - function outside bounds", check);
    return 1.0;
  } else
    return check;
}
