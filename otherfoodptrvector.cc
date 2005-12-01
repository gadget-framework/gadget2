#include "otherfoodptrvector.h"
#include "errorhandler.h"
#include "gadget.h"

extern ErrorHandler handle;

OtherFoodPtrVector::OtherFoodPtrVector(int sz) {
  size = (sz > 0 ? sz : 0);
  if (size > 0)
    v = new OtherFood*[size];
  else
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
  if (addsize != 1)
    handle.logMessage(LOGFAIL, "Error in otherfoodptrvector - cannot add entries to vector");

  this->resize(addsize);
  v[size - 1] = value;
}

void OtherFoodPtrVector::resize(int addsize) {
  int i;
  if (addsize <= 0)
    return;
  if (v == 0) {
    size = addsize;
    v = new OtherFood*[size];
  } else {
    OtherFood** vnew = new OtherFood*[addsize + size];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
    size += addsize;
  }
}
