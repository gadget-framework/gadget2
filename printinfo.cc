#include "printinfo.h"
#include "gadget.h"

PrintInfo::PrintInfo() : PrintInterVal1(-2), PrintInterVal2(-2), forcePrint(0),
  surveyprint(0), catchprint(0), stomachprint(0), oprint(0), coprint(0), givenPrecision(0) {

  OutputFile = NULL;
  ColumnOutputFile = NULL;
  ParamOutFile = NULL;
  surveyfile = NULL;
  catchfile = NULL;
  stomachfile = NULL;

  char tmpname[12];
  strncpy(tmpname, "", 12);
  strcpy(tmpname, "params.out");
  SetParamOutFile(tmpname);
  strncpy(tmpname, "", 12);
  strcpy(tmpname, "stomach.out");
  SetStomachFile(tmpname);
  strncpy(tmpname, "", 12);
  strcpy(tmpname, "survey.out");
  SetSurveyFile(tmpname);
  strncpy(tmpname, "", 12);
  strcpy(tmpname, "catch.out");
  SetCatchFile(tmpname);
}

PrintInfo::PrintInfo(const PrintInfo& pi) {

  surveyfile = NULL;
  catchfile = NULL;
  stomachfile = NULL;
  ParamOutFile = NULL;
  SetSurveyFile(pi.surveyfile);
  SetCatchFile(pi.catchfile);
  SetStomachFile(pi.stomachfile);
  SetParamOutFile(pi.ParamOutFile);

  OutputFile = NULL;
  ColumnOutputFile = NULL;
  if (pi.OutputFile != NULL)
    SetOutputFile(pi.OutputFile);
  if (pi.ColumnOutputFile != NULL)
    SetColumnOutputFile(pi.ColumnOutputFile);

  forcePrint = pi.forcePrint;
  surveyprint = pi.surveyprint;
  catchprint = pi.catchprint;
  stomachprint = pi.stomachprint;
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
  if (surveyfile != NULL) {
    delete[] surveyfile;
    surveyfile = NULL;
  }
  if (catchfile != NULL) {
    delete[] catchfile;
    catchfile = NULL;
  }
  if (stomachfile != NULL) {
    delete[] stomachfile;
    stomachfile = NULL;
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

void PrintInfo::SetSurveyFile(char* filename) {
  if (surveyfile != NULL) {
    delete[] surveyfile;
    surveyfile = NULL;
  }
  if (filename != NULL) {
    surveyfile = new char[strlen(filename) + 1];
    strcpy(surveyfile, filename);
  } else {
    surveyfile = NULL;
  }
}

void PrintInfo::SetCatchFile(char* filename) {
  if (catchfile != NULL) {
    delete[] catchfile;
    catchfile = NULL;
  }
  if (filename != NULL) {
    catchfile = new char[strlen(filename) + 1];
    strcpy(catchfile, filename);
  } else {
    catchfile = NULL;
  }
}

void PrintInfo::SetStomachFile(char* filename) {
  if (stomachfile != NULL) {
    delete[] stomachfile;
    stomachfile = NULL;
  }
  if (filename != NULL) {
    stomachfile = new char[strlen(filename) + 1];
    strcpy(stomachfile, filename);
  } else {
    stomachfile = NULL;
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
