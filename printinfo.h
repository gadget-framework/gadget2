#ifndef printinfo_h
#define printinfo_h

#include "gadget.h"

/**
 * \class PrintInfo
 * \brief This is the class used store information about what format the model parameters should be written to file
 */
class PrintInfo {
public:
  /**
   * \brief This is the PrintInfo constructor
   */
  PrintInfo();
  /**
   * \brief This is the PrintInfo constructor that creates a copy of an existing PrintInfo
   * \param pi is the PrintInfo to copy
   */
  PrintInfo(const PrintInfo& pi);
  /**
   * \brief This is the default MainInfo destructor
   */
  ~PrintInfo();
  /**
   * \brief This function will store the filename that the final parameter values will be written to
   * \param filename is the name of the file
   */
  void setParamOutFile(char* filename);
  /**
   * \brief This function will store the filename that the model parameters will be written to
   * \param filename is the name of the file
   */
  void setOutputFile(char* filename);
  /**
   * \brief This function will store the filename that the model parameters will be written to in a column format
   * \param filename is the name of the file
   */
  void setColumnOutputFile(char* filename);
  /**
   * \brief This function will set the flag used to force the model to print parameter values to file
   * \param print is the value of the flag (0 or 1)
   */
  void setForcePrint(int print) { forceprint = print; };
  /**
   * \brief This function will set the interval between iterations to be used when printing the model parameters to file
   * \param print is the interval
   */
  void setPrint1(int print) { printIter1 = print; };
  /**
   * \brief This function will set the interval between iterations to be used when printing the model parameters to file in a column format
   * \param print is the interval
   */
  void setPrint2(int print) { printIter2 = print; };
  /**
   * \brief This function will set the precision to be used when printing the model parameters
   * \param prec is the precision
   */
  void setPrecision(int prec) { givenPrecision = prec; };
  /**
   * \brief This function will check the printing intervals and precision values that have been set, and override them with default vakues if required
   */
  void checkNumbers();
  /**
   * \brief This function will return the filename that the model parameters will be written to
   * \return filename
   */
  char* getOutputFile() const { return OutputFile; };
  /**
   * \brief This function will return the filename that the model parameters will be written to in a column format
   * \return filename
   */
  char* getColumnOutputFile() const { return ColumnOutputFile; };
  /**
   * \brief This function will return the filename that the final parameter values will be written to
   * \return filename
   */
  char* getParamOutFile() const { return ParamOutFile; };
  /**
   * \brief This function will return the flag used to determine whether the model parameters should be printed to file
   * \return flag
   */
  int Print() const { return oprint; };
  /**
   * \brief This function will return the flag used to determine whether the model parameters should be printed to file in a column format
   * \return flag
   */
  int PrintinColumns() const { return coprint; };
  /**
   * \brief This function will return the flag used to force the model to print parameter values to file
   * \return flag
   */
  int getForcePrint() const { return forceprint; };
  /**
   * \brief This function will return the precision to be used when printing the model parameters
   * \return precision
   */
  int getPrecision() const { return givenPrecision; };
  /**
   * \brief This function will return the interval between iterations to be used when printing the model parameters to file
   * \return print interval
   */
  int getPrint1() const { return printIter1; };
  /**
   * \brief This function will return the interval between iterations to be used when printing the model parameters to file in a column format
   * \return print interval
   */
  int getPrint2() const { return printIter2; };
private:
  /**
   * \brief This is the name of the file that the final parameter values will be written to
   */
  char* ParamOutFile;
  /**
   * \brief This is the name of the file that the model parameters values will be written to
   */
  char* OutputFile;
  /**
   * \brief This is the name of the file that the model parameters values will be written to in a column format
   */
  char* ColumnOutputFile;
  /**
   * \brief This is the interval between iterations that the model parameters should be written to file
   */
  int printIter1;
  /**
   * \brief This is the interval between iterations that the model parameters should be written to file in a column format
   */
  int printIter2;
  /**
   * \brief This is the flag used to override the default settings and force the model to print the parameter values to file
   */
  int forceprint;
  /**
   * \brief This is the flag used to denote whether the model parameters should be printed or not
   */
  int oprint;
  /**
   * \brief This is the flag used to denote whether the model parameters should be printed in a column format or not
   */
  int coprint;
  /**
   * \brief This is the precision that should be used when printing the model parameters
   */
  int givenPrecision;
};

#endif
