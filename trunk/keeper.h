#ifndef keeper_h
#define keeper_h

#include "likelihood.h"
#include "likelihoodptrvector.h"
#include "optinfoptrvector.h"
#include "stochasticdata.h"
#include "addresskeepermatrix.h"
#include "strstack.h"

/**
 * \class Keeper
 * \brief This is the class used to store information about the variables that are used in model simulation
 *
 * This class stores all the information about the variables that are used in the model simulation.  This includes the current value, the initial value and best value found so far by any optimisation routines, the upper and lower bounds for the variables and a flag to denote whether the variable is to be optimised or not.  This class also updates the model parameters with new values for the variables, either as a result of the optimisation routines or when new values for the parameters are received from the network if the simulation is part of a parallel optimisation, using paramin.
 */
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
  /**
   * \brief This function will register a variable in the internal list, along with an initial value
   * \param value is the value for the variable
   * \param attr is the name of the Parameter for the variable
   */
  void keepVariable(double& value, Parameter& attr);
  /**
   * \brief This function will delete a variable from the internal list
   * \param var is the value for the variable
   */
  void deleteParameter(const double& var);
  /**
   * \brief This function will replace the value of a variable with a new value
   * \param pre is the old value for the variable
   * \param post is the new value for the variable
   */
  void changeVariable(const double& pre, double& post);
  /**
   * \brief This function will set the text string that is used to describe a variable
   * \param str is the text string to be stored
   */
  void setString(const char* str);
  /**
   * \brief This function will add a text string that is used to describe a variable
   * \param str is the text string to be stored
   */
  void addString(const char* str);
  /**
   * \brief This function will add a text string that is used to describe a variable
   * \param str is the text string to be stored
   */
  void addString(const string str);
  /**
   * \brief This function will remove the last text string from the list used to describe variables
   */
  void clearLast();
  /**
   * \brief This function will remove all of the text strings from the list used to describe variables
   */
  void clearAll();
  /**
   * \brief This function will store the current value of the variables from the optimisation process
   * \param likvalue is the current likelihood value
   * \param point is the DoubleVector of the current parameter values
   */
  void storeVariables(double likvalue, const DoubleVector& point);
  /**
   * \brief This function will reset the variables that have been scaled, to be optimised using the Simulated Annealing optimisation algorithm
   */
  void resetVariables();
  /**
   * \brief This function will scale the variables to be optimised, for the Hooke & Jeeves and BFGS optimisation algorithms
   */
  void scaleVariables();
  /**
   * \brief This function will return a copy of the flags to denote which variables will be optimsised
   * \param opt is the IntVector that will contain a copy of the flags
   */
  void getOptFlags(IntVector& opt) const;
  /**
   * \brief This function will return a copy of the names of the variables
   * \param sw is the ParameterVector that will contain a copy of the names
   */
  void getSwitches(ParameterVector& sw) const;
  /**
   * \brief This function will return a copy of the lower bounds of the variables
   * \param lbs is the DoubleVector that will contain a copy of the lower bounds
   */
  void getLowerBounds(DoubleVector& lbs) const;
  /**
   * \brief This function will return a copy of the upper bounds of the variables
   * \param ubs is the DoubleVector that will contain a copy of the upper bounds
   */
  void getUpperBounds(DoubleVector& ubs) const;
  /**
   * \brief This function will return a copy of the initial value of the variables
   * \param val is the DoubleVector that will contain a copy of the initial values
   */
  void getInitialValues(DoubleVector& val) const;
  /**
   * \brief This function will return a copy of the scaled value of the variables
   * \param val is the DoubleVector that will contain a copy of the scaled values
   */
  void getScaledValues(DoubleVector& val) const;
  /**
   * \brief This function will return a copy of the current value of the variables
   * \param val is the DoubleVector that will contain a copy of the current values
   */
  void getCurrentValues(DoubleVector& val) const;
  /**
   * \brief This function will return a copy of the lower bounds of the variables to be optimised
   * \param lbs is the DoubleVector that will contain a copy of the lower bounds
   */
  void getOptLowerBounds(DoubleVector& lbs) const;
  /**
   * \brief This function will return a copy of the upper bounds of the variables to be optimised
   * \param ubs is the DoubleVector that will contain a copy of the upper bounds
   */
  void getOptUpperBounds(DoubleVector& ubs) const;
  /**
   * \brief This function will return a copy of the initial value of the variables to be optimised
   * \param val is the DoubleVector that will contain a copy of the initial values
   */
  void getOptInitialValues(DoubleVector& val) const;
  /**
   * \brief This function will return a copy of the scaled value of the variables to be optimised
   * \param val is the DoubleVector that will contain a copy of the scaled values
   */
  void getOptScaledValues(DoubleVector& val) const;
  /**
   * \brief This function will return the number of variables
   * \return number of variables
   */
  int numVariables() const;
  /**
   * \brief This function will return the number of variables to be optimised
   * \return number of variables to be optimised
   */
  int numOptVariables() const { return numoptvar; };
  /**
   * \brief This function will update one Keeper variable with a new value
   * \param pos is the identifier of the variable to update
   * \param value is the new value of the variable
   */
  void Update(int pos, double& value);
  /**
   * \brief This function will update the Keeper variables with new values from a vector
   * \param val is the DoubleVector containing the new values of the variables
   */
  void Update(const DoubleVector& val);
  /**
   * \brief This function will update the Keeper variables with new values from StochasticData
   * \param Stoch is the StochasticData containing the new values of the variables
   */
  void Update(const StochasticData* const Stoch);
  /**
   * \brief This function will open the output file and write header information
   * \param filename is the name of the file to write the model information to
   */
  void openPrintFile(const char* const filename);
  /**
   * \brief This function will write header information about the model parameters to file
   * \param likevec is the LikelihoodPtrVector containing the likelihood components for the current model
   */
  void writeInitialInformation(const LikelihoodPtrVector& likevec);
  /**
   * \brief This function will write current information about the model parameters to file
   * \param likevec is the LikelihoodPtrVector containing the likelihood components for the current model
   * \param prec is the precision to use in the output file
   */
  void writeValues(const LikelihoodPtrVector& likevec, int prec);
  /**
   * \brief This function will write final information about the model parameters to file in a column format (which can then be used as the starting point for a subsequent model run)
   * \param optvec is the OptInfoPtrVector containing the optimisation algorithms used for the current model
   * \param filename is the name of the file to write the model information to
   * \param prec is the precision to use in the output file
   * \param interrupt is the flag to denote whether the current run was interrupted by the user or not
   */
  void writeParams(const OptInfoPtrVector& optvec, const char* const filename, int prec, int interrupt);
  /**
   * \brief This function will display information about the best values of the parameters found so far during an optimisation run
   */
  void writeBestValues();
  /**
   * \brief This function will check that the values of the parameters are within the bounds specified in the input file
   * \param likevec is the LikelihoodPtrVector containing the likelihood components for the current model
   */
  void checkBounds(const LikelihoodPtrVector& likevec) const;
  /**
   * \brief This function will return the flag used to denote whether the bounds of the parameters have been specified or not
   * \return flag to denote whether the bounds have been given or not
   */
  int boundsGiven() const { return boundsgiven; };
  /**
   * \brief This function will return the best likelihood score found so far by the optimisation process
   * \return bestlikelihood
   */
  double getBestLikelihoodScore() const { return bestlikelihood; };
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
   * \brief This is the DoubleVector used to store the best values of the parameters that the optimisation process has found so far
   */
  DoubleVector bestvalues;
  /**
   * \brief This is the IntVector used to store information about whether the parameters are to be optimised
   * \note If opt[i] is 1 then parameter i is to be optimised, else if opt[i] is 0 then parameter i is not to be optimised and its value is fixed throughout the optimisation process
   */
  IntVector opt;
  /**
   * \brief This is the StrStack used to store information about the use of the parameters
   */
  StrStack* stack;
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
   * \brief This is the number of parameters to be optimised
   */
  int numoptvar;
  /**
   * \brief This is the flag used to denote whether the bounds of the parameters have been specified or not
   */
  int boundsgiven;
  /**
   * \brief This is the best likelihood score found so far by the optimisation process
   */
  double bestlikelihood;
  /**
   * \brief This is the flag used to denote whether an output file has been specified or not
   */
  int fileopen;
  /**
   * \brief This ofstream is the file that all the parameter information gets sent to
   */
  ofstream outfile;
};

#endif
