#include "agebandmatrixptrvector.h"
#include "errorhandler.h"
#include "gadget.h"
#include "global.h"

AgeBandMatrixPtrVector::AgeBandMatrixPtrVector(int sz) {
  size = (sz > 0 ? sz : 0);
  if (size > 0)
    v = new AgeBandMatrix*[size];
  else
    v = 0;
}

AgeBandMatrixPtrVector::AgeBandMatrixPtrVector(const AgeBandMatrixPtrVector& initial) {
  size = initial.size;
  int i;
  if (size > 0) {
    v = new AgeBandMatrix*[size];
    for (i = 0; i < size; i++)
      v[i] = initial.v[i];
  } else
    v = 0;
}

AgeBandMatrixPtrVector::AgeBandMatrixPtrVector(int sz, int minage,
  const IntVector& minl, const IntVector& lsize) {

  int i;
  size = (sz > 0 ? sz : 0);
  if (size > 0) {
    v = new AgeBandMatrix*[size];
    for (i = 0; i < size; i++)
      v[i] = new AgeBandMatrix(minage, minl, lsize);
  } else
    v = 0;
}

AgeBandMatrixPtrVector::~AgeBandMatrixPtrVector() {
  int i;
  if (v != 0) {
    for (i = 0; i < size; i++)
      delete v[i];
    delete[] v;
    v = 0;
  }
}

void AgeBandMatrixPtrVector::resize(int addsize, AgeBandMatrix* value) {
  if (addsize != 1)
    handle.logMessage(LOGFAIL, "Error in agebandmatrixptrvector - cannot add entries to vector");

  this->resize(addsize);
  v[size - 1] = value;
}

void AgeBandMatrixPtrVector::resize(int addsize) {
  if (addsize <= 0)
    return;
  int i;
  if (v == 0) {
    size = addsize;
    v = new AgeBandMatrix*[size];
  } else {
    AgeBandMatrix** vnew = new AgeBandMatrix*[addsize + size];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
    size += addsize;
  }
}

void AgeBandMatrixPtrVector::resize(int addsize, int minage,
  const IntVector& minl, const IntVector& lsize) {

  if (addsize <= 0)
    return;

  int i;
  if (v == 0) {
    size = addsize;
    v = new AgeBandMatrix*[size];
    for (i = 0; i < size; i++)
      v[i] = new AgeBandMatrix(minage, minl, lsize);

  } else {
    AgeBandMatrix** vnew = new AgeBandMatrix*[size + addsize];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    for (i = size; i < size + addsize; i++)
      vnew[i] = new AgeBandMatrix(minage, minl, lsize);
    delete[] v;
    v = vnew;
    size += addsize;
  }
}

void AgeBandMatrixPtrVector::resize(int addsize, int minage,
  int minl, const PopInfoMatrix& matr) {

  if (addsize <= 0)
    return;

  int i;
  if (v == 0) {
    size = addsize;
    v = new AgeBandMatrix*[size];
    for (i = 0; i < size; i++)
      v[i] = new AgeBandMatrix(minage, matr, minl);

  } else {
    AgeBandMatrix** vnew = new AgeBandMatrix*[addsize + size];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    for (i = size; i < addsize + size; i++)
      vnew[i] = new AgeBandMatrix(minage, matr, minl);
    delete[] v;
    v = vnew;
    size += addsize;
  }
}

void AgeBandMatrixPtrVector::Delete(int pos) {
  int i;
  if (size > 1) {
    AgeBandMatrix** vnew = new AgeBandMatrix*[size - 1];
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
