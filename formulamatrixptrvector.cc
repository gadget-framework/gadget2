#include "formulamatrixptrvector.h"
#include "gadget.h"

FormulaMatrixPtrVector::FormulaMatrixPtrVector(int sz) {
  size = (sz > 0 ? sz : 0);
  if (size > 0)
    v = new FormulaMatrix*[size];
  else
    v = 0;
}

FormulaMatrixPtrVector::FormulaMatrixPtrVector(int sz, FormulaMatrix* value) {
  size = (sz > 0 ? sz : 0);
  int i;
  if (size > 0) {
    v = new FormulaMatrix*[size];
    for (i = 0; i < size; i++)
      v[i] = value;
  } else
    v = 0;
}

FormulaMatrixPtrVector::FormulaMatrixPtrVector(const FormulaMatrixPtrVector& initial) {
  size = initial.size;
  int i;
  if (size > 0) {
    v = new FormulaMatrix*[size];
    for (i = 0; i < size; i++)
      v[i] = initial.v[i];
  } else
    v = 0;
}

FormulaMatrixPtrVector::~FormulaMatrixPtrVector() {
  if (v != 0) {
    delete[] v;
    v = 0;
  }
}

void FormulaMatrixPtrVector::resize(int addsize, FormulaMatrix* value) {
  int oldsize = size;
  this->resize(addsize);
  int i;
  if (addsize > 0)
    for (i = oldsize; i < size; i++)
      v[i] = value;
}

void FormulaMatrixPtrVector::resize(int addsize) {
  int i;
  if (v == 0) {
    size = addsize;
    v = new FormulaMatrix*[size];
  } else if (addsize > 0) {
    FormulaMatrix** vnew = new FormulaMatrix*[addsize + size];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
    size += addsize;
  }
}

void FormulaMatrixPtrVector::Delete(int pos) {
  int i;
  if (size > 1) {
    FormulaMatrix** vnew = new FormulaMatrix*[size - 1];
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
