#include "intvector.h"
#include "popinfomatrix.h"
#include "popinfoindexvector.h"
#include "agebandmatrix.h"
#include "mathfunc.h"

#ifndef GADGET_INLINE
#include "agebandmatrix.icc"
#endif

Agebandmatrix::Agebandmatrix(const Agebandmatrix& initial)
  : minage(initial.Minage()), nrow(initial.Nrow()) {

  v = new popinfoindexvector*[nrow];
  int i;
  for (i = 0; i < nrow; i++)
    v[i] = new popinfoindexvector(initial[i + minage]);
}

Agebandmatrix::Agebandmatrix(int MinAge, const intvector& minl,
  const intvector& size) : minage(MinAge), nrow(size.Size()) {

  popinfo nullpop;
  assert(minl.Size() == size.Size());
  v = new popinfoindexvector*[nrow];
  int i;
  for (i = 0; i < nrow; i++)
    v[i] = new popinfoindexvector(size[i], minl[i], nullpop);
}

Agebandmatrix::Agebandmatrix(int MinAge, const popinfomatrix& initial)
  : minage(MinAge), nrow(initial.Nrow()) {

  v = new popinfoindexvector*[nrow];
  int i, j;
  int lower, upper;
  for (i = 0; i < nrow; i++) {
    lower = 0;
    upper = initial.Ncol(i) - 1;
    while (iszero(initial[i][lower].N) && lower < upper)
      lower++;
    while (iszero(initial[i][upper].N) && upper > lower)
      upper--;
    v[i] = new popinfoindexvector(upper - lower + 1, lower);
    for (j = lower; j <= upper; j++)
      (*v[i])[j] = initial[i][j];
  }
}

Agebandmatrix::Agebandmatrix(int MinAge, int minl, const popinfomatrix& initial)
  : minage(MinAge), nrow(initial.Nrow()) {

  v = new popinfoindexvector*[nrow];
  int i, j;
  int lower, upper;
  for (i = 0; i < nrow; i++) {
    lower = 0;
    upper = initial.Ncol(i) - 1;
    while (iszero(initial[i][lower].N) && (lower < upper))
      lower++;
    while (iszero(initial[i][upper].N) && (upper > lower))
      upper--;
    v[i] = new popinfoindexvector(upper - lower + 1, lower + minl);
    for (j = lower; j <= upper; j++)
      (*v[i])[j + minl] = initial[i][j];
  }
}

Agebandmatrix::Agebandmatrix(int MinAge, const popinfoindexvector& initial)
  : minage(MinAge), nrow(1) {

  v = new popinfoindexvector*[1];
  v[0] = new popinfoindexvector(initial);
}

Agebandmatrix::~Agebandmatrix() {
  int i;
  if (v != 0) {
    for (i = 0; i < nrow; i++)
      delete v[i];
    delete[] v;
    v = 0;
  }
}
