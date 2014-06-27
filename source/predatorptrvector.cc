#include "predatorptrvector.h"
#include "gadget.h"

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

void PredatorPtrVector::resize(Predator* value) {
  int i;
  if (v == 0) {
    v = new Predator*[1];
  } else {
    Predator** vnew = new Predator*[size + 1];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
  }
  v[size] = value;
  size++;
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
    size = 0;
  }
}
