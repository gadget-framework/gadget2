#include "addr_keepvector.h"
#include "gadget.h"

#ifndef INLINE_VECTORS
#include "addr_keepvector.icc"
#endif

addr_keepvector::addr_keepvector(int sz) {
  size = (sz > 0 ? sz : 0);
  if (size > 0)
    v = new addr_keep[size];
  else
    v = 0;
}

addr_keepvector::addr_keepvector(int sz, addr_keep value) {
  size = (sz > 0 ? sz : 0);
  int i;
  if (size > 0) {
    v = new addr_keep[size];
    for (i = 0; i < size; i++)
      v[i] = value;
  } else
    v = 0;
}

addr_keepvector::addr_keepvector(const addr_keepvector& initial) {
  size = initial.size;
  int i;
  if (size > 0) {
    v = new addr_keep[size];
    for (i = 0; i < size; i++)
      v[i] = initial.v[i];
  } else
    v = 0;
}

//The function resize add addsize elements to a addr_keepvector and fills it vith value.
void addr_keepvector::resize(int addsize, addr_keep value) {
  int oldsize = size;
  int i;
  this->resize(addsize);
  if (addsize > 0)
    for (i = oldsize; i < size; i++)
      v[i] = value;
}

void addr_keepvector::resize(int addsize) {
  int i;
  if (v == 0) {
    size = addsize;
    v = new addr_keep[size];
  } else if (addsize > 0) {
    addr_keep* vnew = new addr_keep[addsize + size];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
    size = addsize + size;
  }
}

void addr_keepvector::Delete(int pos) {
  assert(size > 0);
  assert(0 <= pos && pos < size);
  addr_keep* vnew = new addr_keep[size - 1];
  int i;
  for (i = 0; i < pos; i++)
    vnew[i] = v[i];
  for (i = pos; i < size - 1; i++)
    vnew[i] = v[i + 1];
  delete[] v;
  v = vnew;
  size--;
}
