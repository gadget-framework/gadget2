#include "doubleindexvector.h"
#include "mathfunc.h"
#include "gadget.h"

doubleindexvector::doubleindexvector(int sz, int minp) {
  size = sz;
  minpos = minp;
  if (size > 0)
    v = new double[size];
  else
    v = 0;
}

doubleindexvector::doubleindexvector(int sz, int minp, double value) {
  size = sz;
  minpos = minp;
  int i;
  if (size > 0) {
    v = new double[size];
    for (i = 0; i < size; i++)
      v[i] = value;
  } else
    v = 0;
}

doubleindexvector::doubleindexvector(const doubleindexvector& initial) {
  size = initial.size;
  minpos = initial.minpos;
  v = new double[size];
  int i;
  for (i = 0; i < size; i++)
    v[i] = initial.v[i];
}

void doubleindexvector::resize(int addsize, int lower, double initial) {
  double* vnew = new double[size + addsize];
  int i;
  if (v != 0) {
    assert(lower <= minpos);
    for (i = 0; i < size; i++)
      vnew[i + minpos - lower] = v[i];
    for (i = size; i < size + addsize; i++)
      vnew[i] = initial;
    for (i = 0; i < minpos - lower; i++)
      vnew[i] = initial;
    size += addsize;
    delete[] v;
  } else {
    size = addsize;
    for (i = 0; i < size; i++)
      vnew[i] = initial;
  }
  v = vnew;
  minpos = lower;
}

double& doubleindexvector::operator [] (int pos) {
  assert(minpos <= pos && pos < minpos + size);
  return(v[pos - minpos]);
}

const double& doubleindexvector::operator [] (int pos) const {
  assert(minpos <= pos && pos < minpos + size);
  return(v[pos - minpos]);
}
