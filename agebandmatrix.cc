#include "intvector.h"
#include "popinfomatrix.h"
#include "popinfoindexvector.h"
#include "agebandmatrix.h"
#include "mathfunc.h"

#ifndef GADGET_INLINE
#include "agebandmatrix.icc"
#endif

AgeBandMatrix::AgeBandMatrix(const AgeBandMatrix& initial)
  : minage(initial.minAge()), nrow(initial.Nrow()) {

  v = new PopInfoIndexVector*[nrow];
  int i;
  for (i = 0; i < nrow; i++)
    v[i] = new PopInfoIndexVector(initial[i + minage]);
}

AgeBandMatrix::AgeBandMatrix(int MinAge, const IntVector& minl,
  const IntVector& size) : minage(MinAge), nrow(size.Size()) {

  PopInfo nullpop;
  assert(minl.Size() == size.Size());
  v = new PopInfoIndexVector*[nrow];
  int i;
  for (i = 0; i < nrow; i++)
    v[i] = new PopInfoIndexVector(size[i], minl[i], nullpop);
}

AgeBandMatrix::AgeBandMatrix(int MinAge, const PopInfoMatrix& initial)
  : minage(MinAge), nrow(initial.Nrow()) {

  v = new PopInfoIndexVector*[nrow];
  int i, j;
  int lower, upper;
  for (i = 0; i < nrow; i++) {
    lower = 0;
    upper = initial.Ncol(i) - 1;
    while (isZero(initial[i][lower].N) && lower < upper)
      lower++;
    while (isZero(initial[i][upper].N) && upper > lower)
      upper--;
    v[i] = new PopInfoIndexVector(upper - lower + 1, lower);
    for (j = lower; j <= upper; j++)
      (*v[i])[j] = initial[i][j];
  }
}

AgeBandMatrix::AgeBandMatrix(int MinAge, int minl, const PopInfoMatrix& initial)
  : minage(MinAge), nrow(initial.Nrow()) {

  v = new PopInfoIndexVector*[nrow];
  int i, j;
  int lower, upper;
  for (i = 0; i < nrow; i++) {
    lower = 0;
    upper = initial.Ncol(i) - 1;
    while (isZero(initial[i][lower].N) && (lower < upper))
      lower++;
    while (isZero(initial[i][upper].N) && (upper > lower))
      upper--;
    v[i] = new PopInfoIndexVector(upper - lower + 1, lower + minl);
    for (j = lower; j <= upper; j++)
      (*v[i])[j + minl] = initial[i][j];
  }
}

AgeBandMatrix::AgeBandMatrix(int MinAge, const PopInfoIndexVector& initial)
  : minage(MinAge), nrow(1) {

  v = new PopInfoIndexVector*[1];
  v[0] = new PopInfoIndexVector(initial);
}

AgeBandMatrix::~AgeBandMatrix() {
  int i;
  if (v != 0) {
    for (i = 0; i < nrow; i++)
      delete v[i];
    delete[] v;
    v = 0;
  }
}
