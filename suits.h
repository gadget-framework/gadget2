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
  void deletePrey(int prey, Keeper* const keeper);
  const char* Preyname(int prey) const { return preynames[prey]; };
  int numPreys() const { return suitFunction.Size(); };
  const BandMatrix& Suitable(int prey) const { return preCalcSuitability[prey]; };
  void Reset(const Predator* const pred, const TimeClass* const TimeInfo);
  int DidChange(int prey, const TimeClass* const TimeInfo) const;
protected:
  CharPtrVector preynames;
  SuitFuncPtrVector suitFunction;
  BandMatrixVector preCalcSuitability;
};

#endif
