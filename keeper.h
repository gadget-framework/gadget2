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
  void clearLast();
  void setString(const char* str);
  void addString(const char* str);
  void addString(const string str);
  void addComponent(const char* name);
  void clearAll();
  void ScaleVariables();
  void Opt(IntVector& opt) const;
  void ValuesOfVariables(DoubleVector& val) const;
  void Switches(ParameterVector& switches) const;
  void LowerBds(DoubleVector& lbs) const;
  void UpperBds(DoubleVector& ubs) const;
  void LowerOptBds(DoubleVector& lbs) const;
  void UpperOptBds(DoubleVector& ubs) const;
  void InitialValues(DoubleVector& val) const;
  void ScaledValues(DoubleVector& val) const;
  void InitialOptValues(DoubleVector& val) const;
  void ScaledOptValues(DoubleVector& val) const;
  void OptSwitches(ParameterVector& sw) const;
  void OptValues(DoubleVector& val) const;
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
   */
  void writeParamsInColumns(const char* const filename, int prec) const;
  /**
   * \brief This function will display information about the optimised values of the parameters
   * \param Likely is the LikelihoodPtrVector containing the likelihood components for the current model
   */
  void writeOptValues(const LikelihoodPtrVector& Likely) const;
  /**
   * \brief This function will check that the values of the parameters are within the bounds  specified in the input file
   */
  void checkBounds() const;
  /**
   * \brief This function will return the flag used to denote whether the bounds of the parameters have been specified or not
   */
  int boundsGiven() const { return boundsgiven; };
protected:
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
  StrStack* stack;
  StrStack* likcompnames;
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
