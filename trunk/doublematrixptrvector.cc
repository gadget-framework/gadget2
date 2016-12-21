#include "doublematrixptrvector.h"
#include "gadget.h"

DoubleMatrixPtrVector::DoubleMatrixPtrVector(int sz) {
  size = (sz > 0 ? sz : 0);
  if (size > 0)
    v = new DoubleMatrix*[size];
  else
    v = 0;
}

DoubleMatrixPtrVector::DoubleMatrixPtrVector(const DoubleMatrixPtrVector& initial) {
  size = initial.size;
  int i;
  if (size > 0) {
    v = new DoubleMatrix*[size];
    for (i = 0; i < size; i++)
      v[i] = initial.v[i];
  } else
    v = 0;
}

DoubleMatrixPtrVector::~DoubleMatrixPtrVector() {
  if (v != 0) {
    delete[] v;
    v = 0;
  }
}

void DoubleMatrixPtrVector::resize(DoubleMatrix* value) {
  int i;
  if (v == 0) {
    v = new DoubleMatrix*[1];
  } else {
    DoubleMatrix** vnew = new DoubleMatrix*[size + 1];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
  }
  v[size] = value;
  size++;
}

void DoubleMatrixPtrVector::resizeBlank(int addsize) {
  if (addsize <= 0)
    return;
  int i;
  if (v == 0) {
    size = addsize;
    v = new DoubleMatrix*[size];
  } else {
    DoubleMatrix** vnew = new DoubleMatrix*[addsize + size];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
    size += addsize;
  }
}

void DoubleMatrixPtrVector::Delete(int pos) {
  int i;
  if (size > 1) {
    DoubleMatrix** vnew = new DoubleMatrix*[size - 1];
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
