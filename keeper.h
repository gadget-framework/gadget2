#ifndef keeper_h
#define keeper_h

#include "likelihood.h"
#include "likelihoodptrvector.h"
#include "stochasticdata.h"
#include "addr_keepmatrix.h"
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
  char* SendString() const;
  void AddComponent(const char* name);
  char* SendComponents() const;
  void ClearComponents();
  int ErrorInUpdate() const;
  void Clear();
  void Opt(intvector& opt) const;
  void ValuesOfVariables(doublevector& val) const;
  void Switches(Parametervector& switches) const;
  void LowerBds(doublevector& lbs) const;
  void UpperBds(doublevector& ubs) const;
  void InitialValues(doublevector& val) const;
  void ScaledValues(doublevector& val) const;
  int NoVariables() const;
  int NoOptVariables() const;
  void Update(int pos, double& value);
  void Update(const doublevector& val);
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
  void WriteInitialInformation(const char* const filename, const Likelihoodptrvector& Likely);
  /* Use: keeper.WriteValues(filename, funcValue, likely)
   * the output has been appended to file with filename = filename in the format:
   * For all values print to filename showing 8 digits:
   * "value" " " "tab"
   * For all values in likely print to filename showing 4 digits:
   * "unweighted likelihood" " " "tab" funcValue showing 15 digits. */
  void WriteValues(const char* const filename, double FunctionValue, const Likelihoodptrvector& Likely) const;
  void WriteInitialInformationInColumns(const char* const filenam) const;
  void WriteValuesInColumns(const char* const filename, double functionValue, const Likelihoodptrvector& Likely) const;
  void WriteParamsInColumns(const char* const filename, double functionValue, const Likelihoodptrvector& Likely) const;
  //AJ 08.08.00 end of change to functions.
  void WriteOptValues(double FunctionValue, const Likelihoodptrvector& Likely) const;
  void ScaleVariables();
  void InitialOptValues(doublevector &val) const;
  void ScaledOptValues(doublevector &val) const;
  void OptSwitches(Parametervector& sw) const;
  void OptValues(doublevector &val) const;
protected:
  addr_keepmatrix address;
  doublevector initialvalues;
  doublevector scaledvalues;
  doublevector values;
  intvector opt;
  StrStack* stack;
  StrStack* likcompnames;
  Parametervector switches;
  doublevector lowerbds;
  doublevector upperbds;
  int error;
};

#endif




