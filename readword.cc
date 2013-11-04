#include "readword.h"
#include "errorhandler.h"
#include "gadget.h"
#include "global.h"

void readWordAndVariable(CommentStream& infile, const char* str, double& number) {
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  infile >> text;
  if (strcasecmp(text, str) != 0)
    handle.logFileUnexpected(LOGFAIL, str, text);
  infile >> number >> ws;
}

void readWordAndVariable(CommentStream& infile, const char* str, int& number) {
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  infile >> text;
  if (strcasecmp(text, str) != 0)
    handle.logFileUnexpected(LOGFAIL, str, text);
  infile >> number >> ws;
}

void readWordAndValue(CommentStream& infile, const char* str, char* value) {
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  infile >> text;
  if (strcasecmp(text, str) != 0)
    handle.logFileUnexpected(LOGFAIL, str, text);
  infile >> value >> ws;
}

void readWordAndVariable(CommentStream& infile, const char* str, Formula& formula) {
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  infile >> text;
  if (strcasecmp(text, str) != 0)
    handle.logFileUnexpected(LOGFAIL, str, text);
  infile >> formula >> ws;
}

void readWordAndModelVariable(CommentStream& infile, const char* str, ModelVariable& modelvariable,
  const TimeClass* const TimeInfo, Keeper* const keeper) {
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  infile >> text;
  if (strcasecmp(text, str) != 0)
    handle.logFileUnexpected(LOGFAIL, str, text);
  modelvariable.read(infile, TimeInfo, keeper);
}

void readWordAndTwoVariables(CommentStream& infile, const char* str, double& number1, double& number2) {
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  infile >> text;
  if (strcasecmp(text, str) != 0)
    handle.logFileUnexpected(LOGFAIL, str, text);
  infile >> number1 >> number2 >> ws;
}

void readWordAndTwoVariables(CommentStream& infile, const char* str, int& number1, int& number2) {
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  infile >> text;
  if (strcasecmp(text, str) != 0)
    handle.logFileUnexpected(LOGFAIL, str, text);
  infile >> number1 >> number2 >> ws;
}
