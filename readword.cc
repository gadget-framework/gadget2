#include "readword.h"
#include "errorhandler.h"
#include "gadget.h"

extern ErrorHandler handle;

void readWordAndVariable(CommentStream& infile, const char* str, double& number) {
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  infile >> text;
  if (strcasecmp(text, str) == 0)
    infile >> number >> ws;
  else
    handle.logFileUnexpected(LOGFAIL, str, text);
}

void readWordAndVariable(CommentStream& infile, const char* str, int& number) {
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  infile >> text;
  if (strcasecmp(text, str) == 0)
    infile >> number >> ws;
  else
    handle.logFileUnexpected(LOGFAIL, str, text);
}

void readWordAndValue(CommentStream& infile, const char* str, char* value) {
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  infile >> text;
  if (strcasecmp(text, str) == 0)
    infile >> value >> ws;
  else
    handle.logFileUnexpected(LOGFAIL, str, text);
}

void readWordAndVariable(CommentStream& infile, const char* str, Formula& formula) {
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  infile >> text;
  if (strcasecmp(text, str) == 0)
    infile >> formula >> ws;
  else
    handle.logFileUnexpected(LOGFAIL, str, text);
}

void readWordAndTimeVariable(CommentStream& infile, const char* str, TimeVariable& timevariable,
  const TimeClass* const TimeInfo, Keeper* const keeper) {
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  infile >> text;
  if (strcasecmp(text, str) == 0)
    timevariable.read(infile, TimeInfo, keeper);
  else
    handle.logFileUnexpected(LOGFAIL, str, text);
}

void readWordAndTwoVariables(CommentStream& infile, const char* str, double& number1, double& number2) {
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  infile >> text;
  if (strcasecmp(text, str) == 0)
    infile >> number1 >> number2 >> ws;
  else
    handle.logFileUnexpected(LOGFAIL, str, text);
}

void readWordAndTwoVariables(CommentStream& infile, const char* str, int& number1, int& number2) {
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  infile >> text;
  if (strcasecmp(text, str) == 0)
    infile >> number1 >> number2 >> ws;
  else
    handle.logFileUnexpected(LOGFAIL, str, text);
}
