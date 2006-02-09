#include "intvector.h"
#include "popinfomatrix.h"
#include "popinfoindexvector.h"
#include "agebandmatrix.h"
#include "mathfunc.h"

AgeBandMatrix::AgeBandMatrix(const AgeBandMatrix& initial)
  : minage(initial.minAge()), nrow(initial.Nrow()) {

  v = new PopInfoIndexVector*[nrow];
  int i;
  for (i = 0; i < nrow; i++)
    v[i] = new PopInfoIndexVector(initial[i + minage]);
}

AgeBandMatrix::AgeBandMatrix(int age, const IntVector& minl,
  const IntVector& size) : minage(age), nrow(size.Size()) {

  PopInfo nullpop;
  v = new PopInfoIndexVector*[nrow];
  int i;
  for (i = 0; i < nrow; i++)
    v[i] = new PopInfoIndexVector(size[i], minl[i], nullpop);
}

AgeBandMatrix::AgeBandMatrix(int age, const PopInfoMatrix& initial, int minl)
  : minage(age), nrow(initial.Nrow()) {

  v = new PopInfoIndexVector*[nrow];
  int i, j, lower, upper;
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

AgeBandMatrix::AgeBandMatrix(int age, const PopInfoIndexVector& initial)
  : minage(age), nrow(1) {

  v = new PopInfoIndexVector*[nrow];
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
