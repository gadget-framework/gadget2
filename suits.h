#ifndef suits_h
#define suits_h

#include "charptrvector.h"
#include "suitfuncptrvector.h"
#include "doublematrixptrvector.h"
#include "bandmatrix.h"
#include "keeper.h"

class Predator;

class Suits {
public:
  Suits();
  ~Suits();
  void addPrey(const char* preyname, SuitFunc* suitf);
  void DeletePrey(int prey, Keeper* const keeper);
  const char* Preyname(int prey) const;
  int numPreys() const;
  const BandMatrix& Suitable(int prey) const;
  void Reset(const Predator* const pred, const TimeClass* const TimeInfo);
  int DidChange(int prey, const TimeClass* const TimeInfo) const;
  SuitFunc* FuncPrey(int prey);
protected:
  CharPtrVector preynames;
  SuitFuncPtrVector SuitFunction;
  BandMatrixVector preCalcSuitability;
};

#endif
