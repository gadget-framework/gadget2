#include "doublematrixptrvector.h"
#include "gadget.h"

#ifndef INLINE_VECTORS
#include "doublematrixptrvector.icc"
#endif

doublematrixptrvector::doublematrixptrvector(int sz) {
  size = (sz > 0 ? sz : 0);
  if (size > 0)
    v = new doublematrix*[size];
  else
    v = 0;
}

doublematrixptrvector::doublematrixptrvector(int sz, doublematrix* value) {
  size = (sz > 0 ? sz : 0);
  int i;
  if (size > 0) {
    v = new doublematrix*[size];
    for (i = 0; i < size; i++)
      v[i] = value;
  } else
    v = 0;
}

doublematrixptrvector::doublematrixptrvector(const doublematrixptrvector& initial) {
  size = initial.size;
  int i;
  if (size > 0) {
    v = new doublematrix*[size];
    for (i = 0; i < size; i++)
      v[i] = initial.v[i];
  } else
    v = 0;
}

//The function resize add addsize elements to a doublematrixptrvector and fills it vith value.
void doublematrixptrvector::resize(int addsize, doublematrix* value) {
  int oldsize = size;
  this->resize(addsize);
  int i;
  if (addsize > 0)
    for (i = oldsize; i < size; i++)
      v[i] = value;
}

void doublematrixptrvector::resize(int addsize) {
  int i;
  if (v == 0) {
    size = addsize;
    v = new doublematrix*[size];
  } else if (addsize > 0) {
    doublematrix** vnew = new doublematrix*[addsize + size];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
    size += addsize;
  }
}

void doublematrixptrvector::Delete(int pos) {
  assert(size > 0);
  assert(0 <= pos && pos < size);
  doublematrix** vnew = new doublematrix*[size - 1];
  int i;
  for (i = 0; i < pos; i++)
    vnew[i] = v[i];
  for (i = pos; i < size - 1; i++)
    vnew[i] = v[i + 1];
  delete[] v;
  v = vnew;
  size--;
}
