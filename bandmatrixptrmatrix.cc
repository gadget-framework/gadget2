#include "bandmatrixptrmatrix.h"
#include "gadget.h"

BandMatrixPtrMatrix::BandMatrixPtrMatrix(int Nrow, int ncol) : nrow(Nrow) {
  int i;
  if (nrow == 0) {
    v = 0;
    return;
  }
  v = new BandMatrixPtrVector*[nrow];
  for (i = 0; i < nrow; i++)
    v[i] = new BandMatrixPtrVector(ncol);
}

BandMatrixPtrMatrix::~BandMatrixPtrMatrix() {
  int i;
  if (v != 0) {
    for (i = 0; i < nrow; i++)
      delete v[i];
    delete[] v;
    v = 0;
  }
}

void BandMatrixPtrMatrix::changeElement(int row, int col, const BandMatrix& value) {
  assert(0 <= row && row < nrow);
  v[row]->changeElement(col, value);
}

void BandMatrixPtrMatrix::AddRows(int addrow, int ncol) {
  int i;
  if (addrow <= 0)
    return;
  if (v == 0) {
    nrow = addrow;
    v = new BandMatrixPtrVector*[nrow];
    for (i = 0; i < nrow; i++)
      v[i] = new BandMatrixPtrVector(ncol);
  } else {
    BandMatrixPtrVector** vnew = new BandMatrixPtrVector*[addrow + nrow];
    for (i = 0; i < nrow; i++)
      vnew[i] = v[i];
    for (i = nrow; i < nrow + addrow; i++)
      vnew[i] = new BandMatrixPtrVector(ncol);
    delete[] v;
    v = vnew;
    nrow += addrow;
  }
}

void BandMatrixPtrMatrix::DeleteRow(int row) {
  delete v[row];
  int i;
  if (nrow > 1) {
    BandMatrixPtrVector** vnew = new BandMatrixPtrVector*[nrow - 1];
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
