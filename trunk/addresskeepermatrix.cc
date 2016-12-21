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

void AddressKeeperMatrix::resize() {
  int i;
  if (v == 0) {
    v = new AddressKeeperVector*[1];
  } else {
    AddressKeeperVector** vnew = new AddressKeeperVector*[nrow + 1];
    for (i = 0; i < nrow; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
  }
  v[nrow] = new AddressKeeperVector();
  nrow++;
}

void AddressKeeperMatrix::Delete(int pos) {
  delete v[pos];
  int i;
  if (nrow > 1) {
    AddressKeeperVector** vnew = new AddressKeeperVector*[nrow - 1];
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
