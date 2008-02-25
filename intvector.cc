#include "intvector.h"
#include "gadget.h"

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
  if (addsize <= 0)
    return;

  int i;
  if (v == 0) {
    size = addsize;
    v = new int[size];
    for (i = 0; i < size; i++)
      v[i] = value;

  } else {
    int* vnew = new int[size + addsize];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    for (i = size; i < size + addsize; i++)
      vnew[i] = value;
    delete[] v;
    v = vnew;
    size += addsize;
  }
}

void IntVector::Delete(int pos) {
  int i;
  if (size > 1) {
    int* vnew = new int[size - 1];
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

IntVector& IntVector::operator = (const IntVector& iv) {
  int i;
  if (size == iv.size) {
    for (i = 0; i < size; i++)
      v[i] = iv[i];
    return *this;
  }
  if (v != 0) {
    delete[] v;
    v = 0;
  }
  size = iv.size;
  if (size > 0) {
    v = new int[size];
    for (i = 0; i < size; i++)
      v[i] = iv.v[i];
  } else
    v = 0;
  return *this;
}

void IntVector::Reset() {
  if (size > 0) {
    delete[] v;
    v = 0;
    size = 0;
  }
}

void IntVector::setToZero() {
  int i;
  for (i = 0; i < size; i++)
    v[i] = 0;
}
