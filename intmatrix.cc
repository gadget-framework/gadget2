#include "intmatrix.h"
#include "gadget.h"

IntMatrix::IntMatrix(int nr, int nc) {
  nrow = nr;
  v = new IntVector*[nr];
  int i;
  for (i = 0; i < nr; i++)
    v[i] = new IntVector(nc);
}

IntMatrix::IntMatrix(int nr, int nc, int value) {
  nrow = nr;
  v = new IntVector*[nr];
  int i, j;
  for (i = 0; i < nr; i++)
    v[i] = new IntVector(nc);
  for (i = 0; i < nr; i++)
    for (j = 0; j < nc; j++)
      (*v[i])[j] = value;
}

IntMatrix::IntMatrix(int nr, const IntVector& nc) {
  nrow = nr;
  v = new IntVector*[nr];
  int i;
  for (i = 0; i < nr; i++)
    v[i] = new IntVector(nc[i]);
}

IntMatrix::IntMatrix(int nr, const IntVector& nc, int value) {
  nrow = nr;
  v = new IntVector*[nr];
  int i, j;
  for (i = 0; i < nr; i++)
    v[i] = new IntVector(nc[i]);
  for (i = 0; i < nr; i++)
    for (j = 0; j < nc[i]; j++)
      (*v[i])[j] = value;
}

IntMatrix::IntMatrix(const IntMatrix& initial) : nrow(initial.nrow) {
  int i;
  if (nrow >= 0) {
    v = new IntVector*[nrow];
    for (i = 0; i < nrow; i++)
      v[i] = new IntVector(initial[i]);
  } else {
    v = 0;
    nrow = 0;
  }
}

IntMatrix::~IntMatrix() {
  int i;
  if (v != 0) {
    for (i = 0; i < nrow; i++)
      delete v[i];
    delete[] v;
    v = 0;
  }
}

void IntMatrix::AddRows(int add, int length) {
  IntVector** vnew = new IntVector*[nrow + add];
  int i;
  for (i = 0; i < nrow; i++)
    vnew[i] = v[i];
  delete[] v;
  v = vnew;
  for (i = nrow; i < nrow + add; i++)
    v[i] = new IntVector(length);
  nrow += add;
}

void IntMatrix::AddRows(int add, int length, int initial) {
  int oldnrow = nrow;
  this->AddRows(add, length);
  int i, j;
  for (i = oldnrow; i < nrow; i++)
    for (j = 0; j < length; j++)
      (*v[i])[j] = initial;
}

void IntMatrix::DeleteRow(int row) {
  delete v[row];
  int i;
  if (nrow > 1) {
    IntVector** vnew = new IntVector*[nrow -1];
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

void IntMatrix::Reset() {
  if (nrow > 0) {
    int i;
    for (i = 0; i < nrow; i++)
      delete v[i];
    delete[] v;
    v = 0;
    nrow = 0;
  }
}
