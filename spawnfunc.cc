#include "spawnfunc.h"
#include "gadget.h"

// ********************************************************
// Functions for base spawning function
// ********************************************************
void SpawnFunc::readConstants(CommentStream& infile,
  const TimeClass* const TimeInfo, Keeper* const keeper) {

  coeff.Read(infile, TimeInfo, keeper);
  coeff.Update(TimeInfo);
}

SpawnFunc::SpawnFunc() {
  name = NULL;
}

SpawnFunc::~SpawnFunc() {
  if (name != NULL) {
    delete[] name;
    name = NULL;
  }
}

void SpawnFunc::setName(const char* spawnname) {
  if (name != NULL) {
    delete[] name;
    name = NULL;
  }

  name = new char[strlen(spawnname) + 1];
  strcpy(name, spawnname);
}

const char* SpawnFunc::getName() {
  return name;
}

const TimeVariableVector& SpawnFunc::getConstants() const {
  return coeff;
}

void SpawnFunc::updateConstants(const TimeClass* const TimeInfo) {
  coeff.Update(TimeInfo);
}

int SpawnFunc::constantsHaveChanged(const TimeClass* const TimeInfo) {
  return coeff.DidChange(TimeInfo);
}

int SpawnFunc::noOfConstants() {
  return coeff.Size();
}

// ********************************************************
// Functions for ConstSpawnFunc spawning function
// ********************************************************
ConstSpawnFunc::ConstSpawnFunc() {
  this->setName("ConstSpawnFunc");
  coeff.resize(1);
}

ConstSpawnFunc::~ConstSpawnFunc() {
}

double ConstSpawnFunc::calculate(double len) {
  assert(coeff.Size() == 1);
  return coeff[0];
}

// ********************************************************
// Functions for ExpSpawnFunc spawning function
// ********************************************************
ExpSpawnFunc::ExpSpawnFunc() {
  this->setName("ExpSpawnFunc");
  coeff.resize(2);
}

ExpSpawnFunc::~ExpSpawnFunc() {
}

double ExpSpawnFunc::calculate(double len) {
  assert(coeff.Size() == 2);
  return (1.0 / (1 + exp(coeff[0] * (len - coeff[1]))));
}

// ********************************************************
// Functions for StraightSpawnFunc spawning function
// ********************************************************
StraightSpawnFunc::StraightSpawnFunc() {
  this->setName("StraightSpawnFunc");
  coeff.resize(2);
}

StraightSpawnFunc::~StraightSpawnFunc() {
}

double StraightSpawnFunc::calculate(double len) {
  assert(coeff.Size() == 2);
  return (coeff[0] * len + coeff[1]);
}
