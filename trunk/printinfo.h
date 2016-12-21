#ifndef printinfo_h
#define printinfo_h

#include "gadget.h"

/**
 * \class PrintInfo
 * \brief This is the class used to store information about what format the model parameters should be written to file
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
   * \brief This function will set the interval between iterations to be used when printing the model parameters to file
   * \param print is the interval
   */
  void setPrintIteration(int print) { printiter = print; };
  /**
   * \brief This function will set the precision to be used when printing the model parameters
   * \param prec is the precision
   */
  void setPrecision(int prec) { givenPrecision = prec; };
  /**
   * \brief This function will check the printing intervals and precision values that have been set, and override them with default vakues if required
   * \param net is the flag to denote whether the current model run is a network run or not
   */
  void checkPrintInfo(int net);
  /**
   * \brief This function will return the filename that the model parameters will be written to
   * \return filename
   */
  char* getOutputFile() const { return strOutputFile; };
  /**
   * \brief This function will return the filename that the final parameter values will be written to
   * \return filename
   */
  char* getParamOutFile() const { return strParamOutFile; };
  /**
   * \brief This function will return the flag used to determine whether the model parameters should be printed to file
   * \return flag
   */
  int getPrint() const { return printoutput; };
  /**
   * \brief This function will return the precision to be used when printing the model parameters
   * \return precision
   */
  int getPrecision() const { return givenPrecision; };
  /**
   * \brief This function will return the interval between iterations to be used when printing the model parameters to file
   * \return print interval
   */
  int getPrintIteration() const { return printiter; };
private:
  /**
   * \brief This is the name of the file that the final parameter values will be written to
   */
  char* strParamOutFile;
  /**
   * \brief This is the name of the file that the model parameters values will be written to
   */
  char* strOutputFile;
  /**
   * \brief This is the interval between iterations that the model parameters should be written to file
   */
  int printiter;
  /**
   * \brief This is the flag used to denote whether the model parameters should be printed or not
   */
  int printoutput;
  /**
   * \brief This is the precision that should be used when printing the model parameters
   */
  int givenPrecision;
};

#endif
