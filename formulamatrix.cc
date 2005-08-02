#include "formulamatrix.h"
#include "gadget.h"

FormulaMatrix::FormulaMatrix(int nr, int nc) {
  nrow = (nr > 0 ? nr : 0);
  int i;
  if (nrow > 0) {
    v = new FormulaVector*[nrow];
    for (i = 0; i < nrow; i++)
      v[i] = new FormulaVector(nc);
  } else
    v = 0;
}

FormulaMatrix::FormulaMatrix(int nr, int nc, Formula initial) {
  nrow = nr;
  v = new FormulaVector*[nr];
  int i, j;
  for (i = 0; i < nr; i++)
    v[i] = new FormulaVector(nc);
  for (i = 0; i < nr; i++) {
    for (j = 0; j < nc; j++)
      (*v[i])[j] = initial;
  }
}

FormulaMatrix::FormulaMatrix(int nr, int nc, double initial) {
  nrow = nr;
  v = new FormulaVector*[nr];
  int i, j;
  for (i = 0; i < nr; i++)
    v[i] = new FormulaVector(nc);
  for (i = 0; i < nr; i++) {
    for (j = 0; j < nc; j++)
      (*v[i])[j].setValue(initial);
  }
}

FormulaMatrix::FormulaMatrix(int nr, const IntVector& nc) {
  nrow = (nr > 0 ? nr : 0);
  int i;
  if (nrow > 0) {
    v = new FormulaVector*[nrow];
    for (i = 0; i < nrow; i++)
      v[i] = new FormulaVector(nc[i]);
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

void FormulaMatrix::AddRows(int add, int length) {
  int i;
  if (add <= 0)
    return;
  if (v == 0) {
    nrow = add;
    v = new FormulaVector*[nrow];
    for (i = 0; i < nrow; i++)
      v[i] = new FormulaVector(length);
  } else {
    FormulaVector** vnew = new FormulaVector*[add + nrow];
    for (i = 0; i < nrow; i++)
      vnew[i] = v[i];
    for (i = nrow; i < nrow + add; i++)
      vnew[i] = new FormulaVector(length);
    delete[] v;
    v = vnew;
    nrow += add;
  }
}

void FormulaMatrix::AddRows(int add, int length, Formula initial) {
  if (v == 0)
    nrow = 0;
  int oldnrow = nrow;
  //Add vectors to matrix
  this->AddRows(add, length);
  int i, j;
  //Initiate added vectors with formula
  for (i = oldnrow; i < nrow; i++)
    for (j = 0; j < length; j++)
      (*v[i])[j] = initial;
}

void FormulaMatrix::AddRows(int add, int length, double initial) {
  if (v == 0)
    nrow = 0;
  int oldnrow = nrow;
  //Add vectors to matrix
  this->AddRows(add, length);
  int i, j;
  //Initiate added vectors with formula
  for (i = oldnrow; i < nrow; i++)
    for (j = 0; j < length; j++)
      (*v[i])[j].setValue(initial);
}

void FormulaMatrix::Inform(Keeper* keeper) {
  int i;
  for (i = 0; i < nrow; i++)
    v[i]->Inform(keeper);
}
