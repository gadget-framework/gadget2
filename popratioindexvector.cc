#include "popratioindexvector.h"
#include "gadget.h"

PopRatioIndexVector::PopRatioIndexVector(int sz, int minp) {
  size = sz;
  int i;
  if (size > 0)
    v = new PopRatioVector*[size];
  else
    v = 0;

  for (i = 0; i < size; i++)
    v[i] = new PopRatioVector();

  minpos = minp;
}

PopRatioIndexVector::PopRatioIndexVector(int sz, int minp, const PopRatioVector& initial) {
  size = sz;
  minpos = minp;
  int i;
  if (size > 0) {
    v = new PopRatioVector*[size];
    for (i = 0; i < size; i++)
      v[i] = new PopRatioVector(initial);
  } else
    v = 0;
}

PopRatioIndexVector::PopRatioIndexVector(const PopRatioIndexVector& initial) {
  size = initial.size;
  minpos = initial.minpos;
  int i;

  if (size > 0)  {
    v = new PopRatioVector*[size];
    for (i = 0; i < size; i++)
      v[i] = initial.v[i];

  } else
    v = 0;
}

PopRatioIndexVector::~PopRatioIndexVector() {
  int i;
  if (v != 0) {
    for (i = 0; i < size; i++)
      delete v[i];
    delete[] v;
    v = 0;
  }
}

void PopRatioIndexVector::resize(int addsize, int lower, const PopRatioVector& initial) {
  PopRatioVector** vnew = new PopRatioVector*[size + addsize];
  int i;
  if (v != 0) {
    assert(lower <= minpos);
    for (i = 0; i < size; i++)
      vnew[i + minpos - lower] = v[i];
    for (i = size; i < size + addsize; i++)
      vnew[i] = new PopRatioVector(initial);
    for (i = 0; i < minpos - lower; i++)
      vnew[i] = new PopRatioVector(initial);
    size += addsize;
    delete[] v;
  } else {
    size = addsize;
    for (i = 0; i < size; i++)
      vnew[i] = new PopRatioVector(initial);
  }
  v = vnew;
  minpos = lower;
}
