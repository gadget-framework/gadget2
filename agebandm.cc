#include "intvector.h"
#include "popinfomatrix.h"
#include "popinfoindexvector.h"
#include "agebandm.h"
#include "mathfunc.h"

#ifndef GADGET_INLINE
#include "agebandm.icc"
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

Agebandmatrixvector::Agebandmatrixvector(int size1, int minage,
  const intvector& minl, const intvector& size2) {

  assert(size1 >= 0);
  size = size1;
  int i;
  if (size == 0) {
    v = 0;
  } else {
    v = new Agebandmatrix*[size];
    for (i = 0; i < size; i++)
      v[i] = new Agebandmatrix(minage, minl, size2);
  }
}

Agebandmatrixvector::~Agebandmatrixvector() {
  int i;
  if (v != 0) {
    for (i = 0; i < size; i++)
      delete v[i];
    delete[] v;
    v = 0;
  }
}

void Agebandmatrixvector::resize(int addsize, int minage,
  int minl, const popinfomatrix& matr) {

  assert(addsize > 0);
  int i;
  if (v == 0) {
    size = addsize;
    v = new Agebandmatrix*[size];
    for (i = 0; i < size; i++)
      v[i] = new Agebandmatrix(minage, minl, matr);

  } else {
    Agebandmatrix** vnew = new Agebandmatrix*[addsize + size];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
    for (i = size; i < addsize + size; i++)
      v[i] = new Agebandmatrix(minage, minl, matr);
    size = addsize + size;
  }
}

void Agebandmatrixvector::ChangeElement(int nr, const Agebandmatrix& value) {
  assert(0 <= nr && nr < size);
  delete v[nr];
  v[nr] = new Agebandmatrix(value);
}

void Agebandmatrixvector::resize(int addsize, Agebandmatrix* matr) {
  assert(addsize > 0);
  int i;
  if (v == 0) {
    size = addsize;
    v = new Agebandmatrix*[size];
    for (i = 0; i < size; i++)
      v[i] = matr;
  } else {
    Agebandmatrix** vnew = new Agebandmatrix*[addsize + size];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
    for (i = size; i < addsize + size; i++)
      v[i] = matr;
    size = addsize + size;
  }
}

void Agebandmatrixvector::resize(int addsize, int minage,
  const intvector& minl, const intvector& lsize) {

  assert(addsize > 0);
  Agebandmatrix** vnew = new Agebandmatrix*[size + addsize];
  int i;
  for (i = 0; i < size; i++)
    vnew[i] = v[i];
  for (i = size; i < size + addsize; i++)
    vnew[i] = new Agebandmatrix(minage, minl, lsize);
  delete[] v;
  v = vnew;
  size += addsize;
}
