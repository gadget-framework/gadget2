#include "addresskeepervector.h"
#include "gadget.h"

AddressKeeperVector::AddressKeeperVector(int sz) {
  size = (sz > 0 ? sz : 0);
  if (size > 0)
    v = new AddressKeeper[size];
  else
    v = 0;
}

AddressKeeperVector::~AddressKeeperVector() {
  if (v != 0) {
    delete[] v;
    v = 0;
  }
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
  int i;
  if (size > 1) {
    AddressKeeper* vnew = new AddressKeeper[size - 1];
    for (i = 0; i < pos; i++)
      vnew[i] = v[i];
    for (i = pos; i < size - 1; i++)
      vnew[i] = v[i + 1];
    delete[] v;
    v = vnew;
    size--;
  } else {
    delete[] v;
    v = 0;
    size = 0;
  }
}
