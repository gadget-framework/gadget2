#include "bandmatrixptrvector.h"
#include "gadget.h"

BandMatrixPtrVector::BandMatrixPtrVector(int sz) {
  size = (sz > 0 ? sz : 0);
  int i;
  if (size > 0) {
    v = new BandMatrix*[size];
    for (i = 0; i < size; i++)
      v[i] = 0;
  } else
    v = 0;
}

BandMatrixPtrVector::~BandMatrixPtrVector() {
  int i;
  if (v != 0) {
    for (i = 0; i < size; i++)
      delete v[i];
    delete[] v;
    v = 0;
  }
}

void BandMatrixPtrVector::changeElement(int nr, const BandMatrix& value) {
  if (v[nr] != 0)
    delete v[nr];
  v[nr] = new BandMatrix(value);
}

void BandMatrixPtrVector::resize(int addsize) {
  int i;
  if (addsize <= 0)
    return;
  if (v == 0) {
    size = addsize;
    v = new BandMatrix*[size];
    for (i = 0; i < size; i++)
      v[i] = 0;
  } else {
    BandMatrix** vnew = new BandMatrix*[addsize + size];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    for (i = size; i < size + addsize; i++)
      vnew[i] = 0;
    delete[] v;
    v = vnew;
    size += addsize;
  }
}

void BandMatrixPtrVector::resize(int addsize, const BandMatrix& initial) {
  int i;
  if (addsize <= 0)
    return;
  if (v == 0) {
    size = addsize;
    v = new BandMatrix*[size];
    for (i = 0; i < size; i++)
      v[i] = new BandMatrix(initial);
  } else if (addsize > 0) {
    BandMatrix** vnew = new BandMatrix*[addsize + size];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    for (i = size; i < size + addsize; i++)
      vnew[i] = new BandMatrix(initial);
    delete[] v;
    v = vnew;
    size += addsize;
  }
}

void BandMatrixPtrVector::Delete(int pos) {
  delete v[pos];
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
