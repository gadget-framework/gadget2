#include "doublevector.h"
#include "gadget.h"

#ifndef GADGET_INLINE
#include "doublevector.icc"
#endif

DoubleVector::DoubleVector(int sz) {
  size = (sz > 0 ? sz : 0);
  if (size > 0)
    v = new double[size];
  else
    v = 0;
}

DoubleVector::DoubleVector(double* initial, int sz) {
  size = sz;
  int i;
  if (size > 0) {
    v = new double[size];
    for (i = 0; i < size; i++)
      v[i] = initial[i];
  } else
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
  int oldsize = size;
  this->resize(addsize);
  int i;
  if (addsize > 0)
    for (i = oldsize; i < size; i++)
      v[i] = value;
}

void DoubleVector::resize(int addsize) {
  int i;
  if (v == 0) {
    size = addsize;
    v = new double[size];
  } else if (addsize > 0) {
    double* vnew = new double[addsize + size];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
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

void DoubleVector::setElementsTo(double d) {
  int i;
  for (i = 0; i < size; i++)
    v[i] = d;
}

DoubleVector& DoubleVector::operator *= (double d) {
  int i;
  for (i = 0; i<size; i++)
    v[i] *= d;
  return *this;
}

DoubleVector& DoubleVector::operator * (double d) const {
  DoubleVector* result = new DoubleVector(*this);
  return ((*result) *= d);
}

DoubleVector& DoubleVector::operator += (double d) {
  int i;
  for (i = 0; i < size; i++)
    v[i] += d;
  return *this;
}

DoubleVector& DoubleVector::operator + (double d) const {
  DoubleVector* result = new DoubleVector(*this);
  return ((*result) += d);
}

DoubleVector& DoubleVector::operator -= (double d) {
  int i;
  for (i = 0; i < size; i++)
    v[i] -= d;
  return *this;
}

DoubleVector& DoubleVector::operator - (double d) const {
  DoubleVector* result = new DoubleVector(*this);
  return ((*result) -= d);
}

DoubleVector& DoubleVector::addVector(const DoubleVector& d, int sz) {
  int i;
  DoubleVector* result = new DoubleVector(sz, 0);
  for (i = 0; i < min(sz, size); i++)
    (*result)[i] = (*this)[i];
  for (i = 0; i < min(sz, d.Size()); i++)
    (*result)[i] += d[i];
  return *result;
}

DoubleVector& DoubleVector::subVector(const DoubleVector& d, int sz) {
  int i;
  DoubleVector* result = new DoubleVector(sz, 0);
  for (i = 0; i < min(sz, size); i++)
    (*result)[i] = (*this)[i];
  for (i = 0; i < min(sz, d.Size()); i++)
    (*result)[i] -= d[i];
  return *result;
}

DoubleVector& DoubleVector::operator += (const DoubleVector& d) {
  int i;
  for (i = 0; i < min(size, d.Size()); i++)
    (*this)[i] += d[i];
  return *this;
}

DoubleVector& DoubleVector::operator -= (const DoubleVector& d) {
  int i;
  for (i = 0; i < min(size, d.Size()); i++)
    (*this)[i] -= d[i];
  return *this;
}

double DoubleVector::operator * (const DoubleVector& d) const {
  if (size !=d.Size()) {
    cerr << "Error: vectors must be of equal length for multiplication!\n";
    exit(EXIT_FAILURE);
  }
  double result = 0;
  int i;
  for (i = 0; i < size; i++)
    result += (d[i] * v[i]);
  return result;
}

DoubleVector& DoubleVector::operator = (const DoubleVector& d) {
  if (this == &d)
    return *this;
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

int DoubleVector::operator == (const DoubleVector& d) const {
  if (size != d.Size())
    return 0;
  int i;
  for (i = 0; i < size; i++)
    if (v[i] != d[i])
      return 0;
  return 1;
}

ostream& operator << (ostream& out, const DoubleVector& d) {
  int i;
  for (i = 0; i < d.Size(); i++)
    out << d[i] << sep;
  return out;
}
