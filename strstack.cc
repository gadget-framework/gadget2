#include "strstack.h"
#include "gadget.h"

StrStack::~StrStack() {
  int i;
  for (i = 0; i < v.Size(); i++)
    delete[] v[i];
}

void StrStack::OutOfStack() {
  if (size > 0)
    size--;
}

void StrStack::PutInStack(const char* str) {
  if (size == v.Size()) {
    v.resize(1);
    v[size] = new char[MaxStrLength];
    strncpy(v[size], "", MaxStrLength);
  }
  /*JMB code removed from here - see RemovedCode.txt for details*/
  strcpy(v[size], str);
  size++;
}

void StrStack::ClearStack() {
  size = 0;
}

char* StrStack::SendAll() const {
  int len = size * MaxStrLength;
  char* rstr = new char[len];
  strncpy(rstr, "", len);
  int i;
  for (i = 0; i < size; i++) {
    if (len - (strlen(rstr) + 1) < strlen(v[i]) + 1) {
      char* str = new char[sizeof(rstr) + strlen(v[i]) + 1];
      strcpy(str, rstr);
      delete[] rstr;
      rstr = str;
    }
    strcat(rstr, v[i]);
  }
  return rstr;
}

char* StrStack::SendTop() const {
  char* rstr = new char[strlen(v[size - 1]) + 1];
  strcpy(rstr, v[size - 1]);
  return rstr;
}
