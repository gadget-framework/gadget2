#ifndef keeper_h
#define keeper_h

#include "likelihood.h"
#include "likelihoodptrvector.h"
#include "stochasticdata.h"
#include "addresskeepermatrix.h"
#include "strstack.h"

class Keeper {
public:
  /**
   * \brief This is the default Keeper constructor
   */
  Keeper();
  /**
   * \brief This is the default Keeper destructor
   */
  ~Keeper();
  void KeepVariable(double& value, const Parameter& attr);
  void DeleteParam(const double& var);
  void ChangeVariable(const double& pre, double& post);
  void setString(const char* str);
  void addString(const char* str);
  void addString(const string str);
  void addComponent(const char* name);
  void clearLast();
  void clearAll();
  /**
   * \brief This function will scale the variables to be optimised (for the Hooke & Jeeves optimisation algorithm)
   */
  void ScaleVariables();
  /**
   * \brief This function will return a copy the flags to denote which variables will be optimsised
   * \param opt is the IntVector that will contain a copy of the flags
   */
  void Opt(IntVector& opt) const;
  /**
   * \brief This function will return a copy the names of the variables
   * \param sw is the ParameterVector that will contain a copy of the names
   */
  void Switches(ParameterVector& sw) const;
  /**
   * \brief This function will return a copy the lower bounds of the variables
   * \param lbs is the DoubleVector that will contain a copy of the lower bounds
   */
  void LowerBds(DoubleVector& lbs) const;
  /**
   * \brief This function will return a copy the upper bounds of the variables
   * \param ubs is the DoubleVector that will contain a copy of the upper bounds
   */
  void UpperBds(DoubleVector& ubs) const;
  /**
   * \brief This function will return a copy the initial value of the variables
   * \param val is the DoubleVector that will contain a copy of the initial values
   */
  void InitialValues(DoubleVector& val) const;
  /**
   * \brief This function will return a copy the scaled value of the variables
   * \param val is the DoubleVector that will contain a copy of the scaled values
   */
  void ScaledValues(DoubleVector& val) const;
  /**
   * \brief This function will return a copy the current value of the variables
   * \param val is the DoubleVector that will contain a copy of the current values
   */
  void CurrentValues(DoubleVector& val) const;
  /**
   * \brief This function will return a copy the names of the variables to be optimised
   * \param sw is the ParameterVector that will contain a copy of the names
   */
  void OptSwitches(ParameterVector& sw) const;
  /**
   * \brief This function will return a copy the lower bounds of the variables to be optimised
   * \param lbs is the DoubleVector that will contain a copy of the lower bounds
   */
  void LowerOptBds(DoubleVector& lbs) const;
  /**
   * \brief This function will return a copy the upper bounds of the variables to be optimised
   * \param ubs is the DoubleVector that will contain a copy of the upper bounds
   */
  void UpperOptBds(DoubleVector& ubs) const;
  /**
   * \brief This function will return a copy the initial value of the variables to be optimised
   * \param val is the DoubleVector that will contain a copy of the initial values
   */
  void InitialOptValues(DoubleVector& val) const;
  /**
   * \brief This function will return a copy the scaled value of the variables to be optimised
   * \param val is the DoubleVector that will contain a copy of the scaled values
   */
  void ScaledOptValues(DoubleVector& val) const;
  /**
   * \brief This function will return the number of variables
   * \return number of variables
   */
  int numVariables() const;
  /**
   * \brief This function will return the number of variables to be optimised
   * \return number of variables to be optimised
   */
  int numOptVariables() const;
  /**
   * \brief This function will update one Keeper variables with a new value
   * \param pos is the identifier of the varaibles to update
   * \param value is the new value of the variable
   */
  void Update(int pos, double& value);
  /**
   * \brief This function will update the Keeper variables with new values
   * \param val is the DoubleVector containing the new values of the variables
   */
  void Update(const DoubleVector& val);
  /**
   * \brief This function will update the Keeper variables with new values from StochasticData
   * \param Stoch is the StochasticData containing the new values of the variables
   */
  void Update(const StochasticData* const Stoch);
  /**
   * \brief This function will write header information about the model parameters to file
   * \param filename is the name of the file to write the model information to
   * \param Likely is the LikelihoodPtrVector containing the likelihood components for the current model
   */
  void writeInitialInformation(const char* const filename, const LikelihoodPtrVector& Likely);
  /**
   * \brief This function will write current information about the model parameters to file
   * \param filename is the name of the file to write the model information to
   * \param Likely is the LikelihoodPtrVector containing the likelihood components for the current model
   * \param prec is the precision to use in the output file
   */
  void writeValues(const char* const filename, const LikelihoodPtrVector& Likely, int prec) const;
  /**
   * \brief This function will write header information about the model parameters to file in a column format
   * \param filename is the name of the file to write the model information to
   */
  void writeInitialInformationInColumns(const char* const filename) const;
  /**
   * \brief This function will write current information about the model parameters to file in a column format
   * \param filename is the name of the file to write the model information to
   * \param prec is the precision to use in the output file
   */
  void writeValuesInColumns(const char* const filename, int prec) const;
  /**
   * \brief This function will write final information about the model parameters to file in a column format (which can then be used as the starting point for a subsequent model run)
   * \param filename is the name of the file to write the model information to
   * \param prec is the precision to use in the output file
   * \param interrupt is a flag to denote whether the current run was interrupted by the user or not
   */
  void writeParamsInColumns(const char* const filename, int prec, int interrupt) const;
  /**
   * \brief This function will display information about the optimised values of the parameters
   * \param Likely is the LikelihoodPtrVector containing the likelihood components for the current model
   */
  void writeOptValues(const LikelihoodPtrVector& Likely) const;
  /**
   * \brief This function will check that the values of the parameters are within the bounds specified in the input file
   */
  void checkBounds() const;
  /**
   * \brief This function will return the flag used to denote whether the bounds of the parameters have been specified or not
   */
  int boundsGiven() const { return boundsgiven; };
protected:
  /**
   * \brief This is the AddressKeeperMatrix used to store information about the value and name of the parameters
   */
  AddressKeeperMatrix address;
  /**
   * \brief This is the DoubleVector used to store the initial values of the parameters
   */
  DoubleVector initialvalues;
  /**
   * \brief This is the DoubleVector used to store the scaled values of the parameters
   */
  DoubleVector scaledvalues;
  /**
   * \brief This is the DoubleVector used to store the values of the parameters
   */
  DoubleVector values;
  /**
   * \brief This is the IntVector used to store information about whether the parameters are to be optimised
   * \note if opt[i] is 1 then parameter i is to be optimised, else if opt[i] is 0 then parameter i is not to be optimised and its value is fixed throughout the optimisation process
   */
  IntVector opt;
  /**
   * \brief This is the StrStack used to store information about the use of the parameters
   */
  StrStack* stack;
  /**
   * \brief This is the StrStack used to store information about the likelihood components
   */
  StrStack* likcompnames;
  /**
   * \brief This is the ParameterVector used to store information about the names of the parameters
   */
  ParameterVector switches;
  /**
   * \brief This is the DoubleVector used to store information about the lower bounds of the parameters
   */
  DoubleVector lowerbds;
  /**
   * \brief This is the DoubleVector used to store information about the upper bounds of the parameters
   */
  DoubleVector upperbds;
  /**
   * \brief This is a flag used to denote whether the bounds of the parameters have been specified or not
   */
  int boundsgiven;
};

#endif
