#include "printinfo.h"
#include "gadget.h"

PrintInfo::PrintInfo() : PrintInterVal1(-2), PrintInterVal2(-2), forceprint(0),
  oprint(0), coprint(0), givenPrecision(0) {

  OutputFile = NULL;
  ColumnOutputFile = NULL;
  ParamOutFile = NULL;

  char tmpname[12];
  strncpy(tmpname, "", 12);
  strcpy(tmpname, "params.out");
  SetParamOutFile(tmpname);
}

PrintInfo::PrintInfo(const PrintInfo& pi) {

  ParamOutFile = NULL;
  SetParamOutFile(pi.ParamOutFile);

  OutputFile = NULL;
  ColumnOutputFile = NULL;
  if (pi.OutputFile != NULL)
    SetOutputFile(pi.OutputFile);
  if (pi.ColumnOutputFile != NULL)
    SetColumnOutputFile(pi.ColumnOutputFile);

  forceprint = pi.forceprint;
  oprint = pi.oprint;
  coprint = pi.coprint;
  givenPrecision = pi.givenPrecision;
  PrintInterVal1 = pi.PrintInterVal1;
  PrintInterVal2 = pi.PrintInterVal2;
}

PrintInfo::~PrintInfo() {
  if (OutputFile != NULL) {
    delete[] OutputFile;
    OutputFile = NULL;
  }
  if (ColumnOutputFile != NULL) {
    delete[] ColumnOutputFile;
    ColumnOutputFile = NULL;
  }
  if (ParamOutFile != NULL) {
    delete[] ParamOutFile;
    ParamOutFile = NULL;
  }
}

void PrintInfo::SetOutputFile(char* filename) {
  if (OutputFile != NULL) {
    delete[] OutputFile;
    OutputFile = NULL;
  }
  if (filename != NULL) {
    OutputFile = new char[strlen(filename) + 1];
    strcpy(OutputFile, filename);
    oprint = 1;
  } else {
    OutputFile = NULL;
  }
}

void PrintInfo::SetColumnOutputFile(char* filename) {
  if (ColumnOutputFile != NULL) {
    delete[] ColumnOutputFile;
    ColumnOutputFile = NULL;
  }
  if (filename != NULL) {
    ColumnOutputFile = new char[strlen(filename) + 1];
    strcpy(ColumnOutputFile, filename);
    coprint = 1;
  } else {
    ColumnOutputFile = NULL;
  }
}

void PrintInfo::SetParamOutFile(char* filename) {
  if (ParamOutFile != NULL) {
    delete[] ParamOutFile;
    ParamOutFile = NULL;
  }
  if (filename != NULL) {
    ParamOutFile = new char[strlen(filename) + 1];
    strcpy(ParamOutFile, filename);
  } else {
    ParamOutFile = NULL;
  }
}

void PrintInfo::CheckNumbers() {
  if (PrintInterVal2 < 0)
    PrintInterVal2 = 5;
  if (PrintInterVal1 < 0)
    PrintInterVal1 = 1;
  if (givenPrecision < 0)
    givenPrecision = 0;
}
