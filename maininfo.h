#ifndef maininfo_h
#define maininfo_h

#include "gadget.h"
#include "commentstream.h"
#include "printinfo.h"

/**
 * \class MainInfo
 * \brief This is the class used to store information for the current model given from the command-line
 */
class MainInfo {
public:
  /**
   * \brief This is the MainInfo constructor
   */
  MainInfo();
  /**
   * \brief This is the default MainInfo destructor
   */
  ~MainInfo();
  /**
   * \brief This function will print an error message, brief help information and then exit
   * \param error is the error message
   */
  void showCorrectUsage(char* error);
  /**
   * \brief This function will print help information and then exit
   */
  void showUsage();
  /**
   * \brief This function will check the options that have been specified on the commandline
   * \param inputdir is the name of the directory containing the input files to the model
   * \param workingdir is the name of the directory used for the output from the model
   */
  void checkUsage(const char* const inputdir, const char* const workingdir);
  /**
   * \brief This function will read input from the command line
   * \param aNumber is the number of command line entries
   * \param aVector contains the command line entries
   */
  void read(int aNumber, char* const aVector[]);
  /**
   * \brief This function will store the filename that the initial model information will be written to
   * \param filename is the name of the file
   */
  void setPrintInitialFile(char* filename);
  /**
   * \brief This function will store the filename that the final model information will be written to
   * \param filename is the name of the file
   */
  void setPrintFinalFile(char* filename);
  /**
   * \brief This function will store the filename that the initial values for the model parameters will be read from
   * \param filename is the name of the file
   */
  void setInitialParamFile(char* filename);
  /**
   * \brief This function will store the filename that the main model information will be read from
   * \param filename is the name of the file
   */
  void setMainGadgetFile(char* filename);
  /**
   * \brief This function will store the filename that the optimisation information will be read from
   * \param filename is the name of the file
   */
  void setOptInfoFile(char* filename);
  /**
   * \brief This function will return the flag used to determine whether the likelihood score should be optimised
   * \return flag
   */
  int runOptimise() const { return runoptimise; };
  /**
   * \brief This function will return the flag used to determine whether a simulation run should be performed
   * \return flag
   */
  int runStochastic() const { return runstochastic; };
  /**
   * \brief This function will return the flag used to determine whether the current simulation has been started using paramin
   * \return flag
   */
  int runNetwork() const { return runnetwork; };
  /**
   * \brief This function will return the flag used to determine whether the current simulation should print the model output
   * \return flag
   */
  int runPrint() const { return runprint; };
  /**
   * \brief This function will return the flag used to determine whether the optimisation parameters have been given
   * \return flag
   */
  int getOptInfoGiven() const { return givenOptInfo; };
  /**
   * \brief This function will return the flag used to determine whether the initial values for the model parameters have been given
   * \return flag
   */
  int getInitialParamGiven() const { return givenInitialParam; };
  /**
   * \brief This function will return the flag used to force the model to print the model output to file
   * \return flag
   */
  int getForcePrint() const { return forceprint; };
  /**
   * \brief This function will return the PrintInfo used to store printing information
   * \return pi, the PrintInfo containing the printing information
   */
  PrintInfo getPI() const { return printinfo; };
  /**
   * \brief This function will return the flag used to determine whether the initial model information should be printed to file
   * \return flag
   */
  int printInitial() const { return printInitialInfo; };
  /**
   * \brief This function will return the flag used to determine whether the final model information should be printed to file
   * \return flag
   */
  int printFinal() const { return printFinalInfo; };
  /**
   * \brief This function will return the filename that the initial values for the model parameters will be read from
   * \return filename
   */
  char* getInitialParamFile() const { return strInitialParamFile; };
  /**
   * \brief This function will return the filename that the initial model information will be written to
   * \return filename
   */
  char* getPrintInitialFile() const { return strPrintInitialFile; };
  /**
   * \brief This function will return the filename that the final model information will be written to
   * \return filename
   */
  char* getPrintFinalFile() const { return strPrintFinalFile; };
  /**
   * \brief This function will return the filename that the main model information will be read from
   * \return filename
   */
  char* getMainGadgetFile() const { return strMainGadgetFile; };
  /**
   * \brief This function will return the filename that the optimisation information will be read from
   * \return filename
   */
  char* getOptInfoFile() const { return strOptInfoFile; };
  /**
   * \brief This function will return the maximum ratio of a stock that can be consumed on any given timestep
   * \return maxratio
   */
  double getMaxRatio() const { return maxratio; };
private:
  /**
   * \brief This function will read input from a file instead of the command line
   * \param file is the CommentStream to read from
   */
  void read(CommentStream& file);
  /**
   * \brief This is the name of the file that optimisation parameters will be read from
   */
  char* strOptInfoFile;
  /**
   * \brief This is the name of the file that initial values for the model parameters will be read from
   */
  char* strInitialParamFile;
  /**
   * \brief This is the name of the file that the initial model information will be written to
   */
  char* strPrintInitialFile;
  /**
   * \brief This is the name of the file that the final model information will be written to
   */
  char* strPrintFinalFile;
  /**
   * \brief This is the name of the file that the main model information will be read from
   */
  char* strMainGadgetFile;
  /**
   * \brief This is the PrintInfo used to store information about the format the model parameters should be written to file
   */
  PrintInfo printinfo;
  /**
   * \brief This is the flag used to denote whether the optimisation parameters have been given or not
   */
  int givenOptInfo;
  /**
   * \brief This is the flag used to denote whether the initial values for the model parameters have been given or not
   */
  int givenInitialParam;
  /**
   * \brief This is the flag used to denote whether the likelihood score should be optimised or not
   */
  int runoptimise;
  /**
   * \brief This is the flag used to denote whether a simulation run should be performed or not
   */
  int runstochastic;
  /**
   * \brief This is the flag used to denote whether the current simulation has been started using paramin (PVM) or not
   */
  int runnetwork;
  /**
   * \brief This is the flag used to denote whether the current simulation should print model output or not
   */
  int runprint;
  /**
   * \brief This is the flag used to override the default settings and force the model to print model output at the end of an optimisation run
   */
  int forceprint;
  /**
   * \brief This is the flag used to denote whether the initial model information should be printed or not
   */
  int printInitialInfo;
  /**
   * \brief This is the flag used to denote whether the final model information should be printed or not
   */
  int printFinalInfo;
  /**
   * \brief This is the value used to decide on the level of warnings displayed during the model run
   */
  int printLogLevel;
  /**
   * \brief This is the maximum ratio of a stock that can be consumed on any given timestep
   */
  double maxratio;
};

#endif
