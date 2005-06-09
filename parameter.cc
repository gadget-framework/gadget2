#include "parameter.h"
#include "gadget.h"

Parameter::Parameter(char* value) {
  if (value == NULL) {
    name = NULL;
  } else {
    if (this->isValidName(value)) {
      name = new char[strlen(value) + 1];
      strcpy(name, value);
    } else {
      cerr << "Invalid parameter name - " << value << endl;
      exit(EXIT_FAILURE);
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
  //AJ 03.10.00 Adding checks if need to allocate/delete memory.
  int len;
  if (this == &p)
    return *this;

  //return memory if needed
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
    if (this->isValidChar(value[i]) == 0)
      return 0;
  return 1;
}

int Parameter::isValidChar(int c) {
  //Numbers and letters are ok.
  if (isalnum(c))
    return 1;

  switch (c) {
    case '_':
    case '.':
      //Valid characters in addition to numbers and letters
      return 1;
      break;
    default:
      //All other character are invalid in parameter names
      return 0;
  }
}

CommentStream& operator >> (CommentStream& in, Parameter& p) {
  in >> ws;
  int i = 0;
  char* tempString = new char[MaxStrLength];
  while (p.isValidChar(in.peek()) && i < (MaxStrLength - 1)) {
    if (in.fail() && !in.eof()) {
      in.makebad();
      delete[] tempString;
      return in;
    }
    in.get(tempString[i]);
    i++;
  }

  tempString[i] = '\0';
  if (i == MaxStrLength)
    cerr << "Warning - name of parameter has reached maximum allowed length\n";

  if (p.name != NULL) {
    delete[] p.name;
    p.name = NULL;
  }

  if (i <= 0) {
    p.name = NULL;
  } else {
    p.name = new char[strlen(tempString) + 1];
    strcpy(p.name, tempString);
  }

  delete[] tempString;
  return in;
}

istream& operator >> (istream& in, Parameter& p) {
  in >> ws;
  int i = 0;
  char* tempString = new char[MaxStrLength];
  while (p.isValidChar(in.peek()) && i < (MaxStrLength - 1)) {
    if (in.fail() && !in.eof()) {
      //in.makebad();
      delete[] tempString;
      return in;
    }
    in.get(tempString[i]);
    i++;
  }

  tempString[i] = '\0';
  if (i == MaxStrLength)
    cerr << "Warning - name of parameter has reached maximum allowed length\n";

  if (p.name != NULL) {
    delete[] p.name;
    p.name = NULL;
  }

  if (i <= 0) {
    p.name = NULL;
  } else {
    p.name = new char[strlen(tempString) + 1];
    strcpy(p.name, tempString);
  }

  delete[] tempString;
  return in;
}
