#include "agebandmatrixptrvector.h"

#ifndef GADGET_INLINE
#include "agebandmatrixptrvector.icc"
#endif

agebandmatrixptrvector::agebandmatrixptrvector(int sz) {
  size = (sz > 0 ? sz : 0);
  if (size > 0)
    v = new Agebandmatrix*[size];
  else
    v = 0;
}
agebandmatrixptrvector::agebandmatrixptrvector(int sz, Agebandmatrix* value) {
  assert(value != NULL);
  size = (sz > 0 ? sz : 0);
  int i;
  if (size > 0) {
    v = new Agebandmatrix*[size];
    for (i = 0; i < size; i++)
      v[i] = value;
  } else
    v = 0;
}

agebandmatrixptrvector::agebandmatrixptrvector(int size1, int minage,
  const intvector& minl, const intvector& size2) {

  assert(size1 >= 0);
  size = size1;
  int i;
  if (size == 0) {
    v = 0;
  } else {
    v = new Agebandmatrix*[size];
    for (i = 0; i < size; i++)
      v[i] = new Agebandmatrix(minage, minl, size2);
  }
}

agebandmatrixptrvector::~agebandmatrixptrvector() {
  if (v != 0) {
    delete[] v;
    v = 0;
  }
}

//The function resize add addsize elements to a agebandmatrixptrvector and fills it vith value.
void agebandmatrixptrvector::resize(int addsize, Agebandmatrix* value) {
  int oldsize = size;
  this->resize(addsize);
  int i;
  if (addsize > 0)
    for (i = oldsize; i < size; i++)
      v[i] = value;
}

void agebandmatrixptrvector::resize(int addsize) {
  int i;
  if (v == 0) {
    size = addsize;
    v = new Agebandmatrix*[size];
  } else if (addsize > 0) {
    Agebandmatrix** vnew = new Agebandmatrix*[addsize + size];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
    size += addsize;
  }
}

void agebandmatrixptrvector::resize(int addsize, int minage,
  const intvector& minl, const intvector& lsize) {

  assert(addsize > 0);
  Agebandmatrix** vnew = new Agebandmatrix*[size + addsize];
  int i;
  for (i = 0; i < size; i++)
    vnew[i] = v[i];
  for (i = size; i < size + addsize; i++)
    vnew[i] = new Agebandmatrix(minage, minl, lsize);
  delete[] v;
  v = vnew;
  size += addsize;
}

void agebandmatrixptrvector::resize(int addsize, int minage,
  int minl, const popinfomatrix& matr) {

  assert(addsize > 0);
  int i;
  if (v == 0) {
    size = addsize;
    v = new Agebandmatrix*[size];
    for (i = 0; i < size; i++)
      v[i] = new Agebandmatrix(minage, minl, matr);

  } else {
    Agebandmatrix** vnew = new Agebandmatrix*[addsize + size];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
    for (i = size; i < addsize + size; i++)
      v[i] = new Agebandmatrix(minage, minl, matr);
    size = addsize + size;
  }
}

void agebandmatrixptrvector::Delete(int pos) {
  assert(size > 0);
  assert(0 <= pos && pos < size);
  Agebandmatrix** vnew = new Agebandmatrix*[size - 1];
  int i;
  for (i = 0; i < pos; i++)
    vnew[i] = v[i];
  for (i = pos; i < size - 1; i++)
    vnew[i] = v[i + 1];
  delete[] v;
  v = vnew;
  size--;
}
