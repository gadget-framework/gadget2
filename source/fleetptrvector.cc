#include "fleetptrvector.h"
#include "gadget.h"

FleetPtrVector::FleetPtrVector(const FleetPtrVector& initial) {
  size = initial.size;
  int i;
  if (size > 0) {
    v = new Fleet*[size];
    for (i = 0; i < size; i++)
      v[i] = initial.v[i];
  } else
    v = 0;
}

FleetPtrVector::~FleetPtrVector() {
  if (v != 0) {
    delete[] v;
    v = 0;
  }
}

void FleetPtrVector::resize(Fleet* value) {
  int i;
  if (v == 0) {
    v = new Fleet*[1];
  } else {
    Fleet** vnew = new Fleet*[size + 1];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
  }
  v[size] = value;
  size++;
}

void FleetPtrVector::Delete(int pos) {
  int i;
  if (size > 1) {
    Fleet** vnew = new Fleet*[size - 1];
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
