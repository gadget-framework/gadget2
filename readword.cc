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
    handle.Unexpected(str, text);
}

void readWordAndVariable(CommentStream& infile, const char* str, int& number) {
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  infile >> text;
  if (strcasecmp(text, str) == 0)
    infile >> number >> ws;
  else
    handle.Unexpected(str, text);
}

void readWordAndValue(CommentStream& infile, const char* str, char* value) {
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  infile >> text;
  if (strcasecmp(text, str) == 0)
    infile >> value >> ws;
  else
    handle.Unexpected(str, text);
}

void readWordAndFormula(CommentStream& infile, const char* str, Formula& formula) {
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  infile >> text;
  if (strcasecmp(text, str) == 0)
    infile >> formula >> ws;
  else
    handle.Unexpected(str, text);
}

void readWordAndTwoVariables(CommentStream& infile, const char* str, double& number1, double& number2) {
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  infile >> text;
  if (strcasecmp(text, str) == 0)
    infile >> number1 >> number2 >> ws;
  else
    handle.Unexpected(str, text);
}

void readWordAndTwoVariables(CommentStream& infile, const char* str, int& number1, int& number2) {
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  infile >> text;
  if (strcasecmp(text, str) == 0)
    infile >> number1 >> number2 >> ws;
  else
    handle.Unexpected(str, text);
}
