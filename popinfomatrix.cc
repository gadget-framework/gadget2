#include "popinfomatrix.h"
#include "gadget.h"

PopInfoMatrix::PopInfoMatrix(int nr, int nc) {
  nrow = nr;
  v = new PopInfoVector*[nr];
  int i;
  for (i = 0; i < nr; i++)
    v[i] = new PopInfoVector(nc);
}

PopInfoMatrix::PopInfoMatrix(int nr, int nc, PopInfo value) {
  nrow = nr;
  v = new PopInfoVector*[nr];
  int i, j;
  for (i = 0; i < nr; i++)
    v[i] = new PopInfoVector(nc);
  for (i = 0; i < nr; i++) {
    for (j = 0; j < nc; j++)
      (*v[i])[j] = value;
  }
}

PopInfoMatrix::PopInfoMatrix(int nr, const IntVector& nc) {
  nrow = nr;
  v = new PopInfoVector*[nr];
  int i;
  for (i = 0; i < nr; i++)
    v[i] = new PopInfoVector(nc[i]);
}

PopInfoMatrix::PopInfoMatrix(int nr, const IntVector& nc, PopInfo value) {
  nrow = nr;
  v = new PopInfoVector*[nr];
  int i, j;
  for (i = 0; i < nr; i++)
    v[i] = new PopInfoVector(nc[i]);
  for (i = 0; i < nr; i++)
    for (j = 0; j < nc[i]; j++)
      (*v[i])[j] = value;
}

PopInfoMatrix::PopInfoMatrix(const PopInfoMatrix& initial) : nrow(initial.nrow) {
  int i;
  if (nrow >= 0) {
    v = new PopInfoVector*[nrow];
    for (i = 0; i < nrow; i++)
      v[i] = new PopInfoVector(initial[i]);
  } else {
    v = 0;
    nrow = 0;
  }
}

PopInfoMatrix::~PopInfoMatrix() {
  int i;
  if (v != 0) {
    for (i = 0; i < nrow; i++)
      delete v[i];
    delete[] v;
    v = 0;
  }
}

void PopInfoMatrix::AddRows(int add, int length) {
  PopInfoVector** vnew = new PopInfoVector*[nrow + add];
  int i;
  for (i = 0; i < nrow; i++)
    vnew[i] = v[i];
  delete[] v;
  v = vnew;
  for (i = nrow; i < nrow + add; i++)
    v[i] = new PopInfoVector(length);
  nrow += add;
}

void PopInfoMatrix::AddRows(int add, int length, PopInfo initial) {
  int oldnrow = nrow;
  this->AddRows(add, length);
  int i, j;
  for (i = oldnrow; i < nrow; i++)
    for (j = 0; j < length; j++)
      (*v[i])[j] = initial;
}

void PopInfoMatrix::DeleteRow(int row) {
  delete v[row];
  int i;
  if (nrow > 1) {
    PopInfoVector** vnew = new PopInfoVector*[nrow - 1];
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
