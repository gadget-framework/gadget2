#ifndef modelvariable_h
#define modelvariable_h

#include "formulavector.h"
#include "areatime.h"
#include "doublematrix.h"
#include "commentstream.h"
#include "timevariable.h"
#include "stockvariable.h"

enum MVType { MVFORMULA = 1, MVTIME, MVSTOCK };

/**
 * \class ModelVariable
 * \brief This is the base class used to calculate the value of variables used in the model simulation that are based on other values within the model.  This can be used to calculate values that can change due to time (using the TimeVariable class) or values that are based on stock population (using the StockVariable class).
 *
 * \note This will always be overridden by the derived classes that actually calculate the model values
 */
class ModelVariable {
public:
  /**
   * \brief This is the default ModelVariable constructor
   */
  ModelVariable();
  /**
   * \brief This is the default ModelVariable destructor
   */
  ~ModelVariable() {};
  /**
   * \brief This function will read ModelVariable data from file
   * \param infile is the CommentStream to read the data from
   * \param TimeInfo is the TimeClass for the current model
   * \param keeper is the Keeper for the current model
   */
  virtual void read(CommentStream& infile, const TimeClass* const TimeInfo, Keeper* const keeper);
  /**
   * \brief This operator will return the value of the ModelVariable
   * \return the value of the ModelVariable
   */
  operator double() const { return value; };
  /**
   * \brief This function will swap the ModelVariable value for a new value
   * \param newMV is the new ModelVariable value
   * \param keeper is the Keeper for the current model
   */
  void Interchange(ModelVariable& newMV, Keeper* const keeper) const;
  /**
   * \brief This function will update the ModelVariable value
   * \param newValue is the new value of the ModelVariable value
   */
  void setValue(double newValue);
  /**
   * \brief This function will update the ModelVariable value
   * \param TimeInfo is the TimeClass for the current model
   */
  void Update(const TimeClass* const TimeInfo);
  /**
   * \brief This function will delete a ModelVariable value
   * \param keeper is the Keeper for the current model
   */
  void Delete(Keeper* const keeper) const;
  /**
   * \brief This function will check to see if the ModelVariable value has changed
   * \param TimeInfo is the TimeClass for the current model
   * \return 1 if the values have changed, 0 otherwise
   */
  int didChange(const TimeClass* const TimeInfo);
  /**
   * \brief This will return the type of model variable to be used
   * \return sitype
   */
  MVType getMVType() const { return mvtype; };
private:
  /**
   * \brief This denotes what type of model variable is to be used
   */
  MVType mvtype;
  /**
   * \brief This is the Formula of the value of the ModelVariable
   */
  Formula init;
  /**
   * \brief This is the TimeVariable of the values of the ModelVariable
   */
  TimeVariable TV;
  /**
   * \brief This is the StockVariable of the values of the ModelVariable
   */
  StockVariable SV;
  /**
   * \brief This is the value of the ModelVariable
   */
  double value;
};

#endif
