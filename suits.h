#ifndef suits_h
#define suits_h

#include "charptrvector.h"
#include "suitfuncptrvector.h"
#include "bandmatrixptrvector.h"
#include "keeper.h"

class Predator;

class Suits {
public:
  /**
   * \brief This is the default Suits constructor
   */
  Suits();
  /**
   * \brief This is the default Suits destructor
   */
  ~Suits();
  void addPrey(const char* preyname, SuitFunc* suitf);
  void deletePrey(int i, Keeper* const keeper);
  const char* getPreyName(int i) const { return preynames[i]; };
  int numPreys() const { return preynames.Size(); };
  const BandMatrix& Suitable(int i) const { return preCalcSuitability[i]; };
  /**
   * \brief This function will reset the suitability information for a given predator
   * \param pred is the Predator that the suitability functions are defined for
   * \param TimeInfo is the TimeClass for the current model
   */
  void Reset(const Predator* const pred, const TimeClass* const TimeInfo);
  int DidChange(int i, const TimeClass* const TimeInfo) const;
protected:
  /**
   * \brief This is the CharPtrVector of prey names
   */
  CharPtrVector preynames;
  /**
   * \brief This is the SuitFuncPtrVector of the suitability functions used when calculating the consumption of the preys
   */
  SuitFuncPtrVector suitFunction;
  /**
   * \brief This is the BandMatrixPtrVector of calculated suitability values
   */
  BandMatrixPtrVector preCalcSuitability;
};

#endif
