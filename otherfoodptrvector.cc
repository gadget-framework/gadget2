#include "otherfoodptrvector.h"
#include "gadget.h"

#ifndef GADGET_INLINE
#include "otherfoodptrvector.icc"
#endif

OtherFoodptrvector::OtherFoodptrvector(int sz) {
  size = (sz > 0 ? sz : 0);
  if (size > 0)
    v = new OtherFood*[size];
  else
    v = 0;
}

OtherFoodptrvector::OtherFoodptrvector(int sz, OtherFood* value) {
  size = (sz > 0 ? sz : 0);
  int i;
  if (size > 0) {
    v = new OtherFood*[size];
    for (i = 0; i < size; i++)
      v[i] = value;
  } else
    v = 0;
}

OtherFoodptrvector::OtherFoodptrvector(const OtherFoodptrvector& initial) {
  size = initial.size;
  int i;
  if (size > 0) {
    v = new OtherFood*[size];
    for (i = 0; i < size; i++)
      v[i] = initial.v[i];
  } else
    v = 0;
}

OtherFoodptrvector::~OtherFoodptrvector() {
  if (v != 0) {
    delete[] v;
    v = 0;
  }
}

//The function resize add addsize elements to a OtherFoodptrvector and fills it vith value.
void OtherFoodptrvector::resize(int addsize, OtherFood* value) {
  int oldsize = size;
  this->resize(addsize);
  int i;
  if (addsize > 0)
    for (i = oldsize; i < size; i++)
      v[i] = value;
}

void OtherFoodptrvector::resize(int addsize) {
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

void OtherFoodptrvector::Delete(int pos) {
  assert(size > 0);
  assert(0 <= pos && pos < size);
  OtherFood** vnew = new OtherFood*[size - 1];
  int i;
  for (i = 0; i < pos; i++)
    vnew[i] = v[i];
  for (i = pos; i < size - 1; i++)
    vnew[i] = v[i + 1];
  delete[] v;
  v = vnew;
  size--;
}
