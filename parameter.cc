#include "parameter.h"
#include "errorhandler.h"
#include "gadget.h"

extern ErrorHandler handle;

Parameter::Parameter(char* value) {
  if (value == NULL) {
    name = NULL;
  } else {
    if (this->isValidName(value)) {
      name = new char[strlen(value) + 1];
      strcpy(name, value);
    } else {
      handle.logMessage(LOGFAIL, "Error in parameter - invalid parameter name", value);
    }
  }
}

Parameter::Parameter(const Parameter& p) {
  if (p.name == NULL)  {
    name = NULL;
  } else {
    name = new char[strlen(p.name) + 1];
    strcpy(name, p.name);
  }
}

Parameter::~Parameter() {
  if (name != NULL) {
    delete[] name;
    name = NULL;
  }
}

int Parameter::operator == (const Parameter& p) const {
  return (strcasecmp(name, p.name) == 0);
}

Parameter& Parameter::operator = (const Parameter& p) {
  if (this == &p)
    return *this;

  if (name != NULL) {
    delete[] name;
    name = NULL;
  }
  if (p.name == NULL) {
    name = NULL;
  } else {
    name = new char[strlen(p.name) + 1];
    strcpy(name, p.name);
  }
  return *this;
}

int Parameter::isValidName(char* value) {
  int len = strlen(value);
  if (len > MaxStrLength)
    return 0;
  int i;
  for (i = 0; i < len; i++)
    if (!this->isValidChar(value[i]))
      return 0;
  return 1;
}

int Parameter::isValidChar(int c) {
  //numbers and letters are ok
  if (isalnum(c))
    return 1;

  switch (c) {
    case '_':
    case '.':
      //other valid characters in addition to numbers and letters
      return 1;
      break;
    case '\\':
      //reading backslash will do nasty things to the input stream
      handle.logFileMessage(LOGFAIL, "error in parameter - name cannot include backslash");
      return 0;
      break;
    default:
      //all other character are invalid in parameter names
      return 0;
  }
  return 0;
}

CommentStream& operator >> (CommentStream& infile, Parameter& p) {
  char* tempString = new char[MaxStrLength];
  strncpy(tempString, "", MaxStrLength);
  int i = 0;
  infile >> ws;
  while (p.isValidChar(infile.peek()) && i < (MaxStrLength - 1)) {
    if (infile.fail() && !infile.eof()) {
      delete[] tempString;
      return infile;
    }
    infile.get(tempString[i]);
    i++;
  }

  tempString[i] = '\0';
  if (i == MaxStrLength)
    handle.logMessage(LOGWARN, "Warning in parameter - parameter name has reached maximum length");

  if (p.name != NULL) {
    delete[] p.name;
    p.name = NULL;
  }

  if (i != 0) {
    p.name = new char[strlen(tempString) + 1];
    strcpy(p.name, tempString);
  }

  delete[] tempString;
  return infile;
}

istream& operator >> (istream& infile, Parameter& p) {
  char* tempString = new char[MaxStrLength];
  strncpy(tempString, "", MaxStrLength);
  int i = 0;
  infile >> ws;
  while (p.isValidChar(infile.peek()) && i < (MaxStrLength - 1)) {
    if (infile.fail() && !infile.eof()) {
      delete[] tempString;
      return infile;
    }
    infile.get(tempString[i]);
    i++;
  }

  tempString[i] = '\0';
  if (i == MaxStrLength)
    handle.logMessage(LOGWARN, "Warning in parameter - parameter name has reached maximum length");

  if (p.name != NULL) {
    delete[] p.name;
    p.name = NULL;
  }

  if (i != 0) {
    p.name = new char[strlen(tempString) + 1];
    strcpy(p.name, tempString);
  }

  delete[] tempString;
  return infile;
}
