#include "popinfovector.h"
#include "gadget.h"

#ifndef INLINE_VECTORS
#include "popinfovector.icc"
#endif

popinfovector::popinfovector(int sz) {
  size = (sz > 0 ? sz : 0);
  if (size > 0)
    v = new popinfo[size];
  else
    v = 0;
}

popinfovector::popinfovector(int sz, popinfo value) {
  size = (sz > 0 ? sz : 0);
  int i;
  if (size > 0) {
    v = new popinfo[size];
    for (i = 0; i < size; i++)
      v[i] = value;
  } else
    v = 0;
}

popinfovector::popinfovector(const popinfovector& initial) {
  size = initial.size;
  int i;
  if (size > 0) {
    v = new popinfo[size];
    for (i = 0; i < size; i++)
      v[i] = initial.v[i];
  } else
    v = 0;
}

//The function resize add addsize elements to a popinfovector and fills it vith value.
void popinfovector::resize(int addsize, popinfo value) {
  int oldsize = size;
  this->resize(addsize);
  int i;
  if (addsize > 0)
    for (i = oldsize; i < size; i++)
      v[i] = value;
}

void popinfovector::resize(int addsize) {
  int i;
  if (v == 0) {
    size = addsize;
    v = new popinfo[size];
  } else if (addsize > 0) {
    popinfo* vnew = new popinfo[addsize + size];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
    size += addsize;
  }
}

void popinfovector::Delete(int pos) {
  assert(size > 0);
  assert(0 <= pos && pos < size);
  popinfo* vnew = new popinfo[size - 1];
  int i;
  for (i = 0; i < pos; i++)
    vnew[i] = v[i];
  for (i = pos; i < size - 1; i++)
    vnew[i] = v[i + 1];
  delete[] v;
  v = vnew;
  size--;
}
