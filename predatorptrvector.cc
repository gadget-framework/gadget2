#include "predatorptrvector.h"
#include "errorhandler.h"
#include "gadget.h"

extern ErrorHandler handle;

PredatorPtrVector::PredatorPtrVector(int sz) {
  size = (sz > 0 ? sz : 0);
  if (size > 0)
    v = new Predator*[size];
  else
    v = 0;
}

PredatorPtrVector::PredatorPtrVector(const PredatorPtrVector& initial) {
  size = initial.size;
  int i;
  if (size > 0) {
    v = new Predator*[size];
    for (i = 0; i < size; i++)
      v[i] = initial.v[i];
  } else
    v = 0;
}

PredatorPtrVector::~PredatorPtrVector() {
  if (v != 0) {
    delete[] v;
    v = 0;
  }
}

void PredatorPtrVector::resize(int addsize, Predator* value) {
  if (addsize != 1)
    handle.logMessage(LOGFAIL, "Error in predatorptrvector - cannot add entries to vector");

  this->resize(addsize);
  v[size - 1] = value;
}

void PredatorPtrVector::resize(int addsize) {
  if (addsize <= 0)
    return;
  int i;
  if (v == 0) {
    size = addsize;
    v = new Predator*[size];
  } else {
    Predator** vnew = new Predator*[addsize + size];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
    size += addsize;
  }
}

void PredatorPtrVector::Delete(int pos) {
  int i;
  if (size > 1) {
    Predator** vnew = new Predator*[size - 1];
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
