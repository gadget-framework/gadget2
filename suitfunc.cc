#include "suitfunc.h"
#include "gadget.h"

//JMB - should remove some of the suitability functions that are no longer used
int findSuitFunc(SuitfuncPtrvector& suitf, const char* suitname) {
  int found = 0;
  SuitFunc* tempFunc;

  if (strcasecmp(suitname, "ExpsuitfuncA") == 0) {
    tempFunc = new ExpsuitfuncA();
    suitf.resize(1, tempFunc);
    found = 1;

  } else if (strcasecmp(suitname, "Constsuitfunc") == 0) {
    tempFunc = new Constsuitfunc();
    suitf.resize(1, tempFunc);
    found = 1;

  } else if (strcasecmp(suitname, "Andersensuitfunc") == 0) {
    tempFunc = new Andersensuitfunc();
    suitf.resize(1, tempFunc);
    found = 1;

  } else if (strcasecmp(suitname, "ImprovedExpsuitfunc") == 0) {
    tempFunc = new ImprovedExpsuitfunc();
    suitf.resize(1, tempFunc);
    found = 1;

  } else if (strcasecmp(suitname, "ImprovedAndExtendedExpsuitfunc") == 0) {
    tempFunc = new ImprovedAndExtendedExpsuitfunc();
    suitf.resize(1, tempFunc);
    found = 1;

  } else if (strcasecmp(suitname, "Expsuitfuncl50") == 0) {
    tempFunc = new Expsuitfuncl50();
    suitf.resize(1, tempFunc);
    found = 1;

  } else if (strcasecmp(suitname, "ExtendedExpsuitfuncl50") == 0) {
    tempFunc = new ExtendedExpsuitfuncl50();
    suitf.resize(1, tempFunc);
    found = 1;

  } else if (strcasecmp(suitname, "BadExpsuitfuncl50") == 0) {
    tempFunc = new BadExpsuitfuncl50();
    suitf.resize(1, tempFunc);
    found = 1;

  } else if (strcasecmp(suitname, "SurveySelection") == 0) {
    tempFunc = new SurveySelection();
    suitf.resize(1, tempFunc);
    found = 1;

  } else if (strcasecmp(suitname, "StraightLine") == 0) {
    tempFunc = new StraightLine();
    suitf.resize(1, tempFunc);
    found = 1;

  } else if (strcasecmp(suitname, "CLogLog") == 0) {
    tempFunc = new CLogLog();
    suitf.resize(1, tempFunc);
    found = 1;

  } else if (strcasecmp(suitname, "Combination") == 0) {
    tempFunc = new Combination();
    suitf.resize(1, tempFunc);
    found = 1;

  } else
    found = 0;

  return found;
}

int readSuitFunction(SuitfuncPtrvector& suitf, CommentStream& infile,
  const char* suitname, const TimeClass* const TimeInfo, Keeper* const keeper) {

  if (findSuitFunc(suitf, suitname) == 1) {
    suitf[suitf.Size() - 1]->readConstants(infile, TimeInfo, keeper);
    return 1;
  } else
    return 0;
}

void SuitFunc::readConstants(CommentStream& infile,
  const TimeClass* const TimeInfo, Keeper* const keeper) {

  coeff.Read(infile, TimeInfo, keeper);
  coeff.Update(TimeInfo);
}

SuitFunc::SuitFunc() {
  name = NULL;
}

SuitFunc::~SuitFunc() {
  if (name != NULL)
    delete name;
}

double SuitFunc::getPredLength() {
  cout << "Error - trying to get predlength for a suitability function "
    << this->getName() << " but this function does not use predlength\n";
  return -1.0;
}

double SuitFunc::getPreyLength() {
  cout << "Error - trying to get preylength for a suitability function "
    << this->getName() << " but this function does not use preylength\n";
  return -1.0;
}

void SuitFunc::setPredLength(double length) {
  cout << "Error - trying to set predlength for a suitability function "
    << this->getName() << " but this function does not use predlength\n";
}

void SuitFunc::setPreyLength(double length) {
  cout << "Error - trying to set preylength for a suitability function "
    << this->getName() << " but this function does not use preylength\n";
}

void SuitFunc::setName(const char* suitFuncName) {
  if (name != NULL) {
    delete name;
    name = NULL;
  }
  int len = strlen(suitFuncName);
  name = new char[len + 1];
  strcpy(name, suitFuncName);
}

const char* SuitFunc::getName() {
   return name;
}

const TimeVariablevector& SuitFunc::getConstants() const {
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

ExpsuitfuncA::ExpsuitfuncA() {
  this->setName("ExpSuitFuncA");
  coeff.resize(4);
  preyLength = -1.0;
  predLength = -1.0;
}

ExpsuitfuncA::~ExpsuitfuncA() {
}

int ExpsuitfuncA::usesPredLength() {
  return 1;
}

int ExpsuitfuncA::usesPreyLength() {
  return 1;
}

void ExpsuitfuncA::setPredLength(double length) {
  predLength = length;
}

void ExpsuitfuncA::setPreyLength(double length) {
  preyLength = length;
}

double ExpsuitfuncA::getPredLength() {
  return predLength;
}

double ExpsuitfuncA::getPreyLength() {
  return preyLength;
}

double ExpsuitfuncA::calculate() {
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

Constsuitfunc::Constsuitfunc() {
  this->setName("ConstSuitFunc");
  coeff.resize(1);
}

Constsuitfunc::~Constsuitfunc() {
}

int Constsuitfunc::usesPredLength() {
  return 0;
}

int Constsuitfunc::usesPreyLength() {
  return 0;
}

double Constsuitfunc::calculate() {
  assert(coeff.Size() == 1);
  if ((coeff[0] < 0.0) || (coeff[0] > 1.0)) {
    cerr << "Error in suitability - function outside bounds " << coeff[0] << endl;
    exit(EXIT_FAILURE);
  } else
    return coeff[0];
}

Andersensuitfunc::Andersensuitfunc() {
  this->setName("AndersenSuitFunc");
  coeff.resize(5);
  preyLength = -1.0;
  predLength = -1.0;
}

Andersensuitfunc::~Andersensuitfunc() {
}

int Andersensuitfunc::usesPredLength() {
  return 1;
}

int Andersensuitfunc::usesPreyLength() {
  return 1;
}

void Andersensuitfunc::setPredLength(double length) {
  predLength = length;
}

void Andersensuitfunc::setPreyLength(double length) {
  preyLength = length;
}

double Andersensuitfunc::getPredLength() {
  return predLength;
}

double Andersensuitfunc::getPreyLength() {
  return preyLength;
}

double Andersensuitfunc::calculate() {
  double l = log(predLength / preyLength);
  double e, q, check;
  assert(coeff.Size() == 5);

  q = 0.0;
  check = 0.0;
  if (l > coeff[1])
    q = coeff[3];
  else
    q = coeff[4];

  if (iszero(q))
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

ImprovedExpsuitfunc::ImprovedExpsuitfunc() {
  this->setName("ImprovedExpSuitFunc");
  coeff.resize(4);
  preyLength = -1.0;
  predLength = -1.0;
}

ImprovedExpsuitfunc::~ImprovedExpsuitfunc() {
}

int ImprovedExpsuitfunc::usesPredLength() {
  return 1;
}

int ImprovedExpsuitfunc::usesPreyLength() {
  return 1;
}

void ImprovedExpsuitfunc::setPredLength(double length) {
  predLength = length;
}

void ImprovedExpsuitfunc::setPreyLength(double length) {
  preyLength = length;
}

double ImprovedExpsuitfunc::getPredLength() {
  return predLength;
}

double ImprovedExpsuitfunc::getPreyLength() {
  return preyLength;
}

double ImprovedExpsuitfunc::calculate() {
  assert(coeff.Size() == 4);
  assert(predLength != 0);
  int i;
  double rell = preyLength / predLength;
  doublevector par(coeff.Size());
  for (i = 0; i < par.Size(); i++)
    par[i] = coeff[i];
  for (i = 0; i < 2; i++)
    par[i] = absolute(par[i]);
  return par[3] + par[2] / (1 + exp(-(par[0] * (rell - par[1]))));
}

ImprovedAndExtendedExpsuitfunc::ImprovedAndExtendedExpsuitfunc() {
  this->setName("ImprovedAndExtendedExpSuitFunc");
  coeff.resize(6);
  preyLength = -1.0;
  predLength = -1.0;
}

ImprovedAndExtendedExpsuitfunc::~ImprovedAndExtendedExpsuitfunc() {
}

int ImprovedAndExtendedExpsuitfunc::usesPredLength() {
  return 1;
}

int ImprovedAndExtendedExpsuitfunc::usesPreyLength() {
  return 1;
}

void ImprovedAndExtendedExpsuitfunc::setPredLength(double length) {
  predLength = length;
}

void ImprovedAndExtendedExpsuitfunc::setPreyLength(double length) {
  preyLength = length;
}

double ImprovedAndExtendedExpsuitfunc::getPredLength() {
  return predLength;
}

double ImprovedAndExtendedExpsuitfunc::getPreyLength() {
  return preyLength;
}

double ImprovedAndExtendedExpsuitfunc::calculate() {
  assert(predLength != 0);
  assert(coeff.Size() == 6);
  int i;
  double rell = preyLength / predLength;
  doublevector par(coeff.Size());
  for (i = 0; i < par.Size(); i++)
    par[i] = coeff[i];
  for (i = 0; i < 6; i++)
    par[i] = absolute(par[i]);
  return par[5] + par[2] / (1 + exp(-(par[0] * (rell - par[1]) -
    par[3] * (rell - par[4]))));
}

Expsuitfuncl50::Expsuitfuncl50() {
  this->setName("ExpSuitFuncL50");
  coeff.resize(2);
  preyLength = -1.0;
}

Expsuitfuncl50::~Expsuitfuncl50() {
}

int Expsuitfuncl50::usesPredLength() {
  return 0;
}

int Expsuitfuncl50::usesPreyLength() {
  return 1;
}

void Expsuitfuncl50::setPreyLength(double length) {
  preyLength = length;
}

double Expsuitfuncl50::getPreyLength() {
  return preyLength;
}

double Expsuitfuncl50::calculate() {
  assert(coeff.Size() == 2);
  assert(coeff[0] > 0 && coeff[1] > 0);
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

ExtendedExpsuitfuncl50::ExtendedExpsuitfuncl50() {
  this->setName("ExtendedExpSuitFuncL50");
  coeff.resize(3);
  preyLength = -1.0;
}

ExtendedExpsuitfuncl50::~ExtendedExpsuitfuncl50() {
}

int ExtendedExpsuitfuncl50::usesPredLength() {
  return 0;
}

int ExtendedExpsuitfuncl50::usesPreyLength() {
  return 1;
}

void ExtendedExpsuitfuncl50::setPreyLength(double length) {
  preyLength = length;
}

double ExtendedExpsuitfuncl50::getPreyLength() {
  return preyLength;
}

double ExtendedExpsuitfuncl50::calculate() {
  assert(coeff.Size() == 3);
  assert(coeff[0] > 0 && coeff[1] > 0 && coeff[2] > 0);

  /* Parameters: alpha1, alpha2, l50
   * Parameter [0], [1], and [2] got to be greater then 0 because of
   * the l_50 form.  [mna 04.10.00]
   * Predlength is not taken into account. */
  if (preyLength > coeff[2]) //if l>l50
    return 1.0 / (1 + exp(-4.0 * coeff[1] * (preyLength - coeff[2])));
  else
    return 1.0 / (1 + exp(-4.0 * coeff[0] * (preyLength - coeff[2])));
}

BadExpsuitfuncl50::BadExpsuitfuncl50() {
  this->setName("BadExpSuitFuncL50");
  coeff.resize(2);
  preyLength = -1.0;
}

BadExpsuitfuncl50::~BadExpsuitfuncl50() {
}

int BadExpsuitfuncl50::usesPredLength() {
  return 0;
}

int BadExpsuitfuncl50::usesPreyLength() {
  return 1;
}

void BadExpsuitfuncl50::setPreyLength(double length) {
  preyLength = length;
}

double BadExpsuitfuncl50::getPreyLength() {
  return preyLength;
}

double BadExpsuitfuncl50::calculate() {
  assert(coeff.Size() == 2);
  assert(coeff[0] > 0 && coeff[1] > 0);

  /* This function was entered only to reproduce the result from the
   * AFWG 2000 (mna 14.11.00)
   * Parameter [0] and [1] got to be greater then 0 because of the l_50 form.
   * Modified by kgf 21/8 00 and mna 04.10.00
   * Predlength is not taken into account. */
  if (preyLength > coeff[1]) //if l>l50
    return 1.0 / (1 + exp(-6.0 * coeff[0] * (preyLength - coeff[1])));
  else
    return 1.0 / (1 + exp(-4.0 * coeff[0] * (preyLength - coeff[1])));
}

SurveySelection::SurveySelection() {
  this->setName("SurveySelection");
  coeff.resize(2);
  preyLength = -1.0;
}

SurveySelection::~SurveySelection() {
}

int SurveySelection::usesPredLength() {
  return 0;
}

int SurveySelection::usesPreyLength() {
  return 1;
}

void SurveySelection::setPreyLength(double length) {
  preyLength = length;
}

double SurveySelection::getPreyLength() {
  return preyLength;
}

double SurveySelection::calculate() {
  assert(coeff.Size() == 2);
  assert(coeff[0] > 0 && coeff[1] > 0);
  //Predlength is not taken into account.
  return coeff[0] * exp(coeff[1] * preyLength);
}

StraightLine::StraightLine() {
  this->setName("StraightLine");
  coeff.resize(2);
  preyLength = -1.0;
}

StraightLine::~StraightLine() {
}

int StraightLine::usesPredLength() {
  return 0;
}

int StraightLine::usesPreyLength() {
  return 1;
}

void StraightLine::setPreyLength(double length) {
  preyLength = length;
}

double StraightLine::getPreyLength() {
  return preyLength;
}

double StraightLine::calculate() {
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

CLogLog::CLogLog() {
  this->setName("CLogLog");
  coeff.resize(2);
  preyLength = -1.0;
}

CLogLog::~CLogLog() {
}

int CLogLog::usesPredLength() {
  return 0;
}

int CLogLog::usesPreyLength() {
  return 1;
}

void CLogLog::setPreyLength(double length) {
  preyLength = length;
}

double CLogLog::getPreyLength() {
  return preyLength;
}

double CLogLog::calculate() {
  //Written by kgf 8/3 01
  //Predlength is not taken into account.
  assert(coeff.Size() == 2);
  double arg = exp(coeff[0] + coeff[1] * preyLength);
  return(1.0 - exp(-arg));
}

Combination::Combination() {
  this->setName("Combination");
  coeff.resize(4);
  preyLength = -1.0;
}

Combination::~Combination() {
}

int Combination::usesPredLength() {
  return 0;
}

int Combination::usesPreyLength() {
  return 1;
}

void Combination::setPreyLength(double length) {
  preyLength = length;
}

double Combination::getPreyLength() {
  return preyLength;
}

double Combination::calculate() {
  //Written by kgf 8/3 01
  assert(coeff.Size() == 4);
  double arg, cloglog, logistic;
  arg = exp(coeff[2] + coeff[3] * preyLength);
  cloglog = 1.0 - exp(-arg);
  logistic = 1.0 / (1 + exp(-coeff[0] - coeff[1] * preyLength));
  return(logistic * cloglog);
}
