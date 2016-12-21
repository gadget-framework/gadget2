#include "doublematrixptrmatrix.h"
#include "gadget.h"

DoubleMatrixPtrMatrix::~DoubleMatrixPtrMatrix() {
  int i;
  if (v != 0) {
    for (i = 0; i < nrow; i++)
      delete v[i];
    delete[] v;
    v = 0;
  }
}

void DoubleMatrixPtrMatrix::resize() {
  int i;
  if (v == 0) {
    v = new DoubleMatrixPtrVector*[1];
  } else {
    DoubleMatrixPtrVector** vnew = new DoubleMatrixPtrVector*[nrow + 1];
    for (i = 0; i < nrow; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
  }
  v[nrow] = new DoubleMatrixPtrVector();
  nrow++;
}

void DoubleMatrixPtrMatrix::Delete(int pos) {
  delete v[pos];
  int i;
  if (nrow > 1) {
    DoubleMatrixPtrVector** vnew = new DoubleMatrixPtrVector*[nrow - 1];
    for (i = 0; i < pos; i++)
      vnew[i] = v[i];
    for (i = pos; i < nrow - 1; i++)
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
