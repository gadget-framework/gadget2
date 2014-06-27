#ifndef selectfunc_h
#define selectfunc_h

#include "commentstream.h"
#include "hasname.h"
#include "keeper.h"
#include "modelvariablevector.h"

/**
 * \class SelectFunc
 * \brief This is the base class used to calculate the selection level as a function of length
 * \note This will always be overridden by the derived classes that actually calculate the selection level
 */
class SelectFunc : public HasName {
public:
  /**
   * \brief This is the default SelectFunc constructor
   * \param givenname is the name for the SelectFunc selection function
   */
  SelectFunc(const char* givenname) : HasName(givenname) {};
  /**
   * \brief This is the default SelectFunc destructor
   */
  virtual ~SelectFunc() {};
  /**
   * \brief This function will read the value of the selection function parameters from file
   * \param infile is the CommentStream to read the parameters from
   * \param TimeInfo is the TimeClass for the current model
   * \param keeper is the Keeper for the current model
   */
  void readConstants(CommentStream& infile, const TimeClass* const TimeInfo, Keeper* const keeper);
  /**
   * \brief This function will update the selection function parameters
   * \param TimeInfo is the TimeClass for the current model
   */
  void updateConstants(const TimeClass* const TimeInfo);
  /**
   * \brief This function will check to see if the selection function parameters have changed
   * \param TimeInfo is the TimeClass for the current model
   * \return 1 if the parameters have changed, 0 otherwise
   */
  int didChange(const TimeClass* const TimeInfo);
  /**
   * \brief This will return the selection level that has been calculated
   * \param len is the length of the length class that is selected
   * \return 0 (will be overridden in derived classes)
   */
  virtual double calculate(double len) = 0;
  /**
   * \brief This will return the number of constants used to calculate the selection value
   * \return number
   */
  int numConstants() { return coeff.Size(); };
protected:
  /**
   * \brief This is the ModelVariableVector of the selection function constants
   */
  ModelVariableVector coeff;
};

/**
 * \class ConstSelectFunc
 * \brief This is the class used to calculate a constant selection level
 */
class ConstSelectFunc : public SelectFunc {
public:
  /**
   * \brief This is the ConstSelectFunc constructor
   */
  ConstSelectFunc();
  /**
   * \brief This is the default ConstSelectFunc destructor
   */
  virtual ~ConstSelectFunc() {};
  /**
   * \brief This will return the selection level that has been calculated
   * \param len is the length of the length class that is selected
   * \return selection level
   */
  virtual double calculate(double len);
};

/**
 * \class ExpSelectFunc
 * \brief This is the class used to calculate the selection level based on an exponential function of length
 */
class ExpSelectFunc : public SelectFunc {
public:
  /**
   * \brief This is the ExpSelectFunc constructor
   */
  ExpSelectFunc();
  /**
   * \brief This is the default ExpSelectFunc destructor
   */
  virtual ~ExpSelectFunc() {};
  /**
   * \brief This will return the selection level that has been calculated
   * \param len is the length of the length class that is selected
   * \return selection level
   */
  virtual double calculate(double len);
};

/**
 * \class StraightSelectFunc
 * \brief This is the class used to calculate the selection level based on a linear function of length
 */
class StraightSelectFunc : public SelectFunc {
public:
  /**
   * \brief This is the StraightSelectFunc constructor
   */
  StraightSelectFunc();
  /**
   * \brief This is the default StraightSelectFunc destructor
   */
  virtual ~StraightSelectFunc() {};
  /**
   * \brief This will return the selection level that has been calculated
   * \param len is the length of the length class that is selected
   * \return selection level
   */
  virtual double calculate(double len);
};

#endif
