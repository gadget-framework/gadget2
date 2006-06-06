#include "popinfoindexvector.h"
#include "gadget.h"

PopInfoIndexVector::PopInfoIndexVector(int sz, int minp) {
  size = (sz > 0 ? sz : 0);
  minpos = minp;
  if (size > 0)
    v = new PopInfo[size];
  else
    v = 0;
}

PopInfoIndexVector::PopInfoIndexVector(int sz, int minp, PopInfo value) {
  size = (sz > 0 ? sz : 0);
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
  size = initial.size;
  minpos = initial.minpos;
  int i;
  if (size > 0) {
    v = new PopInfo[size];
    for (i = 0; i < size; i++)
      v[i] = initial.v[i];
  } else
    v = 0;
}

PopInfoIndexVector::~PopInfoIndexVector() {
  if (v != 0) {
    delete[] v;
    v = 0;
  }
}

void PopInfoIndexVector::resize(int addsize, int lower, PopInfo initial) {
  if (addsize <= 0)
    return;

  int i;
  if (v == 0) {
    size = addsize;
    minpos = lower;
    v = new PopInfo[size];
    for (i = 0; i < size; i++)
      v[i] = initial;

  } else {
    PopInfo* vnew = new PopInfo[size + addsize];
    for (i = 0; i < size; i++)
      vnew[i + minpos - lower] = v[i];
    delete[] v;
    v = vnew;
    for (i = size; i < size + addsize; i++)
      v[i] = initial;
    for (i = 0; i < minpos - lower; i++)
      v[i] = initial;
    size += addsize;
    minpos = lower;
  }
}
