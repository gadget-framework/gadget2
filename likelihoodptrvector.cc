#include "likelihoodptrvector.h"
#include "gadget.h"

#ifndef INLINE_VECTORS
#include "likelihoodptrvector.icc"
#endif

Likelihoodptrvector::Likelihoodptrvector(int sz) {
  size = (sz > 0 ? sz : 0);
  if (size > 0)
    v = new Likelihood*[size];
  else
    v = 0;
}

Likelihoodptrvector::Likelihoodptrvector(int sz, Likelihood* value) {
  size = (sz > 0 ? sz : 0);
  int i;
  if (size > 0) {
    v = new Likelihood*[size];
    for (i = 0; i < size; i++)
      v[i] = value;
  } else
    v = 0;
}

Likelihoodptrvector::Likelihoodptrvector(const Likelihoodptrvector& initial) {
  size = initial.size;
  int i;
  if (size > 0) {
    v = new Likelihood*[size];
    for (i = 0; i < size; i++)
      v[i] = initial.v[i];
  } else
    v = 0;
}

//The function resize add addsize elements to a Likelihoodptrvector and fills it vith value.
void Likelihoodptrvector::resize(int addsize, Likelihood* value) {
  int oldsize = size;
  this->resize(addsize);
  int i;
  if (addsize > 0)
    for (i = oldsize; i < size; i++)
      v[i] = value;
}

void Likelihoodptrvector::resize(int addsize) {
  int i;
  if (v == 0) {
    size = addsize;
    v = new Likelihood*[size];
  } else if (addsize > 0) {
    Likelihood** vnew = new Likelihood*[addsize + size];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
    size += addsize;
  }
}

void Likelihoodptrvector::Delete(int pos) {
  assert(size > 0);
  assert(0 <= pos && pos < size);
  Likelihood** vnew = new Likelihood*[size - 1];
  int i;
  for (i = 0; i < pos; i++)
    vnew[i] = v[i];
  for (i = pos; i < size - 1; i++)
    vnew[i] = v[i + 1];
  delete[] v;
  v = vnew;
  size--;
}
