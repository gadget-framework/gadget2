#include "popratiomatrix.h"
#include "gadget.h"

#ifndef GADGET_INLINE
#include "popratiomatrix.icc"
#endif

PopRatioMatrix::PopRatioMatrix(int nr, int nc) {
  nrow = nr;
  v = new PopRatioVector*[nr];
  int i;
  for (i = 0; i < nr; i++)
    v[i] = new PopRatioVector(nc);
}

PopRatioMatrix::PopRatioMatrix(int nr, int nc, PopRatio value) {
  nrow = nr;
  v = new PopRatioVector*[nr];
  int i, j;
  for (i = 0; i < nr; i++)
    v[i] = new PopRatioVector(nc);
  for (i = 0; i < nr; i++)
    for (j = 0; j < nc; j++)
      (*v[i])[j] = value;
}

PopRatioMatrix::PopRatioMatrix(int nr, const IntVector& nc) {
  nrow = nr;
  v = new PopRatioVector*[nr];
  int i;
  for (i = 0; i < nr; i++)
    v[i] = new PopRatioVector(nc[i]);
}

PopRatioMatrix::PopRatioMatrix(int nr, const IntVector& nc, PopRatio value) {
  nrow = nr;
  v = new PopRatioVector*[nr];
  int i, j;
  for (i = 0; i < nr; i++)
    v[i] = new PopRatioVector(nc[i]);
  for (i = 0; i < nr; i++)
    for (j = 0; j < nc[i]; j++)
      (*v[i])[j] = value;
}

PopRatioMatrix::PopRatioMatrix(const PopRatioMatrix& initial) : nrow(initial.nrow) {
  int i;
  if (nrow >= 0) {
    v = new PopRatioVector*[nrow];
    for (i = 0; i < nrow; i++)
      v[i] = new PopRatioVector(initial[i]);
  } else {
    v = 0;
    nrow = 0;
  }
}

PopRatioMatrix::~PopRatioMatrix() {
  int i;
  if (v != 0) {
    for (i = 0; i < nrow; i++)
      delete v[i];
    delete[] v;
    v = 0;
  }
}

void PopRatioMatrix::AddRows(int add, int length) {
  PopRatioVector** vnew = new PopRatioVector*[nrow + add];
  int i;
  for (i = 0; i < nrow; i++)
    vnew[i] = v[i];
  delete[] v;
  v = vnew;
  for (i = nrow; i < nrow + add; i++)
    v[i] = new PopRatioVector(length);
  nrow += add;
}

void PopRatioMatrix::AddRows(int add, int length, PopRatio initial) {
  int oldnrow = nrow;
  this->AddRows(add, length);
  int i, j;
  for (i = oldnrow; i < nrow; i++)
    for (j = 0; j < length; j++)
      (*v[i])[j] = initial;
}

void PopRatioMatrix::DeleteRow(int row) {
  delete v[row];
  int i;
  if (nrow > 1) {
    PopRatioVector** vnew = new PopRatioVector*[nrow - 1];
    for (i = 0; i < row; i++)
      vnew[i] = v[i];
    for (i = row; i < nrow - 1; i++)
      vnew[i] = v[i + 1];
    delete[] v;
    v = vnew;
    nrow--;
  } else {
    delete[] v;
    v = 0;
    nrow = 0;
  }
}
