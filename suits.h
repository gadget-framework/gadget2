#ifndef suits_h
#define suits_h

#include "charptrvector.h"
#include "suitfuncptrvector.h"
#include "doublematrixptrvector.h"
#include "bandmatrix.h"
#include "timevariablematrix.h"

class Keeper;

class Suits {
public:
  Suits();
  ~Suits();
  Suits(const Suits& initial, Keeper* const keeper);
  void AddPrey(const char* preyname, SuitFunc* suitf);
  void AddPrey(const char* preyname, double multiplication,
    const doublematrix& suitabilities, Keeper* const keeper);
  void DeletePrey(int prey, Keeper* const keeper);
  const char* Preyname(int prey) const;
  int NoFuncPreys() const;
  int NoPreys() const;
  const bandmatrix& Suitable(int prey) const;
  void Reset(const class Predator* const, const TimeClass* const TimeInfo);
  int DidChange(int prey, const TimeClass* const TimeInfo) const;
  SuitFunc* FuncPrey(int prey);
protected:
  void DeleteFuncPrey(int prey, Keeper* const keeper);
  void DeleteMatrixPrey(int prey, Keeper* const keeper);
  charptrvector FuncPreynames;
  charptrvector MatrixPreynames;
  doublevector Multiplication;
  SuitfuncPtrvector SuitFunction;
  doublematrixptrvector MatrixSuit;
  bandmatrixvector PrecalcSuitability;
};

#endif
