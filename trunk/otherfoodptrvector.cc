#include "otherfoodptrvector.h"
#include "gadget.h"

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

void OtherFoodPtrVector::resize(OtherFood* value) {
  int i;
  if (v == 0) {
    v = new OtherFood*[1];
  } else {
    OtherFood** vnew = new OtherFood*[size + 1];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
  }
  v[size] = value;
  size++;
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
