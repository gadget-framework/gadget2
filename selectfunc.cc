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

SelectFunc::SelectFunc() {
  name = NULL;
}

SelectFunc::~SelectFunc() {
  if (name != NULL) {
    delete[] name;
    name = NULL;
  }
}

void SelectFunc::setName(const char* selectname) {
  if (name != NULL) {
    delete[] name;
    name = NULL;
  }

  name = new char[strlen(selectname) + 1];
  strcpy(name, selectname);
}

const char* SelectFunc::getName() {
  return name;
}

const TimeVariableVector& SelectFunc::getConstants() const {
  return coeff;
}

void SelectFunc::updateConstants(const TimeClass* const TimeInfo) {
  coeff.Update(TimeInfo);
}

int SelectFunc::constantsHaveChanged(const TimeClass* const TimeInfo) {
  return coeff.DidChange(TimeInfo);
}

int SelectFunc::numConstants() {
  return coeff.Size();
}

// ********************************************************
// Functions for ConstSelectFunc selection function
// ********************************************************
ConstSelectFunc::ConstSelectFunc() {
  this->setName("ConstantSelectFunc");
  coeff.resize(1);
}

ConstSelectFunc::~ConstSelectFunc() {
}

double ConstSelectFunc::calculate(double len) {
  return coeff[0];
}

// ********************************************************
// Functions for ExpSelectFunc selection function
// ********************************************************
ExpSelectFunc::ExpSelectFunc() {
  this->setName("ExponentialSelectFunc");
  coeff.resize(2);
}

ExpSelectFunc::~ExpSelectFunc() {
}

double ExpSelectFunc::calculate(double len) {
  return (1.0 / (1 + exp(coeff[0] * (len - coeff[1]))));
}

// ********************************************************
// Functions for StraightSelectFunc selection function
// ********************************************************
StraightSelectFunc::StraightSelectFunc() {
  this->setName("StraightLineSelectFunc");
  coeff.resize(2);
}

StraightSelectFunc::~StraightSelectFunc() {
}

double StraightSelectFunc::calculate(double len) {
  return (coeff[0] * len + coeff[1]);
}
