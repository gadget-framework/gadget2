#include "doublevector.h"
#include "gadget.h"

#ifndef INLINE_VECTORS
#include "doublevector.icc"
#endif

doublevector::doublevector(int sz) {
  size = (sz > 0 ? sz : 0);
  if (size > 0)
    v = new double[size];
  else
    v = 0;
}

doublevector::doublevector(double* initial, int sz) {
  size = sz;
  v = new double[size];
  int i;
  for (i = 0; i<size; i++)
    v[i] = initial[i];
}

doublevector::doublevector(int sz, double value) {
  size = (sz > 0 ? sz : 0);
  int i;
  if (size > 0) {
    v = new double[size];
    for (i = 0; i < size; i++)
      v[i] = value;
  } else
    v = 0;
}

doublevector::doublevector(const doublevector& initial) {
  size = initial.size;
  int i;
  if (size > 0) {
    v = new double[size];
    for (i = 0; i < size; i++)
      v[i] = initial.v[i];
  } else
    v = 0;
}

void doublevector::resize(int addsize, double value) {
  int oldsize = size;
  this->resize(addsize);
  int i;
  if (addsize > 0)
    for (i = oldsize; i < size; i++)
      v[i] = value;
}

void doublevector::resize(int addsize) {
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

void doublevector::Delete(int pos) {
  assert(size > 0);
  assert(0 <= pos && pos < size);
  double* vnew = new double[size - 1];
  int i;
  for (i = 0; i < pos; i++)
    vnew[i] = v[i];
  for (i = pos; i < size - 1; i++)
    vnew[i] = v[i + 1];
  delete[] v;
  v = vnew;
  size--;
}

void doublevector::Reset() {
  if (size > 0) {
    delete[] v;
    v = 0;
    size = 0;
  }
}

void doublevector::setElementsTo(double d) {
  int i;
  for (i = 0; i < size; i++)
    v[i] = d;
}

doublevector& doublevector::operator *= (double d) {
  int i;
  for (i = 0; i<size; i++)
    v[i] *= d;
  return *this;
}

doublevector& doublevector::operator * (double d) const {
  doublevector* result = new doublevector(*this);
  return((*result) *= d);
}

doublevector& doublevector::operator += (double d) {
  int i;
  for (i = 0; i < size; i++)
    v[i] += d;
  return *this;
}

doublevector& doublevector::operator + (double d) const {
  doublevector* result = new doublevector(*this);
  return((*result) += d);
}

doublevector& doublevector::operator -= (double d) {
  int i;
  for (i = 0; i < size; i++)
    v[i] -= d;
  return *this;
}

doublevector& doublevector::operator - (double d) const {
  doublevector* result = new doublevector(*this);
  return((*result) -= d);
}

doublevector& doublevector::addVector(const doublevector& d, int sz) {
  int i;
  doublevector* result = new doublevector(sz, 0);
  for (i = 0; i < min(sz, size); i++)
    (*result)[i] = (*this)[i];
  for (i = 0; i < min(sz, d.Size()); i++)
    (*result)[i] += d[i];
  return *result;
}

doublevector& doublevector::subVector(const doublevector& d, int sz) {
  int i;
  doublevector* result = new doublevector(sz, 0);
  for (i = 0; i < min(sz, size); i++)
    (*result)[i] = (*this)[i];
  for (i = 0; i < min(sz, d.Size()); i++)
    (*result)[i] -= d[i];
  return *result;
}

doublevector& doublevector::operator += (const doublevector& d) {
  int i;
  for (i = 0; i < min(size, d.Size()); i++)
    (*this)[i] += d[i];
  return *this;
}

doublevector& doublevector::operator -= (const doublevector& d) {
  int i;
  for (i = 0; i < min(size, d.Size()); i++)
    (*this)[i] -= d[i];
  return *this;
}

double doublevector::operator * (const doublevector& d) const {
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

doublevector& doublevector::operator = (const doublevector& d) {
  if (this == &d)
    return(*this);
  int i;
  if (size == d.size) {
    for (i = 0; i < size; i++)
      v[i] = d[i];
    return(*this);
  }
  delete[] v;
  size = d.size;
  if (size > 0) {
    v = new double[size];
    for (i = 0; i < size; i++)
      v[i] = d.v[i];
  } else
    v = 0;
  return *this;
}

int doublevector::operator == (const doublevector& d) const {
  if (size != d.Size())
    return 0;
  int i;
  for (i = 0; i < size; i++)
    if (v[i] != d[i])
      return 0;
  return 1;
}

ostream& operator << (ostream& out, const doublevector& d) {
  int i;
  for (i = 0; i < d.Size(); i++)
    out << d[i] << sep;
  return out;
}
