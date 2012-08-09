#include "printinfo.h"
#include "errorhandler.h"
#include "gadget.h"
#include "global.h"

PrintInfo::PrintInfo() : printiter(0), printoutput(0), givenPrecision(0) {

  strOutputFile = NULL;
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

  printoutput = pi.printoutput;
  givenPrecision = pi.givenPrecision;
  printiter = pi.printiter;
}

PrintInfo::~PrintInfo() {
  if (strOutputFile != NULL) {
    delete[] strOutputFile;
    strOutputFile = NULL;
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

void PrintInfo::checkPrintInfo(int network) {
  if (printiter < 1)
    printiter = 1;
  if (givenPrecision < 0)
    givenPrecision = 0;

  //JMB check to see if we can actually open required files ...
  ofstream tmpfile;
  if (!network) {
    tmpfile.open(strParamOutFile, ios::out);
    handle.checkIfFailure(tmpfile, strParamOutFile);
    tmpfile.close();
    tmpfile.clear();
  }
  if (printoutput) {
    tmpfile.open(strOutputFile, ios::out);
    handle.checkIfFailure(tmpfile, strOutputFile);
    tmpfile.close();
    tmpfile.clear();
  }
}
