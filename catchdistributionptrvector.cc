#include "catchdistributionptrvector.h"
#include "gadget.h"

#ifndef INLINE_VECTORS
#include "catchdistributionptrvector.icc"
#endif

CatchDistributionptrvector::CatchDistributionptrvector(int sz) {
  size = (sz > 0 ? sz : 0);
  if (size > 0)
    v = new CatchDistribution*[size];
  else
    v = 0;
}

CatchDistributionptrvector::CatchDistributionptrvector(int sz, CatchDistribution* value) {
  size = (sz > 0 ? sz : 0);
  int i;
  if (size > 0) {
    v = new CatchDistribution*[size];
    for (i = 0; i < size; i++)
      v[i] = value;
  } else
    v = 0;
}

CatchDistributionptrvector::CatchDistributionptrvector(const CatchDistributionptrvector& initial) {
  size = initial.size;
  int i;
  if (size > 0) {
    v = new CatchDistribution*[size];
    for (i = 0; i < size; i++)
      v[i] = initial.v[i];
  } else
    v = 0;
}

//The function resize add addsize elements to a CatchDistributionptrvector and fills it vith value.
void CatchDistributionptrvector::resize(int addsize, CatchDistribution* value) {
  int oldsize = size;
  this->resize(addsize);
  int i;
  if (addsize > 0)
    for (i = oldsize; i < size; i++)
      v[i] = value;
}

void CatchDistributionptrvector::resize(int addsize) {
  int i;
  if (v == 0) {
    size = addsize;
    v = new CatchDistribution* [size];
  } else if (addsize > 0) {
    CatchDistribution** vnew = new CatchDistribution*[addsize + size];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
    size += addsize;
  }
}

void CatchDistributionptrvector::Delete(int pos) {
  assert(size > 0);
  assert(0 <= pos && pos < size);
  CatchDistribution** vnew = new CatchDistribution*[size - 1];
  int i;
  for (i = 0; i < pos; i++)
    vnew[i] = v[i];
  for (i = pos; i < size - 1; i++)
    vnew[i] = v[i + 1];
  delete[] v;
  v = vnew;
  size--;
}
