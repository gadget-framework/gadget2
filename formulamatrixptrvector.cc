#include "formulamatrixptrvector.h"
#include "gadget.h"

#ifndef GADGET_INLINE
#include "formulamatrixptrvector.icc"
#endif

Formulamatrixptrvector::Formulamatrixptrvector(int sz) {
  size = (sz > 0 ? sz : 0);
  if (size > 0)
    v = new Formulamatrix*[size];
  else
    v = 0;
}

Formulamatrixptrvector::Formulamatrixptrvector(int sz, Formulamatrix* value) {
  size = (sz > 0 ? sz : 0);
  int i;
  if (size > 0) {
    v = new Formulamatrix*[size];
    for (i = 0; i < size; i++)
      v[i] = value;
  } else
    v = 0;
}

Formulamatrixptrvector::Formulamatrixptrvector(const Formulamatrixptrvector& initial) {
  size = initial.size;
  int i;
  if (size > 0) {
    v = new Formulamatrix*[size];
    for (i = 0; i < size; i++)
      v[i] = initial.v[i];
  } else
    v = 0;
}

Formulamatrixptrvector::~Formulamatrixptrvector() {
  if (v != 0) {
    delete[] v;
    v = 0;
  }
}

//The function resize add addsize elements to a Formulamatrixptrvector and fills it vith value.
void Formulamatrixptrvector::resize(int addsize, Formulamatrix* value) {
  int oldsize = size;
  this->resize(addsize);
  int i;
  if (addsize > 0)
    for (i = oldsize; i < size; i++)
      v[i] = value;
}

void Formulamatrixptrvector::resize(int addsize) {
  int i;
  if (v == 0) {
    size = addsize;
    v = new Formulamatrix*[size];
  } else if (addsize > 0) {
    Formulamatrix** vnew = new Formulamatrix*[addsize + size];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
    size += addsize;
  }
}

void Formulamatrixptrvector::Delete(int pos) {
  assert(size > 0);
  assert(0 <= pos && pos < size);
  Formulamatrix** vnew = new Formulamatrix*[size - 1];
  int i;
  for (i = 0; i < pos; i++)
    vnew[i] = v[i];
  for (i = pos; i < size - 1; i++)
    vnew[i] = v[i + 1];
  delete[] v;
  v = vnew;
  size--;
}
