#include "baseclassptrvector.h"
#include "gadget.h"

BaseClassPtrVector::BaseClassPtrVector(int sz) {
  size = (sz > 0 ? sz : 0);
  if (size > 0)
    v = new BaseClass*[size];
  else
    v = 0;
}

BaseClassPtrVector::BaseClassPtrVector(int sz, BaseClass* value) {
  size = (sz > 0 ? sz : 0);
  int i;
  if (size > 0) {
    v = new BaseClass*[size];
    for (i = 0; i < size; i++)
      v[i] = value;
  } else
    v = 0;
}

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

void BaseClassPtrVector::resize(int addsize, BaseClass* value) {
  int oldsize = size;
  this->resize(addsize);
  int i;
  if (addsize > 0)
    for (i = oldsize; i < size; i++)
      v[i] = value;
}

void BaseClassPtrVector::resize(int addsize) {
  int i;
  if (v == 0) {
    size = addsize;
    v = new BaseClass*[size];
  } else if (addsize > 0) {
    BaseClass** vnew = new BaseClass*[addsize + size];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
    size += addsize;
  }
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
