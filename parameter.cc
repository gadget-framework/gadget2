#include "parameter.h"
#include "gadget.h"

#ifndef GADGET_INLINE
#include "parameter.icc"
#endif

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

Parametervector::Parametervector(int sz) {
  size = (sz > 0 ? sz : 0);

  if (size > 0)
    v = new Parameter[size];
  else
    v = 0;
}

Parametervector::Parametervector(int sz, Parameter& value) {
  size = (sz > 0 ? sz : 0);
  int i;
  if (size > 0) {
    v = new Parameter[size];
    for (i = 0; i < size; i++)
      v[i] = value;
  } else
    v = 0;
}

Parametervector::Parametervector(const Parametervector& initial) {
  size = initial.size;
  int i;
  if (size > 0) {
    v = new Parameter[size];
    for (i = 0; i < size; i++)
      v[i] = initial.v[i];
  } else
    v = 0;
}

//The function resize add addsize elements to a Parametervector and fills it vith value.
void Parametervector::resize(int addsize, Parameter& value) {
  int oldsize = size;
  this->resize(addsize);
  int i;
  if (addsize > 0)
    for (i = oldsize; i < size; i++)
      v[i] = value;
}

void Parametervector::resize(int addsize) {
  int i;
  if (v == 0) {
    size = addsize;
    v = new Parameter[size];
  } else if (addsize > 0) {
    Parameter* vnew = new Parameter[addsize + size];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
    size += addsize;
  }
}

void Parametervector::Delete(int pos) {
  assert(size > 0);
  assert(0 <= pos && pos < size);
  Parameter* vnew = new Parameter[size - 1];
  int i;
  for (i = 0; i < pos; i++)
    vnew[i] = v[i];
  for (i = pos; i < size - 1; i++)
    vnew[i] = v[i + 1];
  delete[] v;
  v = vnew;
  size--;
}

int Parametervector::findIndex(Parameter& p) {
  int i;
  for (i = 0; i < size; i++)
    if (v[i] == p)
      return i;
  return -1;
}

CommentStream& operator >> (CommentStream& infile, Parametervector& paramVec) {
  if (infile.fail()) {
    infile.makebad();
    return infile;
  }
  int i;
  for (i = 0; i < paramVec.size; i++) {
    if (!(infile >> paramVec[i])){
      infile.makebad();
      return infile;
    }
  }
  return infile;
}

int Parametervector::ReadVectorInLine(CommentStream& infile) {
  if (infile.fail())
    return 0;
  char line[MaxStrLength];
  strncpy(line, "", MaxStrLength);
  infile.getline(line, MaxStrLength);
  if (infile.fail())
    return 0;
  istringstream istr(line);
  istr >> ws;
  int i = 0;
  while (!istr.eof()) {
    if (i == Size())
      resize(1);
    istr >> v[i];
    if (istr.fail())
      //could return the number of the last read element...
      //but compatibility with older code has to be assured
      return 0;
    istr >> ws;
    i++;
  }
  return 1;
}

Parametervector& Parametervector::operator = (const Parametervector& paramv) {
  if (this == &paramv)
    return *this;
  int i;
  if (size == paramv.size) {
    for (i = 0; i < size; i++)
      v[i] = paramv[i];
    return *this;
  }
  delete[] v;
  size = paramv.size;
  if (size > 0) {
    v = new Parameter[size];
    for (i = 0; i < size; i++)
      v[i] = paramv.v[i];
  } else
    v = 0;
  return *this;
}
