#include "suitfunc.h"
#include "gadget.h"

int findSuitFunc(SuitFuncPtrVector& suitf, const char* suitname) {
  int found = 0;
  SuitFunc* tempFunc;

  if (strcasecmp(suitname, "expsuitfunca") == 0) {
    tempFunc = new ExpSuitFuncA();
    suitf.resize(1, tempFunc);
    found++;

  } else if (strcasecmp(suitname, "constsuitfunc") == 0) {
    tempFunc = new ConstSuitFunc();
    suitf.resize(1, tempFunc);
    found++;

  } else if (strcasecmp(suitname, "straightline") == 0) {
    tempFunc = new StraightSuitFunc();
    suitf.resize(1, tempFunc);
    found++;

  } else if (strcasecmp(suitname, "expsuitfuncl50") == 0) {
    tempFunc = new ExpSuitFuncL50();
    suitf.resize(1, tempFunc);
    found++;

  } else if (strcasecmp(suitname, "andersensuitfunc") == 0) {
    tempFunc = new AndersenSuitFunc();
    suitf.resize(1, tempFunc);
    found++;

  } else if (strcasecmp(suitname, "improvedexpsuitfunc") == 0) {
    cout << "The improvedexpsuitfunc suitability function is no longer supported\n";

  } else if (strcasecmp(suitname, "improvedandextendedexpsuitfunc") == 0) {
    cout << "The improvedandextendedexpsuitfunc suitability function is no longer supported\n";

  } else if (strcasecmp(suitname, "extendedexpsuitfuncl50") == 0) {
    cout << "The extendedexpsuitfuncl50 suitability function is no longer supported\n";

  } else if (strcasecmp(suitname, "badexpsuitfuncl50") == 0) {
    cout << "The badexpsuitfuncl50 suitability function is no longer supported\n";

  } else if (strcasecmp(suitname, "surveyselection") == 0) {
    cout << "The surveyselection suitability function is no longer supported\n";

  } else if (strcasecmp(suitname, "cloglog") == 0) {
    cout << "The cloglog suitability function is no longer supported\n";

  } else if (strcasecmp(suitname, "combination") == 0) {
    cout << "The combination suitability function is no longer supported\n";

  } else
    cout << "Warning - no valid suitability function has been found\n";

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

  coeff.Read(infile, TimeInfo, keeper);
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

double SuitFunc::getPredLength() {
  cerr << "Error - trying to get predlength for a suitability function "
    << this->getName() << " but this function does not use predlength\n";
  return -1.0;
}

double SuitFunc::getPreyLength() {
  cerr << "Error - trying to get preylength for a suitability function "
    << this->getName() << " but this function does not use preylength\n";
  return -1.0;
}

void SuitFunc::setPredLength(double length) {
  cerr << "Error - trying to set predlength for a suitability function "
    << this->getName() << " but this function does not use predlength\n";
}

void SuitFunc::setPreyLength(double length) {
  cerr << "Error - trying to set preylength for a suitability function "
    << this->getName() << " but this function does not use preylength\n";
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

int SuitFunc::noOfConstants() {
  return coeff.Size();
}

// ********************************************************
// Functions for ExpSuitFuncA suitability function
// ********************************************************
ExpSuitFuncA::ExpSuitFuncA() {
  this->setName("ExpSuitFuncA");
  coeff.resize(4);
  preyLength = -1.0;
  predLength = -1.0;
}

ExpSuitFuncA::~ExpSuitFuncA() {
}

double ExpSuitFuncA::calculate() {
  assert(coeff.Size() == 4);
  double check = 0.0;

  if (coeff[0] < 0 && coeff[1] < 0) {
    check = coeff[3] / (1 + exp(-(coeff[0] - coeff[1] *
      preyLength + coeff[2] * predLength)));

  } else if (coeff[0] > 0 && coeff[1] > 0) {
    check = coeff[3] / (1 + exp(-(-coeff[0] +  coeff[1] *
      preyLength + coeff[2] * predLength)));

  } else {
    check = coeff[3] / (1 + exp(-(coeff[0] +  coeff[1] *
      preyLength + coeff[2] * predLength)));
  }

  if ((check < 0.0) || (check > 1.0)) {
    cerr << "Error in suitability - function outside bounds " << check << endl;
    exit(EXIT_FAILURE);
  } else
    return check;
}

// ********************************************************
// Functions for ConstSuitFunc suitability function
// ********************************************************
ConstSuitFunc::ConstSuitFunc() {
  this->setName("ConstSuitFunc");
  coeff.resize(1);
}

ConstSuitFunc::~ConstSuitFunc() {
}

double ConstSuitFunc::calculate() {
  assert(coeff.Size() == 1);
  if ((coeff[0] < 0.0) || (coeff[0] > 1.0)) {
    cerr << "Error in suitability - function outside bounds " << coeff[0] << endl;
    exit(EXIT_FAILURE);
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
  assert(coeff.Size() == 5);

  q = 0.0;
  check = 0.0;
  if (l > coeff[1])
    q = coeff[3];
  else
    q = coeff[4];

  if (isZero(q))
    q = 1.0;   //We do not want to divide with 0, no matter what.
  if (q < 0)
    q = -q;    //To avoid getting a big positive number as an argument for exp

  e = (l - coeff[1]) * (l - coeff[1]);
  check = coeff[0] + coeff[2] * exp(-e / q);
  if ((check < 0.0) || (check > 1.0)) {
    cerr << "Error in suitability - function outside bounds " << check << endl;
    exit(EXIT_FAILURE);
  } else
    return check;
}

// ********************************************************
// Functions for ExpSuitFuncL50 suitability function
// ********************************************************
ExpSuitFuncL50::ExpSuitFuncL50() {
  this->setName("ExpSuitFuncL50");
  coeff.resize(2);
  preyLength = -1.0;
}

ExpSuitFuncL50::~ExpSuitFuncL50() {
}

double ExpSuitFuncL50::calculate() {
  assert(coeff.Size() == 2);
  double check = 0.0;

  /* Parameter [0] and [1] got to be greater then 0 because of the l_50 form.
   * Modified by kgf 21/8 00 and mna 04.10.00
   * Predlength is not taken into account. */
  check = 1.0 / (1 + exp(-4.0 * coeff[0] * (preyLength - coeff[1])));
  if ((check < 0.0) || (check > 1.0)) {
    cerr << "Error in suitability - function outside bounds " << check << endl;
    exit(EXIT_FAILURE);
  } else
    return check;
}

// ********************************************************
// Functions for StraightSuitFunc suitability function
// ********************************************************
StraightSuitFunc::StraightSuitFunc() {
  this->setName("StraightSuitFunc");
  coeff.resize(2);
  preyLength = -1.0;
}

StraightSuitFunc::~StraightSuitFunc() {
}

double StraightSuitFunc::calculate() {
  assert(coeff.Size() == 2);
  double check = 0.0;
  /* Line as a function of preylength only, predlength is dummy
   * written by kgf 5/3 01
   * JMB - fixed for values less than zero */
  check = coeff[0] * preyLength + coeff[1];
  if (check < 0.0)
    return 0.0;
  else
    return check;
}
