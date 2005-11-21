#include "printinfo.h"
#include "errorhandler.h"
#include "gadget.h"

extern ErrorHandler handle;

PrintInfo::PrintInfo() : printIter1(-2), printIter2(-2),
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

void PrintInfo::checkPrintInfo(int network) {
  if (printIter2 < 0)
    printIter2 = 5;
  if (printIter1 < 0)
    printIter1 = 1;
  if (givenPrecision < 0)
    givenPrecision = 0;

  if ((printoutput == 1) && (printcolumn == 1)) {
    handle.logMessage(LOGWARN, "\nWarning - Gadget has been started with both the -o switch and the -co switch\nHowever, it is not possible to print the output in both formats\nGadget will print the output in the standard format (and ignore the -co switch)");
    printcolumn = 0;
  }

  //JMB check to see if we can actually open required files ...
  ofstream tmpfile;
  if (network == 0) {
    tmpfile.open(strParamOutFile, ios::out);
    handle.checkIfFailure(tmpfile, strParamOutFile);
    tmpfile.close();
    tmpfile.clear();
  }
  if (printoutput == 1) {
    tmpfile.open(strOutputFile, ios::out);
    handle.checkIfFailure(tmpfile, strOutputFile);
    tmpfile.close();
    tmpfile.clear();
  }
  if (printcolumn == 1) {
    tmpfile.open(strColumnFile, ios::out);
    handle.checkIfFailure(tmpfile, strColumnFile);
    tmpfile.close();
    tmpfile.clear();
  }
}
