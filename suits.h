#ifndef suits_h
#define suits_h

#include "charptrvector.h"
#include "suitfuncptrvector.h"
#include "doublematrixptrvector.h"
#include "keeper.h"

class Predator;

/**
 * \class Suits
 * \brief This is the class used to calculate and store the suitability values for all the preys of a single predator
 */
class Suits {
public:
  /**
   * \brief This is the default Suits constructor
   */
  Suits() {};
  /**
   * \brief This is the default Suits destructor
   */
  ~Suits();
  /**
   * \brief This function will add suitability values for a given prey
   * \param preyname is the name of the prey to be added
   * \param suitf is the SuitFunc that defines the suitability values
   */
  void addPrey(const char* preyname, SuitFunc* suitf);
  /**
   * \brief This function will delete suitability values for a given prey
   * \param i is the index of the prey to be deleted
   * \param keeper is the Keeper for the current model
   */
  void deletePrey(int i, Keeper* const keeper);
  /**
   * \brief This function will return the name of a given prey
   * \param i is the index of the prey
   * \return name of the prey
   */
  const char* getPreyName(int i) const { return preynames[i]; };
  /**
   * \brief This will initialise the preys that will be consumed by the predator
   * \param pred is the Predator that the suitability functions are defined for
   */
  void Initialise(const Predator* const pred);
  /**
   * \brief This function will return the number of preys that have suitability values
   * \return number of preys
   */
  int numPreys() const { return preynames.Size(); };
  /**
   * \brief This function will check to see if the suitability parameters for a given prey have changed on the current timestep
   * \param i is the index of the prey
   * \param TimeInfo is the TimeClass for the current model
   * \return 1 if the values have changed, 0 otherwise
   */
  int didChange(int i, const TimeClass* const TimeInfo) { return suitFunction[i]->didChange(TimeInfo); };
  /**
   * \brief This function will return the calculated suitability values for a given prey
   * \param i is the index of the prey
   * \return preCalcSuitability[i], the DoubleMatrix containing the suitability values
   */
  const DoubleMatrix& getSuitability(int i) const { return (*preCalcSuitability[i]); };
  /**
   * \brief This function will reset the suitability information for a given predator
   * \param pred is the Predator that the suitability functions are defined for
   * \param TimeInfo is the TimeClass for the current model
   */
  void Reset(const Predator* const pred, const TimeClass* const TimeInfo);
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
   * \brief This is the DoubleMatrixPtrVector of calculated suitability values
   * \note The indices for this object are [prey][predator length][prey length]
   */
  DoubleMatrixPtrVector preCalcSuitability;
};

#endif
