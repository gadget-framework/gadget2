#ifndef suitfunc_h
#define suitfunc_h

#include "commentstream.h"
#include "hasname.h"
#include "keeper.h"
#include "modelvariablevector.h"

/**
 * \class SuitFunc
 * \brief This is the base class used to calculate the predator prey suitability value
 * \note This will always be overridden by the derived classes that actually calculate the suitability value
 */
class SuitFunc : public HasName {
public:
  /**
   * \brief This is the default SuitFunc constructor
   * \param givenname is the name for the SuitFunc selection function
   */
  SuitFunc(const char* givenname) : HasName(givenname) {};
  /**
   * \brief This is the default SuitFunc destructor
   */
  virtual ~SuitFunc() {};
  /**
   * \brief This function will return the value of the selection function parameters
   * \return v, a ModelVariableVector of the parameters
   */
  const ModelVariableVector& getConstants() const;
  /**
   * \brief This function will read the value of the suitability function parameters from file
   * \param infile is the CommentStream to read the parameters from
   * \param TimeInfo is the TimeClass for the current model
   * \param keeper is the Keeper for the current model
   */
  void readConstants(CommentStream& infile, const TimeClass* const TimeInfo, Keeper* const keeper);
  /**
   * \brief This function will update the suitability function parameters
   * \param TimeInfo is the TimeClass for the current model
   */
  void updateConstants(const TimeClass* const TimeInfo);
  /**
   * \brief This function will check to see if the suitability function parameters have changed
   * \param TimeInfo is the TimeClass for the current model
   * \return 1 if the parameters have changed, 0 otherwise
   */
  int didChange(const TimeClass* const TimeInfo);
  /**
   * \brief This will return 1 if the suitability function is based on the predator length, 0 otherwise
   * \return 0 (will be overridden in derived classes)
   */
  virtual int usesPredLength() = 0;
  /**
   * \brief This will return 1 if the suitability function is based on the prey length, 0 otherwise
   * \return 0 (will be overridden in derived classes)
   */
  virtual int usesPreyLength() = 0;
  /**
   * \brief This will set the predator length
   * \param length is the predator length
   */
  virtual void setPredLength(double length);
  /**
   * \brief This will set the prey length
   * \param length is the prey length
   */
  virtual void setPreyLength(double length);
  /**
   * \brief This will return the suitability value that has been calculated
   * \return 0 (will be overridden in derived classes)
   */
  virtual double calculate() = 0;
  /**
   * \brief This will return the number of constants used to calculate the suitability value
   * \return number
   */
  int numConstants() { return coeff.Size(); };
protected:
  /**
   * \brief This is the ModelVariableVector of suitability function constants
   */
  ModelVariableVector coeff;
};

/**
 * \class ExpSuitFuncA
 * \brief This is the class used to calculate the suitability based on an exponential function of the predator and prey lengths
 */
class ExpSuitFuncA : public SuitFunc {
public:
  /**
   * \brief This is the ExpSuitFuncA constructor
   */
  ExpSuitFuncA();
  /**
   * \brief This is the default ExpSuitFuncA destructor
   */
  virtual ~ExpSuitFuncA() {};
  /**
   * \brief This will return 1 if the suitability function is based on the predator length, 0 otherwise
   * \return 1
   */
  virtual int usesPredLength() { return 1; };
  /**
   * \brief This will return 1 if the suitability function is based on the prey length, 0 otherwise
   * \return 1
   */
  virtual int usesPreyLength() { return 1; };
  /**
   * \brief This will set the predator length
   * \param length is the predator length
   */
  virtual void setPredLength(double length) { predLength = length; };
  /**
   * \brief This will set the prey length
   * \param length is the prey length
   */
  virtual void setPreyLength(double length) { preyLength = length; };
  /**
   * \brief This will return the suitability value that has been calculated
   * \return value
   */
  virtual double calculate();
private:
  /**
   * \brief This is the length of the prey
   */
  double preyLength;
  /**
   * \brief This is the length of the predator
   */
  double predLength;
};

/**
 * \class ConstSuitFunc
 * \brief This is the class used to calculate a constant suitability
 */
class ConstSuitFunc : public SuitFunc {
public:
  /**
   * \brief This is the ConstSuitFunc constructor
   */
  ConstSuitFunc();
  /**
   * \brief This is the default ConstSuitFunc destructor
   */
  virtual ~ConstSuitFunc() {};
  /**
   * \brief This will return 1 if the suitability function is based on the predator length, 0 otherwise
   * \return 0
   */
  virtual int usesPredLength() { return 0; };
  /**
   * \brief This will return 1 if the suitability function is based on the prey length, 0 otherwise
   * \return 0
   */
  virtual int usesPreyLength() { return 0; };
  /**
   * \brief This will return the suitability value that has been calculated
   * \return value
   */
  virtual double calculate();
};

/**
 * \class AndersenSuitFunc
 * \brief This is the class used to calculate the suitability based on an Andersen function
 */
class AndersenSuitFunc : public SuitFunc {
public:
  /**
   * \brief This is the AndersenSuitFunc constructor
   */
  AndersenSuitFunc();
  /**
   * \brief This is the default AndersenSuitFunc destructor
   */
  virtual ~AndersenSuitFunc() {};
  /**
   * \brief This will return 1 if the suitability function is based on the predator length, 0 otherwise
   * \return 1
   */
  virtual int usesPredLength() { return 1; };
  /**
   * \brief This will return 1 if the suitability function is based on the prey length, 0 otherwise
   * \return 1
   */
  virtual int usesPreyLength() { return 1; };
  /**
   * \brief This will set the predator length
   * \param length is the predator length
   */
  virtual void setPredLength(double length) { predLength = length; };
  /**
   * \brief This will set the prey length
   * \param length is the prey length
   */
  virtual void setPreyLength(double length) { preyLength = length; };
  /**
   * \brief This will return the suitability value that has been calculated
   * \return value
   */
  virtual double calculate();
private:
  /**
   * \brief This is the length of the prey
   */
  double preyLength;
  /**
   * \brief This is the length of the predator
   */
  double predLength;
};

/**
 * \class ExpSuitFuncL50
 * \brief This is the class used to calculate the suitability based on an exponential function of the prey length
 */
class ExpSuitFuncL50 : public SuitFunc {
public:
  /**
   * \brief This is the ExpSuitFuncL50 constructor
   */
  ExpSuitFuncL50();
  /**
   * \brief This is the default ExpSuitFuncL50 destructor
   */
  virtual ~ExpSuitFuncL50() {};
  /**
   * \brief This will return 1 if the suitability function is based on the predator length, 0 otherwise
   * \return 0
   */
  virtual int usesPredLength() { return 0; };
  /**
   * \brief This will return 1 if the suitability function is based on the prey length, 0 otherwise
   * \return 1
   */
  virtual int usesPreyLength() { return 1; };
  /**
   * \brief This will set the prey length
   * \param length is the prey length
   */
  virtual void setPreyLength(double length) { preyLength = length; };
  /**
   * \brief This will return the suitability value that has been calculated
   * \return value
   */
  virtual double calculate();
private:
  /**
   * \brief This is the length of the prey
   */
  double preyLength;
};

/**
 * \class StraightSuitFunc
 * \brief This is the class used to calculate the suitability based on a linear function of the prey length
 */
class StraightSuitFunc : public SuitFunc {
public:
  /**
   * \brief This is the StraightSuitFunc constructor
   */
  StraightSuitFunc();
  /**
   * \brief This is the default StraightSuitFunc destructor
   */
  virtual ~StraightSuitFunc() {};
  /**
   * \brief This will return 1 if the suitability function is based on the predator length, 0 otherwise
   * \return 0
   */
  virtual int usesPredLength() { return 0; };
  /**
   * \brief This will return 1 if the suitability function is based on the prey length, 0 otherwise
   * \return 1
   */
  virtual int usesPreyLength() { return 1; };
  /**
   * \brief This will set the prey length
   * \param length is the prey length
   */
  virtual void setPreyLength(double length) { preyLength = length; };
  /**
   * \brief This will return the suitability value that has been calculated
   * \return value
   */
  virtual double calculate();
private:
  /**
   * \brief This is the length of the prey
   */
  double preyLength;
};

/**
 * \class InverseSuitFunc
 * \brief This is the class used to calculate the suitability based on an inverse exponential function of the prey length
 */
class InverseSuitFunc : public SuitFunc {
public:
  /**
   * \brief This is the InverseSuitFunc constructor
   */
  InverseSuitFunc();
  /**
   * \brief This is the default InverseSuitFunc destructor
   */
  virtual ~InverseSuitFunc() {};
  /**
   * \brief This will return 1 if the suitability function is based on the predator length, 0 otherwise
   * \return 0
   */
  virtual int usesPredLength() { return 0; };
  /**
   * \brief This will return 1 if the suitability function is based on the prey length, 0 otherwise
   * \return 1
   */
  virtual int usesPreyLength() { return 1; };
  /**
   * \brief This will set the prey length
   * \param length is the prey length
   */
  virtual void setPreyLength(double length) { preyLength = length; };
  /**
   * \brief This will return the suitability value that has been calculated
   * \return value
   */
  virtual double calculate();
private:
  /**
   * \brief This is the length of the prey
   */
  double preyLength;
};

/**
 * \class StraightUnboundedSuitFunc
 * \brief This is the class used to calculate the suitability based on a linear function of the prey length with no upper bound
 */
class StraightUnboundedSuitFunc : public SuitFunc {
public:
  /**
   * \brief This is the StraightUnboundedSuitFunc constructor
   */
  StraightUnboundedSuitFunc();
  /**
   * \brief This is the default StraightUnboundedSuitFunc destructor
   */
  virtual ~StraightUnboundedSuitFunc() {};
  /**
   * \brief This will return 1 if the suitability function is based on the predator length, 0 otherwise
   * \return 0
   */
  virtual int usesPredLength() { return 0; };
  /**
   * \brief This will return 1 if the suitability function is based on the prey length, 0 otherwise
   * \return 1
   */
  virtual int usesPreyLength() { return 1; };
  /**
   * \brief This will set the prey length
   * \param length is the prey length
   */
  virtual void setPreyLength(double length) { preyLength = length; };
  /**
   * \brief This will return the suitability value that has been calculated
   * \return value
   */
  virtual double calculate();
private:
  /**
   * \brief This is the length of the prey
   */
  double preyLength;
};

/**
 * \class RichardsSuitFunc
 * \brief This is the class used to calculate the suitability based on a Richards function
 */
class RichardsSuitFunc : public SuitFunc {
public:
  /**
   * \brief This is the RichardsSuitFunc constructor
   */
  RichardsSuitFunc();
  /**
   * \brief This is the default RichardsSuitFunc destructor
   */
  virtual ~RichardsSuitFunc() {};
  /**
   * \brief This will return 1 if the suitability function is based on the predator length, 0 otherwise
   * \return 1
   */
  virtual int usesPredLength() { return 1; };
  /**
   * \brief This will return 1 if the suitability function is based on the prey length, 0 otherwise
   * \return 1
   */
  virtual int usesPreyLength() { return 1; };
  /**
   * \brief This will set the predator length
   * \param length is the predator length
   */
  virtual void setPredLength(double length) { predLength = length; };
  /**
   * \brief This will set the prey length
   * \param length is the prey length
   */
  virtual void setPreyLength(double length) { preyLength = length; };
  /**
   * \brief This will return the suitability value that has been calculated
   * \return value
   */
  virtual double calculate();
private:
  /**
   * \brief This is the length of the prey
   */
  double preyLength;
  /**
   * \brief This is the length of the predator
   */
  double predLength;
};

/**
 * \class GammaSuitFunc
 * \brief This is the class used to calculate the suitability based on a Gamma function, usually used when calculating suitability curves for gillnet fleets
 */
class GammaSuitFunc : public SuitFunc {
public:
  /**
   * \brief This is the GammaSuitFunc constructor
   */
  GammaSuitFunc();
  /**
   * \brief This is the default GammaSuitFunc destructor
   */
  virtual ~GammaSuitFunc() {};
  /**
   * \brief This will return 1 if the suitability function is based on the predator length, 0 otherwise
   * \return 0
   */
  virtual int usesPredLength() { return 0; };
  /**
   * \brief This will return 1 if the suitability function is based on the prey length, 0 otherwise
   * \return 1
   */
  virtual int usesPreyLength() { return 1; };
  /**
   * \brief This will set the prey length
   * \param length is the prey length
   */
  virtual void setPreyLength(double length) { preyLength = length; };
  /**
   * \brief This will return the suitability value that has been calculated
   * \return value
   */
  virtual double calculate();
private:
  /**
   * \brief This is the length of the prey
   */
  double preyLength;
};

/**
 * \class AndersenFleetSuitFunc
 * \brief This is the class used to calculate the suitability based on an Andersen function for fleet based predators
 */
class AndersenFleetSuitFunc : public SuitFunc {
public:
  /**
   * \brief This is the AndersenFleetSuitFunc constructor
   */
  AndersenFleetSuitFunc();
  /**
   * \brief This is the default AndersenFleetSuitFunc destructor
   */
  virtual ~AndersenFleetSuitFunc() {};
  /**
   * \brief This will return 1 if the suitability function is based on the predator length, 0 otherwise
   * \return 0
   */
  virtual int usesPredLength() { return 0; };
  /**
   * \brief This will return 1 if the suitability function is based on the prey length, 0 otherwise
   * \return 1
   */
  virtual int usesPreyLength() { return 1; };
  /**
   * \brief This will set the prey length
   * \param length is the prey length
   */
  virtual void setPreyLength(double length) { preyLength = length; };
  /**
   * \brief This will return the suitability value that has been calculated
   * \return value
   */
  virtual double calculate();
private:
  /**
   * \brief This is the length of the prey
   */
  double preyLength;
};

#endif
