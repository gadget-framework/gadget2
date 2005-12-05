#include "formulamatrix.h"
#include "gadget.h"

FormulaMatrix::FormulaMatrix(int nr, int nc, Formula initial) {
  int i;
  nrow = nr;
  if (nrow > 0) {
    v = new FormulaVector*[nr];
    for (i = 0; i < nr; i++)
      v[i] = new FormulaVector(nc, initial);
  } else
    v = 0;
}

FormulaMatrix::FormulaMatrix(int nr, int nc, double initial) {
  int i;
  nrow = nr;
  if (nrow > 0) {
    v = new FormulaVector*[nr];
    for (i = 0; i < nr; i++)
      v[i] = new FormulaVector(nc, initial);
  } else
    v = 0;
}

FormulaMatrix::~FormulaMatrix() {
  int i;
  if (v != 0) {
    for (i = 0; i < nrow; i++)
      delete v[i];
    delete[] v;
  }
}

void FormulaMatrix::AddRows(int add, int length, Formula value) {
  if (add <= 0)
    return;

  int i;
  if (v == 0) {
    nrow = add;
    v = new FormulaVector*[nrow];
    for (i = 0; i < nrow; i++)
      v[i] = new FormulaVector(length, value);
  } else {
    FormulaVector** vnew = new FormulaVector*[add + nrow];
    for (i = 0; i < nrow; i++)
      vnew[i] = v[i];
    for (i = nrow; i < nrow + add; i++)
      vnew[i] = new FormulaVector(length, value);
    delete[] v;
    v = vnew;
    nrow += add;
  }
}

void FormulaMatrix::AddRows(int add, int length, double value) {
  if (add <= 0)
    return;

  int i;
  if (v == 0) {
    nrow = add;
    v = new FormulaVector*[nrow];
    for (i = 0; i < nrow; i++)
      v[i] = new FormulaVector(length, value);
  } else {
    FormulaVector** vnew = new FormulaVector*[add + nrow];
    for (i = 0; i < nrow; i++)
      vnew[i] = v[i];
    for (i = nrow; i < nrow + add; i++)
      vnew[i] = new FormulaVector(length, value);
    delete[] v;
    v = vnew;
    nrow += add;
  }
}

void FormulaMatrix::Inform(Keeper* keeper) {
  int i;
  for (i = 0; i < nrow; i++)
    v[i]->Inform(keeper);
}
