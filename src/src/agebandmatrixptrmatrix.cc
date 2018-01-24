#include "agebandmatrixptrmatrix.h"

AgeBandMatrixPtrMatrix::~AgeBandMatrixPtrMatrix() {
  int i;
  if (v != 0) {
    for (i = 0; i < nrow; i++)
      delete v[i];
    delete[] v;
    v = 0;
  }
}

void AgeBandMatrixPtrMatrix::resize(AgeBandMatrixPtrVector* value) {
  int i;
  if (v == 0) {
    v = new AgeBandMatrixPtrVector*[1];
  } else {
    AgeBandMatrixPtrVector** vnew = new AgeBandMatrixPtrVector*[nrow + 1];
    for (i = 0; i < nrow; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
  }
  v[nrow] = value;
  nrow++;
}

void AgeBandMatrixPtrMatrix::Delete(int pos) {
  delete v[pos];
  int i;
  if (nrow > 1) {
    AgeBandMatrixPtrVector** vnew = new AgeBandMatrixPtrVector*[nrow - 1];
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
