#include "addresskeepervector.h"
#include "gadget.h"

#ifndef GADGET_INLINE
#include "addresskeepervector.icc"
#endif

AddressKeeperVector::AddressKeeperVector(int sz) {
  size = (sz > 0 ? sz : 0);
  if (size > 0)
    v = new AddressKeeper[size];
  else
    v = 0;
}

AddressKeeperVector::AddressKeeperVector(int sz, AddressKeeper value) {
  size = (sz > 0 ? sz : 0);
  int i;
  if (size > 0) {
    v = new AddressKeeper[size];
    for (i = 0; i < size; i++)
      v[i] = value;
  } else
    v = 0;
}

AddressKeeperVector::AddressKeeperVector(const AddressKeeperVector& initial) {
  size = initial.size;
  int i;
  if (size > 0) {
    v = new AddressKeeper[size];
    for (i = 0; i < size; i++)
      v[i] = initial.v[i];
  } else
    v = 0;
}

AddressKeeperVector::~AddressKeeperVector() {
  if (v != 0) {
    delete[] v;
    v = 0;
  }
}

void AddressKeeperVector::resize(int addsize, AddressKeeper value) {
  int oldsize = size;
  int i;
  this->resize(addsize);
  if (addsize > 0)
    for (i = oldsize; i < size; i++)
      v[i] = value;
}

void AddressKeeperVector::resize(int addsize) {
  int i;
  if (v == 0) {
    size = addsize;
    v = new AddressKeeper[size];
  } else if (addsize > 0) {
    AddressKeeper* vnew = new AddressKeeper[addsize + size];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
    size = addsize + size;
  }
}

void AddressKeeperVector::Delete(int pos) {
  assert(size > 0);
  assert(0 <= pos && pos < size);
  AddressKeeper* vnew = new AddressKeeper[size - 1];
  int i;
  for (i = 0; i < pos; i++)
    vnew[i] = v[i];
  for (i = pos; i < size - 1; i++)
    vnew[i] = v[i + 1];
  delete[] v;
  v = vnew;
  size--;
}
