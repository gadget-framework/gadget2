#include "doublematrixptrmatrix.h"
#include "gadget.h"

DoubleMatrixPtrMatrix::DoubleMatrixPtrMatrix(int nr, int nc) {
  nrow = nr;
  v = new DoubleMatrixPtrVector*[nr];
  int i;
  for (i = 0; i < nr; i++)
    v[i] = new DoubleMatrixPtrVector(nc);
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
