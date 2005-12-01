#include "likelihoodptrvector.h"
#include "errorhandler.h"
#include "gadget.h"

extern ErrorHandler handle;

LikelihoodPtrVector::LikelihoodPtrVector(int sz) {
  size = (sz > 0 ? sz : 0);
  if (size > 0)
    v = new Likelihood*[size];
  else
    v = 0;
}

LikelihoodPtrVector::LikelihoodPtrVector(const LikelihoodPtrVector& initial) {
  size = initial.size;
  int i;
  if (size > 0) {
    v = new Likelihood*[size];
    for (i = 0; i < size; i++)
      v[i] = initial.v[i];
  } else
    v = 0;
}

LikelihoodPtrVector::~LikelihoodPtrVector() {
  if (v != 0) {
    delete[] v;
    v = 0;
  }
}

void LikelihoodPtrVector::resize(int addsize, Likelihood* value) {
  if (addsize != 1)
    handle.logMessage(LOGFAIL, "Error in likelihoodptrvector - cannot add entries to vector");

  this->resize(addsize);
  v[size - 1] = value;
}

void LikelihoodPtrVector::resize(int addsize) {
  if (addsize <= 0)
    return;
  int i;
  if (v == 0) {
    size = addsize;
    v = new Likelihood*[size];
  } else {
    Likelihood** vnew = new Likelihood*[addsize + size];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
    size += addsize;
  }
}

void LikelihoodPtrVector::Delete(int pos) {
  int i;
  if (size > 1) {
    Likelihood** vnew = new Likelihood*[size - 1];
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
