#include "popratioindexvector.h"
#include "gadget.h"

#ifndef GADGET_INLINE
#include "popratioindexvector.icc"
#endif

popratioindexvector::popratioindexvector(int sz, int minp) {
  size = sz;
  int i;
  if (size > 0)
    v = new popratiovector*[size];
  else
    v = 0;

  for (i = 0; i < size; i++)
    v[i] = new popratiovector();

  minpos = minp;
}

popratioindexvector::popratioindexvector(int sz, int minp, const popratiovector& initial) {
  size = sz;
  minpos = minp;
  int i;
  if (size > 0) {
    v = new popratiovector*[size];
    for (i = 0; i < size; i++)
      v[i] = new popratiovector(initial);
  } else
    v = 0;
}

popratioindexvector::popratioindexvector(const popratioindexvector& initial) {
  size = initial.size;
  minpos = initial.minpos;
  int i;

  if (size > 0)  {
    v = new popratiovector*[size];
    for (i = 0; i < size; i++)
      v[i] = initial.v[i];

  } else
    v = 0;
}

popratioindexvector::~popratioindexvector() {
  int i;
  if (v != 0) {
    for (i = 0; i < size; i++)
      delete v[i];
    delete[] v;
    v = 0;
  }
}

void popratioindexvector::resize(int addsize, int lower, const popratiovector& initial) {
  popratiovector** vnew = new popratiovector*[size + addsize];
  int i;
  if (v != 0) {
    assert(lower <= minpos);
    for (i = 0; i < size; i++)
      vnew[i + minpos - lower] = v[i];
    for (i = size; i < size + addsize; i++)
      vnew[i] = new popratiovector(initial);
    for (i = 0; i < minpos - lower; i++)
      vnew[i] = new popratiovector(initial);
    size += addsize;
    delete[] v;
  } else {
    size = addsize;
    for (i = 0; i < size; i++)
      vnew[i] = new popratiovector(initial);
  }
  v = vnew;
  minpos = lower;
}
