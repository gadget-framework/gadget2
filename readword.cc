#include "keeper.h"
#include "readword.h"
#include "errorhandler.h"
#include "gadget.h"

void readWordAndVariable(CommentStream& infile, const char* str, double& number) {
  ErrorHandler handle;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  infile >> text;
  if (strcasecmp(text, str) == 0)
    infile >> number >> ws;
  else
    handle.Unexpected(str, text);
  if (infile.fail())
    handle.Failure(str);
}

void readWordAndVariable(CommentStream& infile, const char* str, int& number) {
  ErrorHandler handle;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  infile >> text;
  if (strcasecmp(text, str) == 0)
    infile >> number >> ws;
  else
    handle.Unexpected(str, text);
  if (infile.fail())
    handle.Failure(str);
}

void readWordAndValue(CommentStream& infile, const char* str, char* value) {
  ErrorHandler handle;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  infile >> text;
  if (strcasecmp(text, str) == 0)
    infile >> value >> ws;
  else
    handle.Unexpected(str, text);
  if (infile.fail())
    handle.Failure(str);
}

void readWordAndTwoVariables(CommentStream& infile, const char* str, double& number1, double& number2) {
  ErrorHandler handle;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  infile >> text;
  if (strcasecmp(text, str) == 0)
    infile >> number1 >> number2 >> ws;
  else
    handle.Unexpected(str, text);
  if (infile.fail())
    handle.Failure(str);
}

void readWordAndTwoVariables(CommentStream& infile, const char* str, int& number1, int& number2) {
  ErrorHandler handle;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  infile >> text;
  if (strcasecmp(text, str) == 0)
    infile >> number1 >> number2 >> ws;
  else
    handle.Unexpected(str, text);
  if (infile.fail())
    handle.Failure(str);
}
