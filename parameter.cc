#include "parameter.h"
#include "gadget.h"

Parameter::Parameter(char* value) {
  if (isLegalParameter(value)) {
    name = new char[strlen(value) + 1];
    strcpy(name, value);
  } else {
    cerr << "Illegal parameter name - " << value << endl;
    exit(EXIT_FAILURE);
  }
}

int Parameter::isLegalParameter(char* param) {
  int strlength = strlen(param);
  int counter = 0;
  int LEGAL = 1;
  if (strlength > MaxStrLength)
    LEGAL = 0;
  else {
    while ((counter < strlength) && (LEGAL == 1)) {
      LEGAL = this->legalchar(param[counter]);
      counter++;
    }
  }
  return LEGAL;
}

char* Parameter::getValue() {
  return name;
}

Parameter::Parameter() {
  name = NULL;
}

Parameter::Parameter(const Parameter& p) {
  //AJ 03.11.00 Maybe should start checking if same parameter???
  int len;
  if (p.name == NULL)  {
    name = NULL;
  } else {
    assert(strlen(p.name) < MaxStrLength);
    len = (strlen(p.name) + 1);
    name = new char[len];
    strncpy(name, p.name, len);
  }
}

Parameter::~Parameter() {
  if (name != NULL) {
    delete[] name;
    name = NULL;
  }
}

int Parameter::operator == (const Parameter& p) const {
  #ifdef PARAMETERS_CASE_SENSITIVE
    return (strcmp(name, p.name) == 0);
  #else
    return (strcasecmp(name, p.name) == 0);
  #endif
}

Parameter& Parameter::operator = (const Parameter& p) {
  //AJ 03.10.00 Adding checks if need to allocate/delete memory.
  //What happens with a = a f.ex.x
  int len;
  if (this == &p) {
    return *this;
  }
  //return memory if needed
  if (name != NULL) {
    delete[] name;
    name = NULL;
  }
  if (p.name == NULL) {
    name = NULL;
  } else {
    assert(strlen(p.name) < MaxStrLength);
    len = (strlen(p.name) + 1);
    name = new char[len];
    strncpy(name, p.name, len);
  }
  //AJ 03.10.00 Adding return value
  return *this;
}

int Parameter::legalchar(int c) {
  //Numbers and letters are ok.
  if (isalnum(c))
    return 1;

  switch(c) {
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

int Parameter::Size() const {
  if (name == NULL)
    return 0;
  else
    return strlen(name);
}

CommentStream& operator >> (CommentStream& in, Parameter& p) {
  int i = 0;
  int len;
  char c;

  char* tempString;
  if (in.fail()) {
    in.makebad();
    return in;
  }
  //No checks for not being able to read from in...
  in >> ws;
  if (in.fail() && !in.eof()) {
    in.makebad();
    return in;
  }
  tempString = new char[MaxStrLength];
  while (p.legalchar(in.peek()) && i < MaxStrLength - 1) {
    if (in.fail() && !in.eof()) {
      in.makebad();
      delete[] tempString;
      return in;
    }
    in.get(c);
    if (in.fail() && !in.eof()) {
      in.makebad();
      delete[] tempString;
      return in;
    }
    tempString[i] = c;
    i++;
  }

  tempString[i] = 0;
  if (strlen(tempString) == MaxStrLength - 1)
    cerr << "Warning: name of switch has reached maximum allowed length\n";

  //return memory possibly used by p.
  if (p.name != NULL) {
    delete[] p.name;
    p.name = NULL;
  }
  if (i <= 0) {
    //did not read any string
    p.name = NULL;
  } else {
    len = (strlen(tempString) + 1);
    p.name = new char[len];
    strncpy(p.name, tempString, len);
  }
  delete[] tempString;
  return in;
}

istream& operator >> (istream& in, Parameter& p) {
  int i = 0;
  int len;
  char c;

  char* tempString = new char[MaxStrLength];
  in >> ws;
  while (p.legalchar(in.peek()) && i < (MaxStrLength - 1)) {
    in.get(c);
    tempString[i] = c;
    i++;
  }

  tempString[i] = 0;
  if (p.name != NULL) {
    delete[] p.name;
    p.name = NULL;
  }
  if (i <= 0) {
    //did not read any string
    p.name = NULL;
  } else {
    len = (strlen(tempString) + 1);
    p.name = new char[len];
    strncpy(p.name, tempString, len);
  }
  delete[] tempString;
  return in;
}
