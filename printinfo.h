#ifndef printinfo_h
#define printinfo_h

#include "gadget.h"

class PrintInfo {
public:
  PrintInfo();
  PrintInfo(const PrintInfo& pi);
  ~PrintInfo();
  void SetParamOutFile(char* filename);
  void SetOutputFile(char* filename);
  void SetColumnOutputFile(char* filename);
  void SetSurveyFile(char* filename);
  void SetCatchFile(char* filename);
  void SetStomachFile(char* filename);
  void CheckNumbers();
  int Print() const { return oprint; };
  int PrintinColumns() const { return coprint; };
  int PrintInterVal1;
  int PrintInterVal2;
  char* ParamOutFile;
  char* OutputFile;
  char* ColumnOutputFile;
  char* surveyfile;
  char* catchfile;
  char* stomachfile;
  int forcePrint;
  int surveyprint;
  int catchprint;
  int stomachprint;
  int oprint;
  int coprint;
  int givenPrecision;
};

#endif
