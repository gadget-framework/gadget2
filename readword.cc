#include "keeper.h"
#include "readword.h"
#include "errorhandler.h"
#include "gadget.h"

void ReadWordAndVariable(CommentStream& infile, const char* str, double& number) {
  ErrorHandler handle;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  infile >> text;
  if (strcasecmp(text, str) == 0)
    infile >> number;
  else
    handle.Unexpected(str, text);
  if (infile.fail())
    handle.Failure(str);
}

void ReadWordAndVariable(CommentStream& infile, const char* str, int& number) {
  ErrorHandler handle;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  infile >> text;
  if (strcasecmp(text, str) == 0)
    infile >> number;
  else
    handle.Unexpected(str, text);
  if (infile.fail())
    handle.Failure(str);
}

void ReadWordAndValue(CommentStream& infile, const char* str, char* value) {
  ErrorHandler handle;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  infile >> text;
  if (strcasecmp(text, str) == 0)
    infile >> value;
  else
    handle.Unexpected(str, text);
  if (infile.fail())
    handle.Failure(str);
}

void ReadWordAndTwoVariables(CommentStream& infile, const char* str, double& number1, double& number2) {
  ErrorHandler handle;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  infile >> text;
  if (strcasecmp(text, str) == 0)
    infile >> number1 >> number2;
  else
    handle.Unexpected(str, text);
  if (infile.fail())
    handle.Failure(str);
}

void ReadWordAndTwoVariables(CommentStream& infile, const char* str, int& number1, int& number2) {
  ErrorHandler handle;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  infile >> text;
  if (strcasecmp(text, str) == 0)
    infile >> number1 >> number2;
  else
    handle.Unexpected(str, text);
  if (infile.fail())
    handle.Failure(str);
}
