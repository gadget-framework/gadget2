#include "likelihoodptrvector.h"
#include "gadget.h"

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

void LikelihoodPtrVector::resize(Likelihood* value) {
  int i;
  if (v == 0) {
    v = new Likelihood*[1];
  } else {
    Likelihood** vnew = new Likelihood*[size + 1];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
  }
  v[size] = value;
  size++;
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
