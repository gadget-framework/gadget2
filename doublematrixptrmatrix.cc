#include "doublematrixptrmatrix.h"
#include "gadget.h"

#ifndef GADGET_INLINE
#include "doublematrixptrmatrix.icc"
#endif

DoubleMatrixPtrMatrix::DoubleMatrixPtrMatrix(int nr, int nc) {
  nrow = nr;
  v = new DoubleMatrixPtrVector*[nr];
  int i;
  for (i = 0; i < nr; i++)
    v[i] = new DoubleMatrixPtrVector(nc);
}

DoubleMatrixPtrMatrix::DoubleMatrixPtrMatrix(int nr, int nc, DoubleMatrix* value) {
  nrow = nr;
  v = new DoubleMatrixPtrVector*[nr];
  int i, j;
  for (i = 0; i < nr; i++)
    v[i] = new DoubleMatrixPtrVector(nc);
  for (i = 0; i < nr; i++) {
    for (j = 0; j < nc; j++)
      (*v[i])[j] = value;
  }
}

DoubleMatrixPtrMatrix::DoubleMatrixPtrMatrix(int nr, const IntVector& nc) {
  nrow = nr;
  v = new DoubleMatrixPtrVector*[nr];
  int i;
  for (i = 0; i < nr; i++)
    v[i] = new DoubleMatrixPtrVector(nc[i]);
}

DoubleMatrixPtrMatrix::DoubleMatrixPtrMatrix(int nr, const IntVector& nc, DoubleMatrix* value) {
  nrow = nr;
  v = new DoubleMatrixPtrVector*[nr];
  int i, j;
  for (i = 0; i < nr; i++)
    v[i] = new DoubleMatrixPtrVector(nc[i]);
  for (i = 0; i < nr; i++)
    for (j = 0; j < nc[i]; j++)
      (*v[i])[j] = value;
}

DoubleMatrixPtrMatrix::DoubleMatrixPtrMatrix(const DoubleMatrixPtrMatrix& initial) : nrow(initial.nrow) {
  int i;
  if (nrow >= 0) {
    v = new DoubleMatrixPtrVector*[nrow];
    for (i = 0; i < nrow; i++)
      v[i] = new DoubleMatrixPtrVector(initial[i]);
  } else {
    v = 0;
    nrow = 0;
  }
}

DoubleMatrixPtrMatrix::~DoubleMatrixPtrMatrix() {
  int i;
  if (v != 0) {
    for (i = 0; i < nrow; i++)
      delete v[i];
    delete[] v;
    v = 0;
  }
}

void DoubleMatrixPtrMatrix::AddRows(int add, int length) {
  DoubleMatrixPtrVector** vnew = new DoubleMatrixPtrVector*[nrow + add];
  int i;
  for (i = 0; i < nrow; i++)
    vnew[i] = v[i];
  delete[] v;
  v = vnew;
  for (i = nrow; i < nrow + add; i++)
    v[i] = new DoubleMatrixPtrVector(length);
  nrow += add;
}

void DoubleMatrixPtrMatrix::AddRows(int add, int length, DoubleMatrix* initial) {
  int oldnrow = nrow;
  this->AddRows(add, length);
  int i, j;
  for (i = oldnrow; i < nrow; i++)
    for (j = 0; j < length; j++)
      (*v[i])[j] = initial;
}

void DoubleMatrixPtrMatrix::DeleteRow(int row) {
  delete v[row];
  int i;
  if (nrow > 1) {
    DoubleMatrixPtrVector** vnew = new DoubleMatrixPtrVector*[nrow - 1];
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
