#include "intvector.h"
#include "popinfomatrix.h"
#include "popinfoindexvector.h"
#include "agebandmatrix.h"
#include "mathfunc.h"

AgeBandMatrix::AgeBandMatrix(const AgeBandMatrix& initial)
  : minage(initial.minAge()), nrow(initial.Nrow()) {

  int i;
  if (nrow > 0) {
    v = new PopInfoIndexVector*[nrow];
    for (i = 0; i < nrow; i++)
      v[i] = new PopInfoIndexVector(initial[i + minage]);
  } else
    v = 0;
}

AgeBandMatrix::AgeBandMatrix(int age, const IntVector& minl,
  const IntVector& size) : minage(age), nrow(size.Size()) {

  int i;
  if (nrow > 0) {
    PopInfo nullpop;
    v = new PopInfoIndexVector*[nrow];
    for (i = 0; i < nrow; i++)
      v[i] = new PopInfoIndexVector(size[i], minl[i], nullpop);
  } else
    v = 0;
}

AgeBandMatrix::AgeBandMatrix(int age, const PopInfoMatrix& initial, int minl)
  : minage(age), nrow(initial.Nrow()) {

  if (nrow > 0) {
    int i, j, lower, upper;
    v = new PopInfoIndexVector*[nrow];
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
  } else
    v = 0;
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
