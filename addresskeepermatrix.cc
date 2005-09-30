#include "addresskeepermatrix.h"
#include "gadget.h"

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
