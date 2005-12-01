#include "agebandmatrixptrmatrix.h"

AgeBandMatrixPtrMatrix::AgeBandMatrixPtrMatrix(int nr) {
  nrow = (nr > 0 ? nr : 0);
  if (nrow > 0)
    v = new AgeBandMatrixPtrVector*[nrow];
  else
    v = 0;
}

AgeBandMatrixPtrMatrix::~AgeBandMatrixPtrMatrix() {
  int i;
  if (v != 0) {
    for (i = 0; i < nrow; i++)
      delete v[i];
    delete[] v;
    v = 0;
  }
}

void AgeBandMatrixPtrMatrix::AddRows(int add, AgeBandMatrixPtrVector* value) {
  int oldnrow = nrow;
  this->AddRows(add);
  int i;
  if (add > 0)
    for (i = oldnrow; i < nrow; i++)
      v[i] = value;
}

void AgeBandMatrixPtrMatrix::AddRows(int add) {
  int i;
  if (v == 0) {
    nrow = add;
    v = new AgeBandMatrixPtrVector*[nrow];
  } else if (add > 0) {
    AgeBandMatrixPtrVector** vnew = new AgeBandMatrixPtrVector*[add + nrow];
    for (i = 0; i < nrow; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
    nrow += add;
  }
}

void AgeBandMatrixPtrMatrix::DeleteRow(int row) {
  delete v[row];
  int i;
  if (nrow > 1) {
    AgeBandMatrixPtrVector** vnew = new AgeBandMatrixPtrVector*[nrow - 1];
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
