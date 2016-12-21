#include "doublevector.h"
#include "gadget.h"

DoubleVector::DoubleVector(int sz) {
  size = (sz > 0 ? sz : 0);
  if (size > 0)
    v = new double[size];
  else
    v = 0;
}

DoubleVector::DoubleVector(int sz, double value) {
  size = (sz > 0 ? sz : 0);
  int i;
  if (size > 0) {
    v = new double[size];
    for (i = 0; i < size; i++)
      v[i] = value;
  } else
    v = 0;
}

DoubleVector::DoubleVector(const DoubleVector& initial) {
  size = initial.size;
  int i;
  if (size > 0) {
    v = new double[size];
    for (i = 0; i < size; i++)
      v[i] = initial.v[i];
  } else
    v = 0;
}

DoubleVector::~DoubleVector() {
  if (v != 0) {
    delete[] v;
    v = 0;
  }
}

void DoubleVector::resize(int addsize, double value) {
  if (addsize <= 0)
    return;

  int i;
  if (v == 0) {
    size = addsize;
    v = new double[size];
    for (i = 0; i < size; i++)
      v[i] = value;

  } else if (addsize > 0) {
    double* vnew = new double[size + addsize];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    for (i = size; i < size + addsize; i++)
      vnew[i] = value;
    delete[] v;
    v = vnew;
    size += addsize;
  }
}

void DoubleVector::Delete(int pos) {
  int i;
  if (size > 1) {
    double* vnew = new double[size - 1];
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

void DoubleVector::Reset() {
  if (size > 0) {
    delete[] v;
    v = 0;
    size = 0;
  }
}

void DoubleVector::setToZero() {
  int i;
  for (i = 0; i < size; i++)
    v[i] = 0.0;
}

double DoubleVector::operator * (const DoubleVector& d) const {
  double result = 0.0;
  int i;
  if (size == d.size)
    for (i = 0; i < size; i++)
      result += (d[i] * v[i]);
  return result;
}

DoubleVector& DoubleVector::operator = (const DoubleVector& d) {
  int i;
  if (size == d.size) {
    for (i = 0; i < size; i++)
      v[i] = d[i];
    return *this;
  }
  if (v != 0) {
    delete[] v;
    v = 0;
  }
  size = d.size;
  if (size > 0) {
    v = new double[size];
    for (i = 0; i < size; i++)
      v[i] = d.v[i];
  } else
    v = 0;
  return *this;
}
