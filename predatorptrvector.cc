#include "predatorptrvector.h"
#include "gadget.h"

#ifndef GADGET_INLINE
#include "predatorptrvector.icc"
#endif

PredatorPtrVector::PredatorPtrVector(int sz) {
  size = (sz > 0 ? sz : 0);
  if (size > 0)
    v = new Predator*[size];
  else
    v = 0;
}

PredatorPtrVector::PredatorPtrVector(int sz, Predator* value) {
  size = (sz > 0 ? sz : 0);
  int i;
  if (size > 0) {
    v = new Predator*[size];
    for (i = 0; i < size; i++)
      v[i] = value;
  } else
    v = 0;
}

PredatorPtrVector::PredatorPtrVector(const PredatorPtrVector& initial) {
  size = initial.size;
  int i;
  if (size > 0) {
    v = new Predator*[size];
    for (i = 0; i < size; i++)
      v[i] = initial.v[i];
  } else
    v = 0;
}

PredatorPtrVector::~PredatorPtrVector() {
  if (v != 0) {
    delete[] v;
    v = 0;
  }
}

void PredatorPtrVector::resize(int addsize, Predator* value) {
  int oldsize = size;
  this->resize(addsize);
  int i;
  if (addsize > 0)
    for (i = oldsize; i < size; i++)
      v[i] = value;
}

void PredatorPtrVector::resize(int addsize) {
  int i;
  if (v == 0) {
    size = addsize;
    v = new Predator*[size];
  } else if (addsize > 0) {
    Predator** vnew = new Predator*[addsize + size];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
    size += addsize;
  }
}

void PredatorPtrVector::Delete(int pos) {
  assert(size > 0);
  assert(0 <= pos && pos < size);
  Predator** vnew = new Predator*[size - 1];
  int i;
  for (i = 0; i < pos; i++)
    vnew[i] = v[i];
  for (i = pos; i < size - 1; i++)
    vnew[i] = v[i + 1];
  delete[] v;
  v = vnew;
  size--;
}
