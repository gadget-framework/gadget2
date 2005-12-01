#include "preyptrvector.h"
#include "errorhandler.h"
#include "gadget.h"

extern ErrorHandler handle;

PreyPtrVector::PreyPtrVector(int sz) {
  size = (sz > 0 ? sz : 0);
  if (size > 0)
    v = new Prey*[size];
  else
    v = 0;
}

PreyPtrVector::PreyPtrVector(const PreyPtrVector& initial) {
  size = initial.size;
  int i;
  if (size > 0) {
    v = new Prey*[size];
    for (i = 0; i < size; i++)
      v[i] = initial.v[i];
  } else
    v = 0;
}

PreyPtrVector::~PreyPtrVector() {
  if (v != 0) {
    delete[] v;
    v = 0;
  }
}

void PreyPtrVector::resize(int addsize, Prey* value) {
  if (addsize != 1)
    handle.logMessage(LOGFAIL, "Error in preyptrvector - cannot add entries to vector");

  this->resize(addsize);
  v[size - 1] = value;
}

void PreyPtrVector::resize(int addsize) {
  if (addsize <= 0)
    return;
  int i;
  if (v == 0) {
    size = addsize;
    v = new Prey*[size];
  } else {
    Prey** vnew = new Prey*[addsize + size];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
    size += addsize;
  }
}

void PreyPtrVector::Delete(int pos) {
  int i;
  if (size > 1) {
    Prey** vnew = new Prey*[size - 1];
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
  }
}
