#include "baseclassptrvector.h"
#include "errorhandler.h"
#include "gadget.h"

extern ErrorHandler handle;

BaseClassPtrVector::BaseClassPtrVector(int sz) {
  size = (sz > 0 ? sz : 0);
  if (size > 0)
    v = new BaseClass*[size];
  else
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
  if (addsize != 1)
    handle.logMessage(LOGFAIL, "Error in baseclassptrvector - cannot add entries to vector");

  this->resize(addsize);
  v[size - 1] = value;
}

void BaseClassPtrVector::resize(int addsize) {
  if (addsize <= 0)
    return;
  int i;
  if (v == 0) {
    size = addsize;
    v = new BaseClass*[size];
  } else {
    BaseClass** vnew = new BaseClass*[addsize + size];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
    size += addsize;
  }
}
