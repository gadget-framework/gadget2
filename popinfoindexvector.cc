#include "popinfoindexvector.h"
#include "gadget.h"

popinfoindexvector::popinfoindexvector(int sz, int minp) {
  size = sz;
  if (size > 0)
    v = new popinfo[size];
  else
    v = 0;
  minpos = minp;
}

popinfoindexvector::popinfoindexvector(int sz, int minp, popinfo value) {
  size = sz;
  minpos = minp;
  int i;
  if (size > 0) {
    v = new popinfo[size];
    for (i = 0; i < size; i++)
      v[i] = value;
  } else
    v = 0;
}

popinfoindexvector::popinfoindexvector(const popinfoindexvector& initial) {
  minpos = initial.minpos;
  size = initial.size;
  v = new popinfo[size];
  int i;
  for (i = 0; i < size; i++)
    v[i] = initial.v[i];
}

void popinfoindexvector::resize(int addsize, int lower, popinfo initial) {
  popinfo* vnew = new popinfo[size + addsize];
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

popinfo& popinfoindexvector::operator [] (int pos) {
  assert(minpos <= pos && pos < minpos + size);
  return(v[pos - minpos]);
}

const popinfo& popinfoindexvector::operator [] (int pos) const {
  assert(minpos <= pos && pos < minpos + size);
  return(v[pos - minpos]);
}
