#include "printinfo.h"
#include "gadget.h"

PrintInfo::PrintInfo() : PrintInterVal1(-2), PrintInterVal2(-2), forcePrint(0),
  surveyprint(0), catchprint(0), stomachprint(0) {

  OutputFile = NULL;
  ColumnOutputFile = NULL;
  ParamOutFile = NULL; 
  surveyfile = NULL;
  catchfile = NULL;
  stomachfile = NULL;

  int len;
  len = strlen("lik.out");
  OutputFile = new char[len + 1];
  strcpy(OutputFile, "lik.out");
  len = strlen("lik.out.co");
  ColumnOutputFile = new char[len + 1];
  strcpy(ColumnOutputFile, "lik.out.co");

  SetParamOutFile("params.out");
  SetStomachFile("stomach.out");
  SetSurveyFile("survey.out");
  SetCatchFile("catch.out");
}

PrintInfo::PrintInfo(const PrintInfo& pi) {
  OutputFile = NULL;
  ColumnOutputFile = NULL;
  surveyfile = NULL;
  catchfile = NULL;
  stomachfile = NULL;
  ParamOutFile = NULL;
  PrintInterVal1 = pi.PrintInterVal1;
  PrintInterVal2 = pi.PrintInterVal2;

  int len;
  if (pi.OutputFile != NULL) {
    len = strlen(pi.OutputFile);
    OutputFile = new char[len + 1];
    strcpy(OutputFile, pi.OutputFile);
  }
  SetSurveyFile(pi.surveyfile);
  SetCatchFile(pi.catchfile);
  SetStomachFile(pi.stomachfile);
  SetParamOutFile(pi.ParamOutFile);
  if (pi.ColumnOutputFile != NULL) {
    len = strlen(pi.ColumnOutputFile);
    ColumnOutputFile = new char[len + 1];
    strcpy(ColumnOutputFile, pi.ColumnOutputFile);
  }
  forcePrint = pi.forcePrint;
  surveyprint = pi.surveyprint;
  catchprint = pi.catchprint;
  stomachprint = pi.stomachprint;
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

void PrintInfo::SetOutputFile(const char* const filename) {
  if (OutputFile != NULL) {
    delete[] OutputFile;
    OutputFile = NULL;
  }
  if (filename != NULL) {
    OutputFile = new char[strlen(filename) + 1];
    strcpy(OutputFile, filename);
  } else {
    OutputFile = NULL;
  }
  if (PrintInterVal1 < 0)
    PrintInterVal1 = 1;
}

void PrintInfo::SetSurveyFile(const char* const filename) {
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

void PrintInfo::SetCatchFile(const char* const filename) {
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

void PrintInfo::SetStomachFile(const char* const filename) {
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

void PrintInfo::SetColumnOutputFile(const char* const filename) {
  if (ColumnOutputFile != NULL) {
    delete[] ColumnOutputFile;
    ColumnOutputFile = NULL;
  }
  if (filename != NULL) {
    ColumnOutputFile = new char[strlen(filename) + 1];
    strcpy(ColumnOutputFile, filename);
  } else {
    ColumnOutputFile = NULL;
  }
  if (PrintInterVal2 < 0)
    PrintInterVal2 = 5;
}

void PrintInfo::SetParamOutFile(const char* const filename) {
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
