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
  void setSurveyPrint(int print) { surveyprint = print; };
  void setCatchPrint(int print) { catchprint = print; };
  void setStomachPrint(int print) { stomachprint = print; };
  void setForcePrint(int print) { forceprint = print; };
  void setPrint1(int print) { PrintInterVal1 = print; };
  void setPrint2(int print) { PrintInterVal2 = print; };
  void setPrecision(int prec) { givenPrecision = prec; };
  void CheckNumbers();
  char* oFile() const { return OutputFile; };
  char* coFile() const { return ColumnOutputFile; };
  char* paramFile() const { return ParamOutFile; };
  int Print() const { return oprint; };
  int PrintinColumns() const { return coprint; };
  int catchPrint() const { return catchprint; };
  int surveyPrint() const { return surveyprint; };
  int stomachPrint() const { return stomachprint; };
  int forcePrint() const { return forceprint; };
  int precision() const { return givenPrecision; };
  int print1() const { return PrintInterVal1; };
  int print2() const { return PrintInterVal2; };
private:
  char* ParamOutFile;
  char* OutputFile;
  char* ColumnOutputFile;
  char* surveyfile;
  char* catchfile;
  char* stomachfile;
  int PrintInterVal1;
  int PrintInterVal2;
  int forceprint;
  int surveyprint;
  int catchprint;
  int stomachprint;
  int oprint;
  int coprint;
  int givenPrecision;
};

#endif
