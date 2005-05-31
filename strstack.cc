#include "strstack.h"
#include "gadget.h"

StrStack::~StrStack() {
  int i;
  for (i = 0; i < v.Size(); i++)
    delete[] v[i];
}

void StrStack::clearString() {
  if (size > 0)
    size--;
}

void StrStack::storeString(const char* str) {
  if (size == v.Size()) {
    v.resize(1);
    v[size] = new char[MaxStrLength];
    strncpy(v[size], "", MaxStrLength);
  }
  strcpy(v[size], str);
  size++;
}

void StrStack::clearStack() {
  size = 0;
}

char* StrStack::sendAll() const {
  int i, len;

  if (size != 0)
    len = size * MaxStrLength;
  else
    len = MaxStrLength;

  char* rstr = new char[len];
  strncpy(rstr, "", len);
  for (i = 0; i < size; i++) {
    if (len - (strlen(rstr) + 1) < strlen(v[i]) + 1) {
      //this should never happen ...
      char* str = new char[sizeof(rstr) + strlen(v[i]) + 1];
      strcpy(str, rstr);
      delete[] rstr;
      rstr = str;
    }
    strcat(rstr, v[i]);
  }
  return rstr;
}

char* StrStack::sendTop() const {
  char* rstr = new char[MaxStrLength];
  strncpy(rstr, "", MaxStrLength);
  if (size != 0)
    strcpy(rstr, v[size - 1]);

  return rstr;
}
