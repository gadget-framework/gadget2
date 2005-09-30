#ifndef initialinputfile_h
#define initialinputfile_h

#include "charptrvector.h"
#include "parametervector.h"
#include "doublevector.h"
#include "intvector.h"
#include "commentstream.h"

/**
 * \class InitialInputFile
 * \brief This is the class used to get and store initial values for the variables used in the model simulation from an input file
 */
class InitialInputFile {
public:
  /**
   * \brief This is the default InitialInputFile constructor
   * \param filename is the name of the file to read the initial parameter values from
   */
  InitialInputFile(const char* const filename);
  /**
   * \brief This is the default InitialInputFile destructor
   */
  ~InitialInputFile();
  /**
   * \brief This function will return a copy of the data read from the input file
   * \param sw is the ParameterVector that will contain a copy of the names
   * \param val is the DoubleVector that will contain a copy of the values
   * \param low is the DoubleVector that will contain a copy of the lower bounds
   * \param upp is the DoubleVector that will contain a copy of the upper bounds
   * \param opt is the IntVector that will contain a copy of the optimise flags
   */
  void getVectors(ParameterVector& sw, DoubleVector& val,
    DoubleVector& low, DoubleVector& upp, IntVector& opt);
  /**
   * \brief This function will return a copy of the value of the variables
   * \param val is the DoubleVector that will contain a copy of the values
   */
  void getValues(DoubleVector& val);
  /**
   * \brief This function will return a copy of the names of the variables
   * \param sw is the ParameterVector that will contain a copy of the names
   */
  void getSwitches(ParameterVector& sw);
  /**
   * \brief This is the function that will read the next line of the data from the input file
   * \note This function is only used when the data is in the 'repeated values' format
   */
  void readNextLine();
  /**
   * \brief This is the function that will read the data from the input file
   */
  void readFromFile();
  /**
   * \brief This function will check to see if there is more data to read in the input file
   * \return 1 if there is more data to read from the input file, 0 otherwise
   */
  int isDataLeft();
  /**
   * \brief This function will return the number of switches read from the input file
   * \return number of switches
   */
  int numSwitches() const { return switches.Size(); };
  /**
   * \brief This function will return the number of variables read from the input file
   * \return number of variables
   */
  int numVariables() const { return values.Size(); };
  /**
   * \brief This function will return the value of the flag used to denote that the 'repeated values' format is used in the input file
   * \return repeatedValues
   */
  int isRepeatedValues() { return repeatedValues; };
  /**
   * \brief This function will return the optimising flag for a switch in the input file
   * \param i is the index for the switch
   * \return optimise flag for the switch
   */
  int getOptimise(int i) const { return optimise[i]; };
  /**
   * \brief This function will return the value of a switch in the input file
   * \param i is the index for the switch
   * \return value of the switch
   */
  double getValue(int i) const { return values[i]; };
  /**
   * \brief This function will return the lower bound for a switch in the input file
   * \param i is the index for the switch
   * \return lower bound for the switch
   */
  double getLower(int i) const { return lowerbound[i]; };
  /**
   * \brief This function will return the upper bound for a switch in the input file
   * \param i is the index for the switch
   * \return upper bound for the switch
   */
  double getUpper(int i) const { return upperbound[i]; };
  /**
   * \brief This function will return the paramter name for a switch in the input file
   * \param i is the index for the switch
   * \return parameter name for the switch
   */
  Parameter getSwitch(int i) const { return switches[i]; };
private:
  /**
   * \brief This is the function that will read the header information from the input file
   */
  void readHeader();
  /**
   * \brief This is the CommentStream to read the initial values data from
   */
  CommentStream infile;
  /**
   * \brief This is the ifstream to read the initial values data from
   */
  ifstream tmpinfile;
  /**
   * \brief This is the ParameterVector used to store the parameters read from file
   */
  ParameterVector switches;
  /**
   * \brief This is the DoubleVector used to store the values of the parameters read from file
   */
  DoubleVector values;
  /**
   * \brief This is the DoubleVector used to store the lower bounds of the parameters read from file
   */
  DoubleVector lowerbound;
  /**
   * \brief This is the DoubleVector used to store the upper bounds of the parameters read from file
   */
  DoubleVector upperbound;
  /**
   * \brief This is the IntVector used to store the flag to denote whether to optimise the parameters read from file
   */
  IntVector optimise;
  /**
   * \brief This is the flag used to denote whether the data is in the 'repeated values' format or not
   */
  int repeatedValues;
};

#endif
