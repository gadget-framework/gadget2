#include "otherfoodptrvector.h"
#include "gadget.h"

#ifndef GADGET_INLINE
#include "otherfoodptrvector.icc"
#endif

OtherFoodPtrVector::OtherFoodPtrVector(int sz) {
  size = (sz > 0 ? sz : 0);
  if (size > 0)
    v = new OtherFood*[size];
  else
    v = 0;
}

OtherFoodPtrVector::OtherFoodPtrVector(int sz, OtherFood* value) {
  size = (sz > 0 ? sz : 0);
  int i;
  if (size > 0) {
    v = new OtherFood*[size];
    for (i = 0; i < size; i++)
      v[i] = value;
  } else
    v = 0;
}

OtherFoodPtrVector::OtherFoodPtrVector(const OtherFoodPtrVector& initial) {
  size = initial.size;
  int i;
  if (size > 0) {
    v = new OtherFood*[size];
    for (i = 0; i < size; i++)
      v[i] = initial.v[i];
  } else
    v = 0;
}

OtherFoodPtrVector::~OtherFoodPtrVector() {
  if (v != 0) {
    delete[] v;
    v = 0;
  }
}

void OtherFoodPtrVector::resize(int addsize, OtherFood* value) {
  int oldsize = size;
  this->resize(addsize);
  int i;
  if (addsize > 0)
    for (i = oldsize; i < size; i++)
      v[i] = value;
}

void OtherFoodPtrVector::resize(int addsize) {
  int i;
  if (v == 0) {
    size = addsize;
    v = new OtherFood*[size];
  } else if (addsize > 0) {
    OtherFood** vnew = new OtherFood*[addsize + size];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
    size += addsize;
  }
}

void OtherFoodPtrVector::Delete(int pos) {
  int i;
  if (size > 1) {
    OtherFood** vnew = new OtherFood*[size - 1];
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
