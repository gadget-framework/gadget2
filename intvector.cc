#include "intvector.h"
#include "gadget.h"

#ifndef GADGET_INLINE
#include "intvector.icc"
#endif

IntVector::IntVector(int sz) {
  size = (sz > 0 ? sz : 0);
  if (size > 0)
    v = new int[size];
  else
    v = 0;
}

IntVector::IntVector(int sz, int value) {
  size = (sz > 0 ? sz : 0);
  int i;
  if (size > 0) {
    v = new int[size];
    for (i = 0; i < size; i++)
      v[i] = value;
  } else
    v = 0;
}

IntVector::IntVector(const IntVector& initial) {
  size = initial.size;
  int i;
  if (size > 0) {
    v = new int[size];
    for (i = 0; i < size; i++)
      v[i] = initial.v[i];
  } else
    v = 0;
}

IntVector::~IntVector() {
  if (v != 0) {
    delete[] v;
    v = 0;
  }
}

void IntVector::resize(int addsize, int value) {
  int oldsize = size;
  this->resize(addsize);
  int i;
  if (addsize > 0)
    for (i = oldsize; i < size; i++)
      v[i] = value;
}

void IntVector::resize(int addsize) {
  int i;
  if (v == 0) {
    size = addsize;
    v = new int[size];
  } else if (addsize > 0) {
    int* vnew = new int[addsize + size];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
    size += addsize;
  }
}

void IntVector::Delete(int pos) {
  assert(size > 0);
  assert(0 <= pos && pos < size);
  int* vnew = new int[size - 1];
  int i;
  for (i = 0; i < pos; i++)
    vnew[i] = v[i];
  for (i = pos; i < size - 1; i++)
    vnew[i] = v[i + 1];
  delete[] v;
  v = vnew;
  size--;
}

IntVector& IntVector::operator = (const IntVector& d) {
  if (this == &d)
    return(*this);
  int i;
  if (size == d.size) {
    for (i = 0; i < size; i++)
      v[i] = d[i];
    return(*this);
  }
  if (v != 0) {
    delete [] v;
    v = 0;
  }
  size = d.size;
  if (size > 0) {
    v = new int[size];
    for (i = 0; i < size; i++)
      v[i] = d.v[i];
  } else
    v = 0;
  return *this;
}
