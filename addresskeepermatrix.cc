#include "addresskeepermatrix.h"
#include "gadget.h"

#ifndef GADGET_INLINE
#include "addresskeepermatrix.icc"
#endif

AddressKeeperMatrix::AddressKeeperMatrix(int nr, int nc) {
  nrow = nr;
  int i;
  v = new AddressKeeperVector*[nr];
  for (i = 0; i < nr; i++)
    v[i] = new AddressKeeperVector(nc);
}

AddressKeeperMatrix::AddressKeeperMatrix(int nr, int nc, AddressKeeper value) {
  nrow = nr;
  v = new AddressKeeperVector*[nr];
  int i, j;
  for (i = 0; i < nr; i++)
    v[i] = new AddressKeeperVector(nc);
  for (i = 0; i < nr; i++) {
    for (j = 0; j < nc; j++)
      (*v[i])[j] = value;
  }
}

AddressKeeperMatrix::AddressKeeperMatrix(int nr, const IntVector& nc) {
  nrow = nr;
  int i;
  v = new AddressKeeperVector*[nr];
  for (i = 0; i < nr; i++)
    v[i] = new AddressKeeperVector(nc[i]);
}

AddressKeeperMatrix::AddressKeeperMatrix(int nr, const IntVector& nc, AddressKeeper value) {
  nrow = nr;
  v = new AddressKeeperVector*[nr];
  int i, j;
  for (i = 0; i < nr; i++)
    v[i] = new AddressKeeperVector(nc[i]);
  for (i = 0; i < nr; i++)
    for (j = 0; j < nc[i]; j++)
      (*v[i])[j] = value;
}

AddressKeeperMatrix::AddressKeeperMatrix(const AddressKeeperMatrix& initial) : nrow(initial.nrow) {
  int i;
  if (nrow >= 0) {
    v = new AddressKeeperVector*[nrow];
    for (i = 0; i < nrow; i++)
      v[i] = new AddressKeeperVector(initial[i]);
  } else {
    v = 0;
    nrow = 0;
  }
}

AddressKeeperMatrix::~AddressKeeperMatrix() {
  int i;
  if (v != 0) {
    for (i = 0; i < nrow; i++)
      delete v[i];
    delete[] v;
    v = 0;
  }
}

void AddressKeeperMatrix::AddRows(int add, int length) {
  AddressKeeperVector** vnew = new AddressKeeperVector*[nrow + add];
  int i;
  for (i = 0; i < nrow; i++)
    vnew[i] = v[i];
  delete[] v;
  v = vnew;
  for (i = nrow; i < nrow + add; i++)
    v[i] = new AddressKeeperVector(length);
  nrow += add;
}

void AddressKeeperMatrix::AddRows(int add, int length, AddressKeeper initial) {
  int oldnrow = nrow;
  int i, j;
  this->AddRows(add, length);
  for (i = oldnrow; i < nrow; i++)
    for (j = 0; j < length; j++)
      (*v[i])[j] = initial;
}

void AddressKeeperMatrix::DeleteRow(int row) {
  delete v[row];
  int i;
  if (nrow > 1) {
    AddressKeeperVector** vnew = new AddressKeeperVector*[nrow - 1];
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
