#include "printinfo.h"
#include "gadget.h"

PrintInfo::PrintInfo() : printIter1(-2), printIter2(-2), forceprint(0),
  printoutput(0), printcolumn(0), givenPrecision(0) {

  strOutputFile = NULL;
  strColumnFile = NULL;
  strParamOutFile = NULL;

  char tmpname[12];
  strncpy(tmpname, "", 12);
  strcpy(tmpname, "params.out");
  setParamOutFile(tmpname);
}

PrintInfo::PrintInfo(const PrintInfo& pi) {

  strParamOutFile = NULL;
  setParamOutFile(pi.strParamOutFile);

  strOutputFile = NULL;
  if (pi.strOutputFile != NULL)
    setOutputFile(pi.strOutputFile);
  strColumnFile = NULL;
  if (pi.strColumnFile != NULL)
    setColumnOutputFile(pi.strColumnFile);

  forceprint = pi.forceprint;
  printoutput = pi.printoutput;
  printcolumn = pi.printcolumn;
  givenPrecision = pi.givenPrecision;
  printIter1 = pi.printIter1;
  printIter2 = pi.printIter2;
}

PrintInfo::~PrintInfo() {
  if (strOutputFile != NULL) {
    delete[] strOutputFile;
    strOutputFile = NULL;
  }
  if (strColumnFile != NULL) {
    delete[] strColumnFile;
    strColumnFile = NULL;
  }
  if (strParamOutFile != NULL) {
    delete[] strParamOutFile;
    strParamOutFile = NULL;
  }
}

void PrintInfo::setOutputFile(char* filename) {
  if (strOutputFile != NULL) {
    delete[] strOutputFile;
    strOutputFile = NULL;
  }
  if (filename != NULL) {
    strOutputFile = new char[strlen(filename) + 1];
    strcpy(strOutputFile, filename);
    printoutput = 1;
  } else {
    strOutputFile = NULL;
  }
}

void PrintInfo::setColumnOutputFile(char* filename) {
  if (strColumnFile != NULL) {
    delete[] strColumnFile;
    strColumnFile = NULL;
  }
  if (filename != NULL) {
    strColumnFile = new char[strlen(filename) + 1];
    strcpy(strColumnFile, filename);
    printcolumn = 1;
  } else {
    strColumnFile = NULL;
  }
}

void PrintInfo::setParamOutFile(char* filename) {
  if (strParamOutFile != NULL) {
    delete[] strParamOutFile;
    strParamOutFile = NULL;
  }
  if (filename != NULL) {
    strParamOutFile = new char[strlen(filename) + 1];
    strcpy(strParamOutFile, filename);
  } else {
    strParamOutFile = NULL;
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
