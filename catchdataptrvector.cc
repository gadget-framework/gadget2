#include "catchdataptrvector.h"
#include "gadget.h"

#ifndef INLINE_VECTORS
#include "catchdataptrvector.icc"
#endif

CatchDataptrvector::CatchDataptrvector(int sz) {
  size = (sz > 0 ? sz : 0);
  if (size > 0)
    v = new CatchData*[size];
  else
    v = 0;
}

CatchDataptrvector::CatchDataptrvector(int sz, CatchData* value) {
  size = (sz > 0 ? sz : 0);
  int i;
  if (size > 0) {
    v = new CatchData*[size];
    for (i = 0; i < size; i++)
      v[i] = value;
  } else
    v = 0;
}

CatchDataptrvector::CatchDataptrvector(const CatchDataptrvector& initial) {
  size = initial.size;
  int i;
  if (size > 0) {
    v = new CatchData*[size];
    for (i = 0; i < size; i++)
      v[i] = initial.v[i];
  } else
    v = 0;
}

//The function resize add addsize elements to a CatchDataptrvector and fills it vith value.
void CatchDataptrvector::resize(int addsize, CatchData* value) {
  int oldsize = size;
  this->resize(addsize);
  int i;
  if (addsize > 0)
    for (i = oldsize; i < size; i++)
      v[i] = value;
}

void CatchDataptrvector::resize(int addsize) {
  int i;
  if (v == 0) {
    size = addsize;
    v = new CatchData*[size];
  } else if (addsize > 0) {
    CatchData** vnew = new CatchData*[addsize + size];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
    size += addsize;
  }
}

void CatchDataptrvector::Delete(int pos) {
  assert(size > 0);
  assert(0 <= pos && pos < size);
  CatchData** vnew = new CatchData*[size - 1];
  int i;
  for (i = 0; i < pos; i++)
    vnew[i] = v[i];
  for (i = pos; i < size - 1; i++)
    vnew[i] = v[i + 1];
  delete[] v;
  v = vnew;
  size--;
}
