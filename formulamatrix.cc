#include "formulamatrix.h"
#include "gadget.h"

#ifndef GADGET_INLINE
#include "formulamatrix.icc"
#endif

FormulaMatrix::FormulaMatrix(int nr, int nc) {
  assert(nr > 0);
  nrow = nr;
  v = new FormulaVector*[nr];
  int i;
  for (i = 0; i < nr; i++)
    v[i] = new FormulaVector(nc);
}

FormulaMatrix::FormulaMatrix(int nr, int nc, Formula value) {
  nrow = nr;
  v = new FormulaVector*[nr];
  int i, j;
  for (i = 0; i < nr; i++)
    v[i] = new FormulaVector(nc);
  for (i = 0; i < nr; i++) {
    for (j = 0; j < nc; j++)
      (*v[i])[j] = value;
  }
}

FormulaMatrix::FormulaMatrix(int nr, const IntVector& nc) {
  assert(nr > 0);
  nrow = nr;
  v = new FormulaVector*[nr];
  int i;
  for (i = 0; i < nr; i++)
    v[i] = new FormulaVector(nc[i]);
}

FormulaMatrix::~FormulaMatrix() {
  int i;
  if (v != 0) {
    for (i = 0; i < nrow; i++)
      delete v[i];
    delete[] v;
  }
}

FormulaMatrix& FormulaMatrix::operator=(const FormulaMatrix& formulaM) {
  if (this == &formulaM) {
    //same object just return
    return *this;
  }
  int i;
  if (v != 0) {
    //this has some vectors to return memory
    for (i = 0; i < nrow; i++)
      delete v[i];
    delete[] v;
    v = 0;
  }
  nrow = formulaM.nrow;
  if (nrow >= 0) {
    v = new FormulaVector*[nrow];
    for (i = 0; i < nrow; i++)
      v[i] = new FormulaVector(formulaM[i]);
  } else {
    v = 0;
    nrow = 0;
  }
  return *this;
}

void FormulaMatrix::AddRows(int add, int length) {
  assert(nrow >= 0 && add > 0);
  FormulaVector** vnew = new FormulaVector*[nrow + add];
  int i;
  for (i = 0; i < nrow; i++)
    vnew[i] = v[i];
  delete[] v;
  v = vnew;
  for (i = nrow; i < nrow + add; i++)
    v[i] = new FormulaVector(length);
  nrow += add;
}

void FormulaMatrix::AddRows(int add, int length, Formula formula) {
  if (v == 0)
    nrow = 0;
  int oldnrow = nrow;
  //Add vectors to matrix
  this->AddRows(add, length);
  int i, j;
  //Initiate added vectors with formula
  for (i = oldnrow; i < nrow; i++)
    for (j = 0; j < length; j++)
      (*v[i])[j] = formula;
}

void FormulaMatrix::Inform(Keeper* keeper) {
  int i;
  for (i = 0; i < nrow; i++) {
    ostringstream ostr;
    ostr << i + 1 << ends;
    keeper->AddString(ostr.str());
    v[i]->Inform(keeper);
    keeper->ClearLast();
  }
}

CommentStream& operator >> (CommentStream& infile, FormulaMatrix& Fmatrix) {
  if (infile.fail()) {
    infile.makebad();
    return infile;
  }
  int i;
  for (i = 0; i < Fmatrix.Nrow(); i++) {
    if (!(infile >> Fmatrix[i])) {
      infile.makebad();
      return infile;
    }
  }
  return infile;
}
