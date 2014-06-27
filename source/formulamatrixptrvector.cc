#include "formulamatrixptrvector.h"
#include "gadget.h"

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

void FormulaMatrixPtrVector::resize(FormulaMatrix* value) {
  int i;
  if (v == 0) {
    v = new FormulaMatrix*[1];
  } else {
    FormulaMatrix** vnew = new FormulaMatrix*[size + 1];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
  }
  v[size] = value;
  size++;
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
