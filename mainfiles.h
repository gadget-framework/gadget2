#ifndef mainfiles_h
#define mainfiles_h

#include "optinfo.h"
#include "printinfo.h"

class MainInfo {
public:
  void ShowCorrectUsage();
  MainInfo();
  void Read(int aNumber, char *const aVector[]);
  void Read(CommentStream& file);
  void OpenOptinfofile(char* filename);
  void CloseOptinfofile();
  void SetPrintLikelihoodFilename(char* filename);
  void SetPrintInitialCondFilename(char* filename);
  void SetPrintFinalCondFilename(char* filename);
  CommentStream optinfocommentfile;
  ifstream optinfofile;
  char* PrintLikelihoodFilename;
  char* optinfofilename;
  char* InitialCommentFilename;
  char* PrintInitialcondfilename;
  char* PrintFinalcondfilename;
  int OptInfoFileisGiven;
  int InitialCondareGiven;
  int CalcLikelihood;
  int Optimize;
  int Stochastic;
  int PrintInitialcond;
  int PrintFinalcond;
  int PrintLikelihoodInfo;
  PrintInfo printinfo;
  int Net;  //Change from AJ
};

#endif
