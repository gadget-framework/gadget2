#include "bandmatrixptrvector.h"
#include "gadget.h"

BandMatrixPtrVector::BandMatrixPtrVector(int sz) {
  size = sz;
  if (size > 0)
    v = new BandMatrix*[size];
  else
    v = 0;
}

BandMatrixPtrVector::BandMatrixPtrVector(int sz, BandMatrix* value) {
  size = sz;
  int i;
  if (size > 0) {
    v = new BandMatrix*[size];
    for (i = 0; i < size; i++)
      v[i] = value;
  } else
    v = 0;
}

BandMatrixPtrVector::~BandMatrixPtrVector() {
  if (v != 0) {
    delete[] v;
    v = 0;
  }
}

void BandMatrixPtrVector::resize(int addsize, BandMatrix* value) {
  int oldsize = size;
  this->resize(addsize);
  int i;
  if (addsize > 0)
    for (i = oldsize; i < size; i++)
      v[i] = value;
}

void BandMatrixPtrVector::resize(int addsize) {
  int i;
  if (v == 0) {
    size = addsize;
    v = new BandMatrix*[size];
  } else if (addsize > 0) {
    BandMatrix** vnew = new BandMatrix*[addsize + size];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
    size += addsize;
  }
}

void BandMatrixPtrVector::Delete(int pos) {
  int i;
  if (size > 1) {
    BandMatrix** vnew = new BandMatrix*[size - 1];
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
