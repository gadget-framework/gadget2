#ifndef printinfo_h
#define printinfo_h

#include "gadget.h"

class PrintInfo {
public:
  PrintInfo();
  PrintInfo(const PrintInfo& pi);
  ~PrintInfo();
  void SetParamOutFile(const char* const filename);
  void SetOutputFile(const char* const filename);
  void SetColumnOutputFile(const char* const filename);
  void SetSurveyFile(const char* const filename);
  void SetCatchFile(const char* const filename);
  void SetStomachFile(const char* const filename);
  int Print() const { return(PrintInterVal1 > 0 ? 1 : 0); };
  int PrintinColumns() const { return(PrintInterVal2 > 0 ? 1 : 0); };
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
};

#endif
