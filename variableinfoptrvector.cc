#include "variableinfoptrvector.h"
#include "gadget.h"

#ifndef GADGET_INLINE
#include "variableinfoptrvector.icc"
#endif

VariableInfoPtrVector::VariableInfoPtrVector(int sz) {
  size = (sz > 0 ? sz : 0);
  if (size > 0)
    v = new VariableInfo*[size];
  else
    v = 0;
}

VariableInfoPtrVector::VariableInfoPtrVector(int sz, VariableInfo* value) {
  size = (sz > 0 ? sz : 0);
  int i;
  if (size > 0) {
    v = new VariableInfo*[size];
    for (i = 0; i < size; i++)
      v[i] = value;
  } else
    v = 0;
}

VariableInfoPtrVector::VariableInfoPtrVector(const VariableInfoPtrVector& initial) {
  size = initial.size;
  int i;
  if (size > 0) {
    v = new VariableInfo*[size];
    for (i = 0; i < size; i++)
      v[i] = initial.v[i];
  } else
    v = 0;
}

VariableInfoPtrVector::~VariableInfoPtrVector() {
  if (v != 0) {
    delete[] v;
    v = 0;
  }
}

void VariableInfoPtrVector::resize(int addsize, VariableInfo* value) {
  int oldsize = size;
  this->resize(addsize);
  int i;
  if (addsize > 0)
    for (i = oldsize; i < size; i++)
      v[i] = value;
}

void VariableInfoPtrVector::resize(int addsize) {
  int i;
  if (v == 0) {
    size = addsize;
    v = new VariableInfo*[size];
  } else if (addsize > 0) {
    VariableInfo** vnew = new VariableInfo*[addsize + size];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
    size += addsize;
  }
}

void VariableInfoPtrVector::Delete(int pos) {
  assert(size > 0);
  assert(0 <= pos && pos < size);
  VariableInfo** vnew = new VariableInfo*[size - 1];
  int i;
  for (i = 0; i < pos; i++)
    vnew[i] = v[i];
  for (i = pos; i < size - 1; i++)
    vnew[i] = v[i + 1];
  delete[] v;
  v = vnew;
  size--;
}
