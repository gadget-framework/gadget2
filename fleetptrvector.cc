#include "fleetptrvector.h"
#include "gadget.h"

#ifndef GADGET_INLINE
#include "fleetptrvector.icc"
#endif

Fleetptrvector::Fleetptrvector(int sz) {
  size = (sz > 0 ? sz : 0);
  if (size > 0)
    v = new Fleet*[size];
  else
    v = 0;
}

Fleetptrvector::Fleetptrvector(int sz, Fleet* value) {
  size = (sz > 0 ? sz : 0);
  int i;
  if (size > 0) {
    v = new Fleet*[size];
    for (i = 0; i < size; i++)
      v[i] = value;
  } else
    v = 0;
}

Fleetptrvector::Fleetptrvector(const Fleetptrvector& initial) {
  size = initial.size;
  int i;
  if (size > 0) {
    v = new Fleet*[size];
    for (i = 0; i < size; i++)
      v[i] = initial.v[i];
  } else
    v = 0;
}

Fleetptrvector::~Fleetptrvector() {
  if (v != 0) {
    delete[] v;
    v = 0;
  }
}

//The function resize add addsize elements to a Fleetptrvector and fills it vith value.
void Fleetptrvector::resize(int addsize, Fleet* value) {
  int oldsize = size;
  this->resize(addsize);
  int i;
  if (addsize > 0)
    for (i = oldsize; i < size; i++)
      v[i] = value;
}

void Fleetptrvector::resize(int addsize) {
  int i;
  if (v == 0) {
    size = addsize;
    v = new Fleet*[size];
  } else if (addsize > 0) {
    Fleet** vnew = new Fleet*[addsize + size];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
    size += addsize;
  }
}

void Fleetptrvector::Delete(int pos) {
  assert(size > 0);
  assert(0 <= pos && pos < size);
  Fleet** vnew = new Fleet*[size - 1];
  int i;
  for (i = 0; i < pos; i++)
    vnew[i] = v[i];
  for (i = pos; i < size - 1; i++)
    vnew[i] = v[i + 1];
  delete[] v;
  v = vnew;
  size--;
}
