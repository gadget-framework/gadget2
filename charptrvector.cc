#include "charptrvector.h"
#include "gadget.h"

#ifndef GADGET_INLINE
#include "charptrvector.icc"
#endif

charptrvector::charptrvector(int sz) {
  size = (sz > 0 ? sz : 0);
  if (size > 0)
    v = new char*[size];
  else
    v = 0;
}

charptrvector::charptrvector(int sz, char* value) {
  size = (sz > 0 ? sz : 0);
  int i;
  if (size > 0) {
    v = new char*[size];
    for (i = 0; i < size; i++)
      v[i] = value;
  } else
    v = 0;
}

charptrvector::charptrvector(const charptrvector& initial) {
  size = initial.size;
  int i;
  if (size > 0) {
    v = new char*[size];
    for (i = 0; i < size; i++)
      v[i] = initial.v[i];
  } else
    v = 0;
}

charptrvector::~charptrvector() {
  if (v != 0) {
    delete[] v;
    v = 0;
  }
}

//The function resize add addsize elements to a charptrvector and fills it vith value.
void charptrvector::resize(int addsize, char* value) {
  int oldsize = size;
  this->resize(addsize);
  int i;
  if (addsize > 0)
    for (i = oldsize; i < size; i++)
      v[i] = value;
}

void charptrvector::resize(int addsize) {
  int i;
  if (v == 0) {
    size = addsize;
    v = new char*[size];
  } else if (addsize > 0) {
    char** vnew = new char*[addsize + size];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
    size += addsize;
  }
}

void charptrvector::Delete(int pos) {
  assert(size > 0);
  assert(0 <= pos && pos < size);
  char** vnew = new char*[size - 1];
  int i;
  for (i = 0; i < pos; i++)
    vnew[i] = v[i];
  for (i = pos; i < size - 1; i++)
    vnew[i] = v[i + 1];
  delete[] v;
  v = vnew;
  size--;
}
