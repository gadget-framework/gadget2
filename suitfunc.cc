#include "suitfunc.h"
#include "errorhandler.h"
#include "gadget.h"

extern ErrorHandler handle;

// ********************************************************
// Functions for base suitability function
// ********************************************************
void SuitFunc::readConstants(CommentStream& infile,
  const TimeClass* const TimeInfo, Keeper* const keeper) {

  coeff.read(infile, TimeInfo, keeper);
  coeff.Update(TimeInfo);
}

SuitFunc::SuitFunc() {
  name = NULL;
}

SuitFunc::~SuitFunc() {
  if (name != NULL) {
    delete[] name;
    name = NULL;
  }
}

void SuitFunc::setPredLength(double length) {
  handle.logMessage(LOGWARN, "Warning in suitability - trying to set predator length for", this->getName());
}

void SuitFunc::setPreyLength(double length) {
  handle.logMessage(LOGWARN, "Warning in suitability - trying to set prey length for", this->getName());
}

void SuitFunc::setName(const char* suitFuncName) {
  if (name != NULL) {
    delete[] name;
    name = NULL;
  }
  name = new char[strlen(suitFuncName) + 1];
  strcpy(name, suitFuncName);
}

const char* SuitFunc::getName() {
  return name;
}

const TimeVariableVector& SuitFunc::getConstants() const {
  return coeff;
}

void SuitFunc::updateConstants(const TimeClass* const TimeInfo) {
  coeff.Update(TimeInfo);
}

int SuitFunc::didChange(const TimeClass* const TimeInfo) {
  return coeff.didChange(TimeInfo);
}

int SuitFunc::numConstants() {
  return coeff.Size();
}

// ********************************************************
// Functions for ExpSuitFuncA suitability function
// ********************************************************
ExpSuitFuncA::ExpSuitFuncA() {
  this->setName("ExponentialSuitFunc");
  coeff.setsize(4);
  preyLength = -1.0;
  predLength = -1.0;
}

ExpSuitFuncA::~ExpSuitFuncA() {
}

double ExpSuitFuncA::calculate() {
  double check = 0.0;

  if (coeff[0] < 0.0 && coeff[1] < 0.0)
    check = coeff[3] / (1.0 + exp(-(coeff[0] - coeff[1] * preyLength + coeff[2] * predLength)));
  else if (coeff[0] > 0.0 && coeff[1] > 0.0)
    check = coeff[3] / (1.0 + exp(-(-coeff[0] + coeff[1] * preyLength + coeff[2] * predLength)));
  else
    check = coeff[3] / (1.0 + exp(-(coeff[0] + coeff[1] * preyLength + coeff[2] * predLength)));

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
ConstSuitFunc::ConstSuitFunc() {
  this->setName("ConstantSuitFunc");
  coeff.setsize(1);
}

ConstSuitFunc::~ConstSuitFunc() {
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
AndersenSuitFunc::AndersenSuitFunc() {
  this->setName("AndersenSuitFunc");
  coeff.setsize(5);
  preyLength = -1.0;
  predLength = -1.0;
}

AndersenSuitFunc::~AndersenSuitFunc() {
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
ExpSuitFuncL50::ExpSuitFuncL50() {
  this->setName("ExponentialL50SuitFunc");
  coeff.setsize(2);
  preyLength = -1.0;
}

ExpSuitFuncL50::~ExpSuitFuncL50() {
}

double ExpSuitFuncL50::calculate() {
  double check = 1.0 / (1.0 + exp(-4.0 * coeff[0] * (preyLength - coeff[1])));
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
StraightSuitFunc::StraightSuitFunc() {
  this->setName("StraightLineSuitFunc");
  coeff.setsize(2);
  preyLength = -1.0;
}

StraightSuitFunc::~StraightSuitFunc() {
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
InverseSuitFunc::InverseSuitFunc() {
  this->setName("InverseSuitFunc");
  coeff.setsize(2);
  preyLength = -1.0;
}

InverseSuitFunc::~InverseSuitFunc() {
}

double InverseSuitFunc::calculate() {
  double check = 1.0 / (1.0 + exp(-4.0 * coeff[0] * (preyLength - coeff[1])));
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
StraightUnboundedSuitFunc::StraightUnboundedSuitFunc() {
  this->setName("StraightLineUnboundedSuitFunc");
  coeff.setsize(2);
  preyLength = -1.0;
}

StraightUnboundedSuitFunc::~StraightUnboundedSuitFunc() {
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
RichardsSuitFunc::RichardsSuitFunc() {
  this->setName("RichardsSuitFunc");
  coeff.setsize(5);
  preyLength = -1.0;
  predLength = -1.0;
}

RichardsSuitFunc::~RichardsSuitFunc() {
}

double RichardsSuitFunc::calculate() {
  double check = 0.0;

  if (coeff[0] < 0.0 && coeff[1] < 0.0)
    check = pow(coeff[3] / (1.0 + exp(-(coeff[0] - coeff[1] * preyLength + coeff[2] * predLength))), (1.0 / coeff[4]));
  else if (coeff[0] > 0.0 && coeff[1] > 0.0)
    check = pow(coeff[3] / (1.0 + exp(-(-coeff[0] + coeff[1] * preyLength + coeff[2] * predLength))), (1.0 / coeff[4]));
  else
    check = pow(coeff[3] / (1.0 + exp(-(coeff[0] + coeff[1] * preyLength + coeff[2] * predLength))), (1.0 / coeff[4]));

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
GammaSuitFunc::GammaSuitFunc() {
  this->setName("GammaSuitFunc");
  coeff.setsize(3);
  preyLength = -1.0;
}

GammaSuitFunc::~GammaSuitFunc() {
}

double GammaSuitFunc::calculate() {
  double check;

  check = exp(coeff[0] - 1.0 - (preyLength / coeff[1] * coeff[2]));
  check *= pow(preyLength / ((coeff[0] - 1.0) * coeff[1] * coeff[2]), (coeff[0] - 1.0));
  if (check < 0.0) {
    handle.logMessage(LOGWARN, "Warning in suitability - function outside bounds", check);
    return 0.0;
  } else if (check > 1.0) {
    handle.logMessage(LOGWARN, "Warning in suitability - function outside bounds", check);
    return 1.0;
  } else
    return check;
}
