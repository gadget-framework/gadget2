#ifndef keeper_h
#define keeper_h

#include "likelihood.h"
#include "likelihoodptrvector.h"
#include "stochasticdata.h"
#include "addresskeepermatrix.h"
#include "strstack.h"

class Keeper {
public:
  Keeper();
  ~Keeper();
  void KeepVariable(double& value, const Parameter& attr);
  void DeleteParam(const double& var);
  void ChangeVariable(const double& pre, double& post);
  void ClearLast();
  void ClearLastAddString(const char* str);
  void ClearAll();
  void SetString(const char* str);
  void AddString(const char* str);
  void AddString(const string str);
  void AddComponent(const char* name);
  char* SendComponents() const;
  void ClearComponents();
  int ErrorInUpdate() const { return error; };
  void Clear() { error = 0; };
  void Opt(IntVector& opt) const;
  void ValuesOfVariables(DoubleVector& val) const;
  void Switches(ParameterVector& switches) const;
  void LowerBds(DoubleVector& lbs) const;
  void UpperBds(DoubleVector& ubs) const;
  void LowerOptBds(DoubleVector& lbs) const;
  void UpperOptBds(DoubleVector& ubs) const;
  void InitialValues(DoubleVector& val) const;
  void ScaledValues(DoubleVector& val) const;
  int NoVariables() const;
  int NoOptVariables() const;
  void Update(int pos, double& value);
  void Update(const DoubleVector& val);
  void Update(const StochasticData * const Stoch);
  /* AJ 08.08.00 Changes made to the following five functions for printing
   * Use:  keeper.WriteInitialInformation(filename, likely)
   * the output has been written to file with filename = filename in the format:
   * For all switches print to filename:
   * Switch "switch-number i" "tab"
   * For all switchnames belonging to switch-number i print to filename:
   * "switchname j" "tab"
   * Print to filename: "New line" Components: "tab"
   * For all Components print to filename:
   * "Componentname" "tab"
   * Print to filename: "New line" Likelihoodtype "tab"
   * For all Likelihoodtypes in likely  print to filename:
   * "likelihoodtype" "tab"
   * Print to filename: "New line" Weights(s)      "tab"
   * For all weights print to filename:
   * "weight" "tab"
   * Print to filename "New line" */
  void WriteInitialInformation(const char* const filename, const LikelihoodPtrVector& Likely);
  /* Use: keeper.WriteValues(filename, funcValue, likely)
   * the output has been appended to file with filename = filename in the format:
   * For all values print to filename showing 8 digits:
   * "value" " " "tab"
   * For all values in likely print to filename showing 4 digits:
   * "unweighted likelihood" " " "tab" funcValue showing 15 digits. */
  void WriteValues(const char* const filename, double FunctionValue, const LikelihoodPtrVector& Likely, int prec) const;
  void WriteInitialInformationInColumns(const char* const filenam) const;
  void WriteValuesInColumns(const char* const filename, double functionValue, const LikelihoodPtrVector& Likely, int prec) const;
  void WriteParamsInColumns(const char* const filename, double functionValue, const LikelihoodPtrVector& Likely, int prec) const;
  //AJ 08.08.00 end of change to functions.
  void WriteOptValues(double FunctionValue, const LikelihoodPtrVector& Likely) const;
  void ScaleVariables();
  void InitialOptValues(DoubleVector &val) const;
  void ScaledOptValues(DoubleVector &val) const;
  void OptSwitches(ParameterVector& sw) const;
  void OptValues(DoubleVector &val) const;
  void CheckBounds() const;
protected:
  AddressKeeperMatrix address;
  DoubleVector initialvalues;
  DoubleVector scaledvalues;
  DoubleVector values;
  IntVector opt;
  StrStack* stack;
  StrStack* likcompnames;
  ParameterVector switches;
  DoubleVector lowerbds;
  DoubleVector upperbds;
  int error;
};

#endif




