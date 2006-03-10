#ifndef timevariable_h
#define timevariable_h

#include "formulavector.h"
#include "doublematrix.h"
#include "commentstream.h"

/**
 * \class TimeVariable
 * \brief This is the class used to calculate the value of the time-dependent variables used in the model simulation
 */
class TimeVariable {
public:
  /**
   * \brief This is the default TimeVariable constructor
   */
  TimeVariable();
  /**
   * \brief This is the default TimeVariable destructor
   */
  ~TimeVariable() {};
  /**
   * \brief This function will read TimeVariable data from file
   * \param infile is the CommentStream to read the data from
   * \param TimeInfo is the TimeClass for the current model
   * \param keeper is the Keeper for the current model
   */
  void read(CommentStream& infile, const TimeClass* const TimeInfo, Keeper* const keeper);
  /**
   * \brief This operator will return the value of the TimeVariable
   * \return the value of the TimeVariable
   */
  operator double() const { return value; };
  /**
   * \brief This function will swap the TimeVariable value for a new value
   * \param newTV is the new TimeVariable value
   * \param keeper is the Keeper for the current model
   */
  void Interchange(TimeVariable& newTV, Keeper* const keeper) const;
  /**
   * \brief This function will update the TimeVariable value
   * \param TimeInfo is the TimeClass for the current model
   */
  void Update(const TimeClass* const TimeInfo);
  /**
   * \brief This function will delete a TimeVariable value
   * \param keeper is the Keeper for the current model
   */
  void Delete(Keeper* const keeper) const;
  /**
   * \brief This function will check to see if the TimeVariable value has changed
   * \param TimeInfo is the TimeClass for the current model
   * \return 1 if the values have changed, 0 otherwise
   */
  int didChange(const TimeClass* const TimeInfo);
private:
  /**
   * \brief This function will read TimeVariable data from a separate input file
   * \param infile is the CommentStream to read the time data from
   * \param TimeInfo is the TimeClass for the current model
   * \param keeper is the Keeper for the current model
   */
  void readFromFile(CommentStream& infile, const TimeClass* const TimeInfo, Keeper* const keeper);
  /**
   * \brief This is the flag used to denote whether parameters have been read from a file or not
   */
  int fromfile;
  /**
   * \brief This is the flag used to denote whether the 'modelmatrix' formulation is used or not
   */
  int usemodelmatrix;
  /**
   * \brief This is the identifier for the last timestep that the value of the TimeVariable changed
   */
  int timeid;
  /**
   * \brief This is the last timestep that the value of the TimeVariable changed
   */
  int time;
  /**
   * \brief This is the IntVector of the years when the value of the TimeVariable changes
   */
  IntVector years;
  /**
   * \brief This is the IntVector of the steps when the value of the TimeVariable changes
   */
  IntVector steps;
  /**
   * \brief This is the FormulaVector of the values of the TimeVariable
   */
  FormulaVector values;
  /**
   * \brief This is the DoubleMatrix of the multiplier values for the TimeVariable
   * \note This is only used when the the 'modelmatrix' formulation is specified
   */
  DoubleMatrix modelmatrix;
  /**
   * \brief This is the FormulaVector of the coefficients for the TimeVariable
   * \note This is only used when the the 'modelmatrix' formulation is specified
   */
  FormulaVector coeff;
  /**
   * \brief This is the Formula of the initial value of the TimeVariable
   */
  Formula init;
  /**
   * \brief This is the last value of the TimeVariable
   */
  double lastvalue;
  /**
   * \brief This is the value of the TimeVariable
   */
  double value;
};

#endif
