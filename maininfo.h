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
   * \brief This is the default MainInfo constructor
   */
  MainInfo();
  /**
   * \brief This is the default MainInfo destructor
   */
  ~MainInfo();
  void ShowCorrectUsage(char* error);
  void ShowUsage();
  void Read(int aNumber, char* const aVector[]);
  void OpenOptinfofile(char* filename);
  void CloseOptinfofile();
  void SetPrintLikelihoodFilename(char* filename);
  void SetPrintInitialCondFilename(char* filename);
  void SetPrintFinalCondFilename(char* filename);
  void SetInitialCommentFilename(char* filename);
  void SetMainGadgetFilename(char* filename);
  int Optimize() const { return optimize; };
  int CalcLikelihood() const { return calclikelihood; };
  int Stochastic() const { return stochastic; };
  int Net() const { return netrun; };
  int OptInfoGiven() const { return OptInfoFileisGiven; };
  int InitCondGiven() const { return InitialCondareGiven; };
  PrintInfo getPI() const { return printinfo; };
  int printInitial() const { return PrintInitialcond; };
  int printFinal() const { return PrintFinalcond; };
  int printLikelihood() const { return PrintLikelihoodInfo; };
  CommentStream& optFile() { return OptinfoCommentFile; };
  char* initParamFile() const { return InitialCommentFilename; };
  char* initFile() const { return PrintInitialCondFilename; };
  char* finalFile() const { return PrintFinalCondFilename; };
  char* likelihoodFile() const { return PrintLikelihoodFilename; };
  char* mainFile() const { return MainGadgetFilename; };
private:
  void Read(CommentStream& file);
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
  int calclikelihood;
  int optimize;
  int stochastic;
  int PrintInitialcond;
  int PrintFinalcond;
  int PrintLikelihoodInfo;
  int netrun;
};

#endif
