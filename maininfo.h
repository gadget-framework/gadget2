#ifndef maininfo_h
#define maininfo_h

#include "optinfo.h"
#include "printinfo.h"

/**
 * \class MainInfo
 * \brief This is the class used store information for the current model given from the command-line
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
   * \brief This function will read input from the command line
   * \param aNumber is the number of command line entries
   * \param aVector contains the command line entries
   */
  void read(int aNumber, char* const aVector[]);
  /**
   * \brief This function will open the file to read optimisation information from
   * \param filename is the name of the file
   */
  void OpenOptinfofile(char* filename);
  /**
   * \brief This function will close the file after the optimisation information has been read
   */
  void CloseOptinfofile();
  /**
   * \brief This function will store the filename that the likelihood information will be written to
   * \param filename is the name of the file
   */
  void setPrintLikelihoodFilename(char* filename);
  /**
   * \brief This function will store the filename that the initial model information will be written to
   * \param filename is the name of the file
   */
  void setPrintInitialCondFilename(char* filename);
  /**
   * \brief This function will store the filename that the final model information will be written to
   * \param filename is the name of the file
   */
  void setPrintFinalCondFilename(char* filename);
  /**
   * \brief This function will store the filename that the initial values for the model parameters will be read from
   * \param filename is the name of the file
   */
  void setInitialParamFilename(char* filename);
  /**
   * \brief This function will store the filename that the main model information will be read from
   * \param filename is the name of the file
   */
  void setMainGadgetFilename(char* filename);
  /**
   * \brief This function will return the flag used to determine whether the likelihood score should be optimised
   * \return flag
   */
  int Optimize() const { return optimize; };
  /**
   * \brief This function will return the flag used to determine whether the likelihood score should be calculated
   * \return flag
   */
  int CalcLikelihood() const { return calclikelihood; };
  /**
   * \brief This function will return the flag used to determine whether a stochastic run should be performed
   * \return flag
   */
  int Stochastic() const { return stochastic; };
  /**
   * \brief This function will return the flag used to determine whether the current simulation has been started using paramin
   * \return flag
   */
  int Net() const { return netrun; };
  /**
   * \brief This function will return the flag used to determine whether the optimisation parameters have been given
   * \return flag
   */
  int OptInfoGiven() const { return OptInfoFileisGiven; };
  /**
   * \brief This function will return the flag used to determine whether the initial values for the model parameters have been given
   * \return flag
   */
  int InitCondGiven() const { return InitialCondareGiven; };
  /**
   * \brief This function will return the PrintInfo used to store printing information
   * \return pi, the PrintInfo containing the printing information
   */
  PrintInfo getPI() const { return printinfo; };
  /**
   * \brief This function will return the flag used to determine whether the initial model information should be printed to file
   * \return flag
   */
  int printInitial() const { return PrintInitialcond; };
  /**
   * \brief This function will return the flag used to determine whether the final model information should be printed to file
   * \return flag
   */
  int printFinal() const { return PrintFinalcond; };
  /**
   * \brief This function will return the flag used to determine whether the likelihood information should be printed to file
   * \return flag
   */
  int printLikelihood() const { return PrintLikelihoodInfo; };
  /**
   * \brief This function will return the CommentStream that all the optimisation information gets read from
   * \return optinfofile, the CommentStream to read from
   */
  CommentStream& optFile() { return OptinfoCommentFile; };
  /**
   * \brief This function will return the filename that the initial values for the model parameters will be read from
   * \return filename
   */
  char* getInitialParamFilename() const { return InitialCommentFilename; };
  /**
   * \brief This function will return the filename that the initial model information will be written to
   * \return filename
   */
  char* getPrintInitialCondFilename() const { return PrintInitialCondFilename; };
  /**
   * \brief This function will return the filename that the final model information will be written to
   * \return filename
   */
  char* getPrintFinalCondFilename() const { return PrintFinalCondFilename; };
  /**
   * \brief This function will return the filename that the likelihood information will be written to
   * \return filename
   */
  char* getPrintLikelihoodFilename() const { return PrintLikelihoodFilename; };
  /**
   * \brief This function will return the filename that the main model information will be read from
   * \return filename
   */
  char* getMainGadgetFilename() const { return MainGadgetFilename; };
private:
  /**
   * \brief This function will read input from a file instead of the command line
   * \param file is the CommentStream to read from
   */
  void read(CommentStream& file);
  /**
   * \brief This is the CommentStream that all the optimisation information gets read from
   */
  CommentStream OptinfoCommentFile;
  /**
   * \brief This ifstream is the file that all the optimisation information gets read from
   */
  ifstream OptinfoFile;
  /**
   * \brief This is the name of the file that likelihood information will be written to
   */
  char* PrintLikelihoodFilename;
  /**
   * \brief This is the name of the file that optimisation parameters will be read from
   */
  char* OptinfoFilename;
  /**
   * \brief This is the name of the file that initial values for the model parameters will be read from
   */
  char* InitialCommentFilename;
  /**
   * \brief This is the name of the file that the initial model information will be written to
   */
  char* PrintInitialCondFilename;
  /**
   * \brief This is the name of the file that the final model information will be written to
   */
  char* PrintFinalCondFilename;
  /**
   * \brief This is the name of the file that the main model information will be read from
   */
  char* MainGadgetFilename;
  /**
   * \brief This is the PrintInfo used to store information about the format the model parameters should be written to file
   */
  PrintInfo printinfo;
  /**
   * \brief This is the flag used to denote whether the optimisation parameters have been given or not
   */
  int OptInfoFileisGiven;
  /**
   * \brief This is the flag used to denote whether the initial values for the model parameters have been given or not
   */
  int InitialCondareGiven;
  /**
   * \brief This is the flag used to denote whether the likelihood score should be calculated or not
   */
  int calclikelihood;
  /**
   * \brief This is the flag used to denote whether the likelihood score should be optimised or not
   */
  int optimize;
  /**
   * \brief This is the flag used to denote whether a stochastic (single) run should be performed or not
   */
  int stochastic;
  /**
   * \brief This is the flag used to denote whether the initial model information should be printed or not
   */
  int PrintInitialcond;
  /**
   * \brief This is the flag used to denote whether the final model information should be printed or not
   */
  int PrintFinalcond;
  /**
   * \brief This is the flag used to denote whether the likelihood information should be printed or not
   */
  int PrintLikelihoodInfo;
  /**
   * \brief This is the flag used to denote whether the current simulation has been started using paramin (PVM) or not
   */
  int netrun;
};

#endif
