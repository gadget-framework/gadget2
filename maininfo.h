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
  void ShowCorrectUsage(char* error);
  void ShowUsage();
  /**
   * \brief This is the default MainInfo constructor
   */
  MainInfo();
  /**
   * \brief This is the default MainInfo destructor
   */
  ~MainInfo();
  void Read(int aNumber, char* const aVector[]);
  void Read(CommentStream& file);
  void OpenOptinfofile(char* filename);
  void CloseOptinfofile();
  void SetPrintLikelihoodFilename(char* filename);
  void SetPrintInitialCondFilename(char* filename);
  void SetPrintFinalCondFilename(char* filename);
  void SetInitialCommentFilename(char* filename);
  void SetMainGadgetFilename(char* filename);
  CommentStream OptinfoCommentFile;
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
  PrintInfo printinfo;
  /**
   * \brief This is the flag used to denote that the optimisation parameters have been given
   */
  int OptInfoFileisGiven;
  /**
   * \brief This is the flag used to denote that the initial values for the model parameters have been given
   */
  int InitialCondareGiven;
  int CalcLikelihood;
  int Optimize;
  int Stochastic;
  int PrintInitialcond;
  int PrintFinalcond;
  int PrintLikelihoodInfo;
  int Net;
};

#endif
