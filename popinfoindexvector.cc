#include "popinfoindexvector.h"
#include "gadget.h"

#ifndef GADGET_INLINE
#include "popinfoindexvector.icc"
#endif

PopInfoIndexVector::PopInfoIndexVector(int sz, int minp) {
  size = sz;
  if (size > 0)
    v = new PopInfo[size];
  else
    v = 0;
  minpos = minp;
}

PopInfoIndexVector::PopInfoIndexVector(int sz, int minp, PopInfo value) {
  size = sz;
  minpos = minp;
  int i;
  if (size > 0) {
    v = new PopInfo[size];
    for (i = 0; i < size; i++)
      v[i] = value;
  } else
    v = 0;
}

PopInfoIndexVector::PopInfoIndexVector(const PopInfoIndexVector& initial) {
  minpos = initial.minpos;
  size = initial.size;
  v = new PopInfo[size];
  int i;
  for (i = 0; i < size; i++)
    v[i] = initial.v[i];
}

PopInfoIndexVector::~PopInfoIndexVector() {
  if (v != 0) {
    delete[] v;
    v = 0;
  }
}

void PopInfoIndexVector::resize(int addsize, int lower, PopInfo initial) {
  PopInfo* vnew = new PopInfo[size + addsize];
  int i;
  if (v != 0) {
    assert(lower <= minpos);
    for (i = 0; i < size; i++)
      vnew[i + minpos - lower] = v[i];
    for (i = size; i < size + addsize; i++)
      vnew[i] = initial;
    for (i = 0; i < minpos - lower; i++)
      vnew[i] = initial;
    size += addsize;
    delete[] v;
  } else {
    size = addsize;
    for (i = 0; i < size; i++)
      vnew[i] = initial;
  }
  v = vnew;
  minpos = lower;
}
