#include "optinfoptrvector.h"
#include "gadget.h"

OptInfoPtrVector::OptInfoPtrVector(const OptInfoPtrVector& initial) {
  size = initial.size;
  int i;
  if (size > 0) {
    v = new OptInfo*[size];
    for (i = 0; i < size; i++)
      v[i] = initial.v[i];
  } else
    v = 0;
}

OptInfoPtrVector::~OptInfoPtrVector() {
  if (v != 0) {
    delete[] v;
    v = 0;
  }
}

void OptInfoPtrVector::resize(OptInfo* value) {
  int i;
  if (v == 0) {
    v = new OptInfo*[1];
  } else {
    OptInfo** vnew = new OptInfo*[size + 1];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
  }
  v[size] = value;
  size++;
}

void OptInfoPtrVector::Delete(int pos) {
  int i;
  if (size > 1) {
    OptInfo** vnew = new OptInfo*[size - 1];
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
