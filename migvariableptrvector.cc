#include "migvariableptrvector.h"
#include "gadget.h"

#ifndef GADGET_INLINE
#include "migvariableptrvector.icc"
#endif

MigVariablePtrVector::MigVariablePtrVector(int sz) {
  size = (sz > 0 ? sz : 0);
  if (size > 0)
    v = new MigVariable*[size];
  else
    v = 0;
}

MigVariablePtrVector::~MigVariablePtrVector() {
  if (v != 0) {
    delete[] v;
    v = 0;
  }
}

void MigVariablePtrVector::resize(int addsize) {
  int i;
  if (v == 0) {
    size = addsize;
    v = new MigVariable*[size];
  } else if (addsize > 0) {
    MigVariable** vnew = new MigVariable*[addsize + size];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
    size += addsize;
  }
}
