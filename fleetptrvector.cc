#include "fleetptrvector.h"
#include "errorhandler.h"
#include "gadget.h"

extern ErrorHandler handle;

FleetPtrVector::FleetPtrVector(int sz) {
  size = (sz > 0 ? sz : 0);
  if (size > 0)
    v = new Fleet*[size];
  else
    v = 0;
}

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

void FleetPtrVector::resize(int addsize, Fleet* value) {
  if (addsize != 1)
    handle.logMessage(LOGFAIL, "Error in fleetptrvector - cannot add entries to vector");

  this->resize(addsize);
  v[size - 1] = value;
}

void FleetPtrVector::resize(int addsize) {
  if (addsize <= 0)
    return;
  int i;
  if (v == 0) {
    size = addsize;
    v = new Fleet*[size];
  } else {
    Fleet** vnew = new Fleet*[addsize + size];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
    size += addsize;
  }
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
