#include "parameter.h"
#include "gadget.h"

Parameter::Parameter() {
  name = NULL;
  size = 0;
}

Parameter::Parameter(char* value) {
  if (value == NULL) {
    name = NULL;
    size = 0;
  } else {
    if (isValidName(value)) {
      size = strlen(value);
      name = new char[size + 1];
      strcpy(name, value);
    } else {
      cerr << "Invalid parameter name - " << value << endl;
      exit(EXIT_FAILURE);
    }
  }
}

Parameter::Parameter(const Parameter& p) {
  //AJ 03.11.00 Maybe should start checking if same parameter???
  if (p.name == NULL)  {
    name = NULL;
    size = 0;
  } else {
    size = p.size;
    name = new char[size + 1];
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
    size = 0;
  } else {
    size = p.size;
    name = new char[size + 1];
    strcpy(name, p.name);
  }
  return *this;
}

int Parameter::isValidName(char* param) {
  int len = strlen(param);
  if (len > MaxStrLength)
    return 0;
  int i;
  for (i = 0; i < len; i++)
    if (this->legalchar(param[i]) == 0)
      return 0;
  return 1;
}

int Parameter::legalchar(int c) {
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
  int i = 0;
  char c;

  char* tempString = new char[MaxStrLength];
  in >> ws;
  while (p.legalchar(in.peek()) && i < (MaxStrLength - 1)) {
    if (in.fail() && !in.eof()) {
      in.makebad();
      delete[] tempString;
      return in;
    }
    in.get(c);
    tempString[i] = c;
    i++;
  }

  tempString[i] = '\0';
  if (strlen(tempString) == MaxStrLength - 1)
    cerr << "Warning - name of switch has reached maximum allowed length\n";

  if (p.name != NULL) {
    delete[] p.name;
    p.name = NULL;
  }

  if (i <= 0) {
    p.name = NULL;
    p.size = 0;
  } else {
    p.size = strlen(tempString);
    p.name = new char[p.size + 1];
    strcpy(p.name, tempString);
  }

  delete[] tempString;
  return in;
}

istream& operator >> (istream& in, Parameter& p) {
  int i = 0;
  char c;

  char* tempString = new char[MaxStrLength];
  in >> ws;
  while (p.legalchar(in.peek()) && i < (MaxStrLength - 1)) {
    if (in.fail() && !in.eof()) {
//      in.makebad();
      delete[] tempString;
      return in;
    }
    in.get(c);
    tempString[i] = c;
    i++;
  }

  tempString[i] = '\0';
  if (strlen(tempString) == MaxStrLength - 1)
    cerr << "Warning - name of switch has reached maximum allowed length\n";

  if (p.name != NULL) {
    delete[] p.name;
    p.name = NULL;
  }

  if (i <= 0) {
    p.name = NULL;
    p.size = 0;
  } else {
    p.size = strlen(tempString);
    p.name = new char[p.size + 1];
    strcpy(p.name, tempString);
  }

  delete[] tempString;
  return in;
}
