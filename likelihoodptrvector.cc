#include "likelihoodptrvector.h"
#include "gadget.h"

#ifndef GADGET_INLINE
#include "likelihoodptrvector.icc"
#endif

LikelihoodPtrVector::LikelihoodPtrVector(int sz) {
  size = (sz > 0 ? sz : 0);
  if (size > 0)
    v = new Likelihood*[size];
  else
    v = 0;
}

LikelihoodPtrVector::LikelihoodPtrVector(int sz, Likelihood* value) {
  size = (sz > 0 ? sz : 0);
  int i;
  if (size > 0) {
    v = new Likelihood*[size];
    for (i = 0; i < size; i++)
      v[i] = value;
  } else
    v = 0;
}

LikelihoodPtrVector::LikelihoodPtrVector(const LikelihoodPtrVector& initial) {
  size = initial.size;
  int i;
  if (size > 0) {
    v = new Likelihood*[size];
    for (i = 0; i < size; i++)
      v[i] = initial.v[i];
  } else
    v = 0;
}

LikelihoodPtrVector::~LikelihoodPtrVector() {
  if (v != 0) {
    delete[] v;
    v = 0;
  }
}

void LikelihoodPtrVector::resize(int addsize, Likelihood* value) {
  int oldsize = size;
  this->resize(addsize);
  int i;
  if (addsize > 0)
    for (i = oldsize; i < size; i++)
      v[i] = value;
}

void LikelihoodPtrVector::resize(int addsize) {
  int i;
  if (v == 0) {
    size = addsize;
    v = new Likelihood*[size];
  } else if (addsize > 0) {
    Likelihood** vnew = new Likelihood*[addsize + size];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
    size += addsize;
  }
}

void LikelihoodPtrVector::Delete(int pos) {
  int i;
  if (size > 1) {
    Likelihood** vnew = new Likelihood*[size - 1];
    for (i = 0; i < pos; i++)
      vnew[i] = v[i];
    for (i = pos; i < size - 1; i++)
      vnew[i] = v[i + 1];
    delete[] v;
    v = vnew;
    size--;
  } else {
    delete[] v;
    v = 0;
    size = 0;
  }
}
