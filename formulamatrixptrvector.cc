#include "formulamatrixptrvector.h"
#include "errorhandler.h"
#include "gadget.h"

extern ErrorHandler handle;

FormulaMatrixPtrVector::FormulaMatrixPtrVector(int sz) {
  size = (sz > 0 ? sz : 0);
  if (size > 0)
    v = new FormulaMatrix*[size];
  else
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
  if (addsize != 1)
    handle.logMessage(LOGFAIL, "Error in formulamatrxptrvector - cannot add entries to vector");

  this->resize(addsize);
  v[size - 1] = value;
}

void FormulaMatrixPtrVector::resize(int addsize) {
  if (addsize <= 0)
    return;
  int i;
  if (v == 0) {
    size = addsize;
    v = new FormulaMatrix*[size];
  } else {
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
