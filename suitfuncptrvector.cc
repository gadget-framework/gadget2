#include "suitfuncptrvector.h"
#include "gadget.h"

#ifndef GADGET_INLINE
#include "suitfuncptrvector.icc"
#endif

SuitFuncPtrVector::SuitFuncPtrVector(int sz) {
  size = (sz > 0 ? sz : 0);
  if (size > 0)
    v = new SuitFunc*[size];
  else
    v = 0;
}

SuitFuncPtrVector::SuitFuncPtrVector(int sz, SuitFunc* value) {
  size = (sz > 0 ? sz : 0);
  int i;
  if (size > 0) {
    v = new SuitFunc*[size];
    for (i = 0; i < size; i++)
      v[i] = value;
  } else
    v = 0;
}

SuitFuncPtrVector::SuitFuncPtrVector(const SuitFuncPtrVector& initial) {
  size = initial.size;
  int i;
  if (size > 0) {
    v = new SuitFunc*[size];
    for (i = 0; i < size; i++)
      v[i] = initial.v[i];
  } else
    v = 0;
}

SuitFuncPtrVector::~SuitFuncPtrVector() {
  if (v != 0) {
    delete[] v;
    v = 0;
  }
}

void SuitFuncPtrVector::resize(int addsize, SuitFunc* value) {
  int oldsize = size;
  this->resize(addsize);
  int i = 0;
  if (addsize > 0)
    for (i = oldsize; i < size; i++)
      v[i] = value;
}

void SuitFuncPtrVector::resize(int addsize) {
  if (v == 0) {
    size = addsize;
    v = new SuitFunc*[size];

  } else if (addsize > 0) {
    SuitFunc** vnew = new SuitFunc*[addsize + size];
    int i = 0;
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
    size += addsize;
  }
}

void SuitFuncPtrVector::Delete(int pos, Keeper* const keeper) {
  assert(size > 0);
  assert(0 <= pos && pos < size);
  SuitFunc** vnew = new SuitFunc*[size - 1];
  int i;
  for (i = 0; i < pos; i++)
    vnew[i] = v[i];
  for (i = pos; i < size - 1; i++)
    vnew[i] = v[i + 1];
  for (i = 0; i < v[pos]->noOfConstants(); i++)
    v[pos]->getConstants()[i].Delete(keeper);
  delete[] v;
  v = vnew;
  size--;
}

