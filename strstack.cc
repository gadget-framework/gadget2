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
    v.resize(new char[MaxStrLength]);
    strncpy(v[size], "", MaxStrLength);
  }
  strcpy(v[size], str);
  size++;
}

char* StrStack::sendAll() const {
  int i, len;
  len = size * MaxStrLength;
  char* rstr = new char[len];
  strncpy(rstr, "", len);
  for (i = 0; i < size; i++)
    strcat(rstr, v[i]);

  return rstr;
}

char* StrStack::sendTop() const {
  char* rstr = new char[MaxStrLength];
  strncpy(rstr, "", MaxStrLength);
  if (size != 0)
    strcpy(rstr, v[size - 1]);

  return rstr;
}
