#include "sionstepptrvector.h"
#include "gadget.h"

#ifndef INLINE_VECTORS
#include "sionstepptrvector.icc"
#endif

SIOnStepptrvector::SIOnStepptrvector(int sz) {
  size = (sz > 0 ? sz : 0);
  if (size > 0)
    v = new SIOnStep*[size];
  else
    v = 0;
}

SIOnStepptrvector::SIOnStepptrvector(int sz, SIOnStep* value) {
  size = (sz > 0 ? sz : 0);
  int i;
  if (size > 0) {
    v = new SIOnStep*[size];
    for (i = 0; i < size; i++)
      v[i] = value;
  } else
    v = 0;
}


SIOnStepptrvector::SIOnStepptrvector(const SIOnStepptrvector& initial) {
  size = initial.size;
  int i;
  if (size > 0) {
    v = new SIOnStep*[size];
    for (i = 0; i < size; i++)
      v[i] = initial.v[i];
  } else
    v = 0;
}

//The function resize add addsize elements to a SIOnStepptrvector and fills it vith value.
void SIOnStepptrvector::resize(int addsize, SIOnStep* value) {
  int oldsize = size;
  this->resize(addsize);
  int i;
  if (addsize > 0)
    for (i = oldsize; i < size; i++)
      v[i] = value;
}

void SIOnStepptrvector::resize(int addsize) {
  int i;
  if (v == 0) {
    size = addsize;
    v = new SIOnStep*[size];
  } else if (addsize > 0) {
    SIOnStep** vnew = new SIOnStep*[addsize + size];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
    size += addsize;
  }
}

void SIOnStepptrvector::Delete(int pos) {
  assert(size > 0);
  assert(0 <= pos && pos < size);
  SIOnStep** vnew = new SIOnStep*[size - 1];
  int i;
  for (i = 0; i < pos; i++)
    vnew[i] = v[i];
  for (i = pos; i < size - 1; i++)
    vnew[i] = v[i + 1];
  delete[] v;
  v = vnew;
  size--;
}
