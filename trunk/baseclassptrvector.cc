#include "baseclassptrvector.h"
#include "gadget.h"

BaseClassPtrVector::BaseClassPtrVector(const BaseClassPtrVector& initial) {
  size = initial.size;
  int i;
  if (size > 0) {
    v = new BaseClass*[size];
    for (i = 0; i < size; i++)
      v[i] = initial.v[i];
  } else
    v = 0;
}

BaseClassPtrVector::~BaseClassPtrVector() {
  if (v != 0) {
    delete[] v;
    v = 0;
  }
}

void BaseClassPtrVector::resize(BaseClass* value) {
  int i;
  if (v == 0) {
    v = new BaseClass*[1];
  } else {
    BaseClass** vnew = new BaseClass*[size + 1];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
  }
  v[size] = value;
  size++;
}

void BaseClassPtrVector::Delete(int pos) {
  int i;
  if (size > 1) {
    BaseClass** vnew = new BaseClass*[size - 1];
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
