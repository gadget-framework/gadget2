#include "agebandmatrixptrmatrix.h"

AgeBandMatrixPtrMatrix::AgeBandMatrixPtrMatrix(int sz) {
  size = (sz > 0 ? sz : 0);
  if (size > 0)
    v = new AgeBandMatrixPtrVector*[size];
  else
    v = 0;
}
AgeBandMatrixPtrMatrix::AgeBandMatrixPtrMatrix(int sz, AgeBandMatrixPtrVector* value) {
  assert(value != NULL);
  size = (sz > 0 ? sz : 0);
  int i;
  if (size > 0) {
    v = new AgeBandMatrixPtrVector*[size];
    for (i = 0; i < size; i++)
      v[i] = value;
  } else
    v = 0;
}

AgeBandMatrixPtrMatrix::~AgeBandMatrixPtrMatrix() {
  int i;
  if (v != 0) {
    for (i = 0; i < size; i++)
      delete v[i];
    delete[] v;
    v = 0;
  }
}

void AgeBandMatrixPtrMatrix::resize(int addsize, AgeBandMatrixPtrVector* value) {
  int oldsize = size;
  this->resize(addsize);
  int i;
  if (addsize > 0)
    for (i = oldsize; i < size; i++)
      v[i] = value;
}

void AgeBandMatrixPtrMatrix::resize(int addsize) {
  int i;
  if (v == 0) {
    size = addsize;
    v = new AgeBandMatrixPtrVector*[size];
  } else if (addsize > 0) {
    AgeBandMatrixPtrVector** vnew = new AgeBandMatrixPtrVector*[addsize + size];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
    size += addsize;
  }
}

void AgeBandMatrixPtrMatrix::Delete(int pos) {
  delete v[pos];
  int i;
  if (size > 1) {
    AgeBandMatrixPtrVector** vnew = new AgeBandMatrixPtrVector*[size - 1];
    for (i = 0; i < pos; i++)
      vnew[i] = v[i];
    for (i = pos; i < size - 1; i++)
      vnew[i] = v[i + 1];
    delete[] v;
    v = vnew;
    size--;
  } else {
    delete[] v;
    v = 0;
    size = 0;
  }
}
