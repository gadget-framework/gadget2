#include "strstack.h"
#include "gadget.h"

StrStack::~StrStack() {
  int i;
  for (i = 0; i < v.Size(); i++)
    delete[] v[i];
}

void StrStack::OutOfStack() {
  if (sz > 0)
    sz -= 1;
}

void StrStack::PutInStack(const char* str) {
  if (sz == v.Size()) {
    v.resize(1);
    v[sz] = new char[MaxStrLength];
    strncpy(v[sz], "", MaxStrLength);
  }
  /*JMB code removed from here - see RemovedCode.txt for details*/
  strcpy(v[sz], str);
  sz++;
}

void StrStack::ClearStack() {
  sz = 0;
}

char* StrStack::SendAll() const {
  int len = sz * MaxStrLength;
  char* rstr = new char[len];
  strncpy(rstr, "", len);
  int i;
  for (i = 0; i < sz; i++) {
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
  char* rstr = new char[strlen(v[sz - 1]) + 1];
  strcpy(rstr, v[sz - 1]);
  return rstr;
}
