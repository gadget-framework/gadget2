#include "printinfo.h"
#include "gadget.h"

PrintInfo::PrintInfo() : printIter1(-2), printIter2(-2), forceprint(0),
  oprint(0), coprint(0), givenPrecision(0) {

  OutputFile = NULL;
  ColumnOutputFile = NULL;
  ParamOutFile = NULL;

  char tmpname[12];
  strncpy(tmpname, "", 12);
  strcpy(tmpname, "params.out");
  setParamOutFile(tmpname);
}

PrintInfo::PrintInfo(const PrintInfo& pi) {

  ParamOutFile = NULL;
  setParamOutFile(pi.ParamOutFile);

  OutputFile = NULL;
  ColumnOutputFile = NULL;
  if (pi.OutputFile != NULL)
    setOutputFile(pi.OutputFile);
  if (pi.ColumnOutputFile != NULL)
    setColumnOutputFile(pi.ColumnOutputFile);

  forceprint = pi.forceprint;
  oprint = pi.oprint;
  coprint = pi.coprint;
  givenPrecision = pi.givenPrecision;
  printIter1 = pi.printIter1;
  printIter2 = pi.printIter2;
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

void PrintInfo::setOutputFile(char* filename) {
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

void PrintInfo::setColumnOutputFile(char* filename) {
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

void PrintInfo::setParamOutFile(char* filename) {
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

void PrintInfo::checkNumbers() {
  if (printIter2 < 0)
    printIter2 = 5;
  if (printIter1 < 0)
    printIter1 = 1;
  if (givenPrecision < 0)
    givenPrecision = 0;
}
