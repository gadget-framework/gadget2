#include "suitfunc.h"
#include "errorhandler.h"
#include "gadget.h"

extern ErrorHandler handle;

int findSuitFunc(SuitFuncPtrVector& suitf, const char* suitname) {
  int found = 0;
  SuitFunc* tempFunc;

  if ((strcasecmp(suitname, "expsuitfunca") == 0) || (strcasecmp(suitname, "exponential") == 0)) {
    tempFunc = new ExpSuitFuncA();
    suitf.resize(1, tempFunc);
    found++;

  } else if ((strcasecmp(suitname, "constsuitfunc") == 0) || (strcasecmp(suitname, "constant") == 0)) {
    tempFunc = new ConstSuitFunc();
    suitf.resize(1, tempFunc);
    found++;

  } else if (strcasecmp(suitname, "straightline") == 0) {
    tempFunc = new StraightSuitFunc();
    suitf.resize(1, tempFunc);
    found++;

  } else if (strcasecmp(suitname, "straightlineunbounded") == 0) {
    tempFunc = new StraightUnboundedSuitFunc();
    suitf.resize(1, tempFunc);
    found++;

  } else if ((strcasecmp(suitname, "expsuitfuncl50") == 0) || (strcasecmp(suitname, "exponentiall50") == 0)) {
    tempFunc = new ExpSuitFuncL50();
    suitf.resize(1, tempFunc);
    found++;

  } else if ((strcasecmp(suitname, "andersensuitfunc") == 0) || (strcasecmp(suitname, "andersen") == 0)) {
    tempFunc = new AndersenSuitFunc();
    suitf.resize(1, tempFunc);
    found++;

  } else if (strcasecmp(suitname, "inverse") == 0) {
    handle.Warning("The inverse suitability function is not yet implemented");

  } else if (strcasecmp(suitname, "improvedexpsuitfunc") == 0) {
    handle.Warning("The improvedexpsuitfunc suitability function is no longer supported");

  } else if (strcasecmp(suitname, "improvedandextendedexpsuitfunc") == 0) {
    handle.Warning("The improvedandextendedexpsuitfunc suitability function is no longer supported");

  } else if (strcasecmp(suitname, "extendedexpsuitfuncl50") == 0) {
    handle.Warning("The extendedexpsuitfuncl50 suitability function is no longer supported");

  } else if (strcasecmp(suitname, "badexpsuitfuncl50") == 0) {
    handle.Warning("The badexpsuitfuncl50 suitability function is no longer supported");

  } else if (strcasecmp(suitname, "surveyselection") == 0) {
    handle.Warning("The surveyselection suitability function is no longer supported");

  } else if (strcasecmp(suitname, "cloglog") == 0) {
    handle.Warning("The cloglog suitability function is no longer supported");

  } else if (strcasecmp(suitname, "combination") == 0) {
    handle.Warning("The combination suitability function is no longer supported");

  } else
    handle.Message("Error in suitability function - no valid suitability function found");

  return found;
}

int readSuitFunction(SuitFuncPtrVector& suitf, CommentStream& infile,
  const char* suitname, const TimeClass* const TimeInfo, Keeper* const keeper) {

  if (findSuitFunc(suitf, suitname) == 1) {
    suitf[suitf.Size() - 1]->readConstants(infile, TimeInfo, keeper);
    return 1;
  } else
    return 0;
}

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
  handle.logWarning("Warning in suitability - trying to set predator length for", this->getName());
}

void SuitFunc::setPreyLength(double length) {
  handle.logWarning("Warning in suitability - trying to set prey length for", this->getName());
}

void SuitFunc::setName(const char* suitFuncName) {
  if (name != NULL) {
    delete[] name;
    name = NULL;
  }
  int len = strlen(suitFuncName);
  name = new char[len + 1];
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

int SuitFunc::constantsHaveChanged(const TimeClass* const TimeInfo) {
  return coeff.DidChange(TimeInfo);
}

int SuitFunc::numConstants() {
  return coeff.Size();
}

// ********************************************************
// Functions for ExpSuitFuncA suitability function
// ********************************************************
ExpSuitFuncA::ExpSuitFuncA() {
  this->setName("ExponentialSuitFunc");
  coeff.resize(4);
  preyLength = -1.0;
  predLength = -1.0;
}

ExpSuitFuncA::~ExpSuitFuncA() {
}

double ExpSuitFuncA::calculate() {
  double check = 0.0;

  if (coeff[0] < 0 && coeff[1] < 0)
    check = coeff[3] / (1 + exp(-(coeff[0] - coeff[1] * preyLength + coeff[2] * predLength)));
  else if (coeff[0] > 0 && coeff[1] > 0)
    check = coeff[3] / (1 + exp(-(-coeff[0] + coeff[1] * preyLength + coeff[2] * predLength)));
  else
    check = coeff[3] / (1 + exp(-(coeff[0] + coeff[1] * preyLength + coeff[2] * predLength)));

  if (check < 0.0) {
    handle.logWarning("Warning in suitability - function outside bounds", check);
    return 0.0;
  } else if (check > 1.0) {
    handle.logWarning("Warning in suitability - function outside bounds", check);
    return 1.0;
  } else
    return check;
}

// ********************************************************
// Functions for ConstSuitFunc suitability function
// ********************************************************
ConstSuitFunc::ConstSuitFunc() {
  this->setName("ConstantSuitFunc");
  coeff.resize(1);
}

ConstSuitFunc::~ConstSuitFunc() {
}

double ConstSuitFunc::calculate() {
  if (coeff[0] < 0.0) {
    handle.logWarning("Warning in suitability - function outside bounds", coeff[0]);
    return 0.0;
  } else if (coeff[0] > 1.0) {
    handle.logWarning("Warning in suitability - function outside bounds", coeff[0]);
    return 1.0;
  } else
    return coeff[0];
}

// ********************************************************
// Functions for AndersenSuitFunc suitability function
// ********************************************************
AndersenSuitFunc::AndersenSuitFunc() {
  this->setName("AndersenSuitFunc");
  coeff.resize(5);
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
  if (q < 0)
    q = -q;

  e = (l - coeff[1]) * (l - coeff[1]);
  check = coeff[0] + coeff[2] * exp(-e / q);
  if (check < 0.0) {
    handle.logWarning("Warning in suitability - function outside bounds", check);
    return 0.0;
  } else if (check > 1.0) {
    handle.logWarning("Warning in suitability - function outside bounds", check);
    return 1.0;
  } else
    return check;
}

// ********************************************************
// Functions for ExpSuitFuncL50 suitability function
// ********************************************************
ExpSuitFuncL50::ExpSuitFuncL50() {
  this->setName("ExponentialL50SuitFunc");
  coeff.resize(2);
  preyLength = -1.0;
}

ExpSuitFuncL50::~ExpSuitFuncL50() {
}

double ExpSuitFuncL50::calculate() {
  double check = 1.0 / (1 + exp(-4.0 * coeff[0] * (preyLength - coeff[1])));
  if (check < 0.0) {
    handle.logWarning("Warning in suitability - function outside bounds", check);
    return 0.0;
  } else if (check > 1.0) {
    handle.logWarning("Warning in suitability - function outside bounds", check);
    return 1.0;
  } else
    return check;
}

// ********************************************************
// Functions for StraightSuitFunc suitability function
// ********************************************************
StraightSuitFunc::StraightSuitFunc() {
  this->setName("StraightLineSuitFunc");
  coeff.resize(2);
  preyLength = -1.0;
}

StraightSuitFunc::~StraightSuitFunc() {
}

double StraightSuitFunc::calculate() {
  double check = coeff[0] * preyLength + coeff[1];
  if (check < 0.0) {
    handle.logWarning("Warning in suitability - function outside bounds", check);
    return 0.0;
  } else if (check > 1.0) {
    handle.logWarning("Warning in suitability - function outside bounds", check);
    return 1.0;
  } else
    return check;
}

// ********************************************************
// Functions for InverseSuitFunc suitability function
// ********************************************************
InverseSuitFunc::InverseSuitFunc() {
  this->setName("InverseSuitFunc");
  coeff.resize(2);
  preyLength = -1.0;
}

InverseSuitFunc::~InverseSuitFunc() {
}

double InverseSuitFunc::calculate() {
  double check = 1.0 / (1 + exp(-4.0 * coeff[0] * (preyLength - coeff[1])));
  if (check < 0.0) {
    handle.logWarning("Warning in suitability - function outside bounds", check);
    return 0.0;
  } else if (check > 1.0) {
    handle.logWarning("Warning in suitability - function outside bounds", check);
    return 1.0;
  } else
    return check;
}

// ********************************************************
// Functions for StraightUnboundedSuitFunc suitability function
// ********************************************************
StraightUnboundedSuitFunc::StraightUnboundedSuitFunc() {
  this->setName("StraightLineUnboundedSuitFunc");
  coeff.resize(2);
  preyLength = -1.0;
}

StraightUnboundedSuitFunc::~StraightUnboundedSuitFunc() {
}

double StraightUnboundedSuitFunc::calculate() {
  double check = coeff[0] * preyLength + coeff[1];
  if (check < 0.0) {
    handle.logWarning("Warning in suitability - function outside bounds", check);
    return 0.0;
  /*JMB this has been removed to create the 'unbounded' part of the suitability function
  } else if (check > 1.0) {
    handle.logWarning("Warning in suitability - function outside bounds", check);
    return 1.0; */
  } else
    return check;
}
