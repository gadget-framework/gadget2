#include "intvector.h"
#include "doublematrix.h"
#include "doubleindexvector.h"
#include "bandmatrix.h"
#include "mathfunc.h"
#include "gadget.h"

#ifndef INLINE_VECTORS
#include "bandmatrix.icc"
#endif

bandmatrix::bandmatrix(const bandmatrix& initial)
  : nrow(initial.nrow), minage(initial.Minage()) {

  v = new doubleindexvector*[nrow];
  int i;
  for (i = 0; i < nrow; i++)
    v[i] = new doubleindexvector(*(initial.v[i]));
}

bandmatrix::bandmatrix(const intvector& minl, const intvector& size,
  int MinAge, double initial) : nrow(size.Size()), minage(MinAge) {

  assert(size.Size() == minl.Size());
  v = new doubleindexvector*[nrow];
  int i;
  for (i = 0; i < nrow; i++)
    v[i] = new doubleindexvector(size[i], minl[i], initial);
}

bandmatrix::bandmatrix(const doublematrix& initial, int MinAge, int minl)
  : nrow(initial.Nrow()), minage(MinAge) {

  v = new doubleindexvector*[nrow];
  int i, j;
  int lower, upper;
  for (i = 0; i < nrow; i++) {
    lower = 0;
    upper = initial.Ncol(i) - 1;
    while (iszero(initial[i][lower]) && (lower < upper))
      lower++;
    while (iszero(initial[i][upper]) && (upper > lower))
      upper--;
    if (iszero(initial[i][lower]) && (lower == upper)) {
      lower = 0;
      upper = initial.Ncol(i) - 1;
    }
    v[i] = new doubleindexvector(upper - lower + 1, lower + minl);
    for (j = lower; j <= upper; j++)
      (*v[i])[j + minl] = initial[i][j];
  }
}

bandmatrix::bandmatrix(const doubleindexvector& initial, int age)
  : nrow(1), minage(age) {

  v = new doubleindexvector*[nrow];
  v[0] = new doubleindexvector(initial);
}

bandmatrix::bandmatrix(int minl, int lengthsize, int Minage, int Nrow,
  double initial) : nrow(Nrow), minage(Minage) {

  v = new doubleindexvector*[nrow];
  int i;
  for (i = 0; i < nrow; i++)
    v[i] = new doubleindexvector(lengthsize, minl, initial);
}

bandmatrix::~bandmatrix() {
  int i;
  if (v != 0) {
    for (i = 0; i < nrow; i++)
      delete v[i];
    delete[] v;
  }
}

void bandmatrix::Print() const {
  int i, j;
  for (i = 0; i < nrow; i++) {
    for (j = v[i]->Mincol(); j < v[i]->Maxcol(); j++)
      cout << (*v[i])[j] << sep;
    cout << endl;
  }
  cout << flush;
}

void bandmatrix::Colsum(doublevector& Result) const {
  int i, j;
  for (i = 0; i < nrow; i++)
    for (j = v[i]->Mincol(); j < v[i]->Maxcol(); j++)
      Result[j] += (*v[i])[j];
}

bandmatrixvector::bandmatrixvector(int sz) : size(sz) {
  assert(size >= 0);
  int i;
  if (size == 0) {
    v = 0;
    return;
  }
  v = new bandmatrix*[size];
  for (i = 0; i < size; i++)
    v[i] = 0;
}

bandmatrixvector::~bandmatrixvector() {
  int i;
  if (v != 0) {
    for (i = 0; i < size; i++)
      delete v[i];
    delete[] v;
  }
}

void bandmatrixvector::ChangeElement(int nr, const bandmatrix& value) {
  if (v[nr] != 0)
    delete v[nr];
  v[nr] = new bandmatrix(value);
}

void bandmatrixvector::resize(int addsize) {
  assert(addsize > 0);
  int i;
  if (v == 0) {
    size = addsize;
    v = new bandmatrix*[size];
    for (i = 0; i < size; i++)
      v[i] = 0;
  } else {
    bandmatrix** vnew = new bandmatrix*[addsize + size];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    for (i = size; i < size + addsize; i++)
      vnew[i] = 0;
    delete[] v;
    v = vnew;
    size += addsize;
  }
}

void bandmatrixvector::resize(int addsize, const bandmatrix& initial) {
  assert(addsize > 0);
  int i;
  if (v == 0) {
    size = addsize;
    v = new bandmatrix*[size];
    for (i = 0; i < size; i++)
      v[i] = new bandmatrix(initial);
  } else if (addsize > 0) {
    bandmatrix** vnew = new bandmatrix*[addsize + size];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    for (i = size; i < size + addsize; i++)
      vnew[i] = new bandmatrix(initial);
    delete[] v;
    v = vnew;
    size += addsize;
  }
}

void bandmatrixvector::Delete(int pos) {
  assert(size > 0);
  assert(0 <= pos && pos < size);
  delete v[pos];
  bandmatrix** vnew = new bandmatrix*[size - 1];
  int i;
  for (i = 0; i < pos; i++)
    vnew[i] = v[i];
  for (i = pos; i < size - 1; i++)
    vnew[i] = v[i + 1];
  delete[] v;
  v = vnew;
  size--;
}

bandmatrixmatrix::bandmatrixmatrix(int NroW, int ncol) : nrow(NroW) {
  assert(nrow >= 0);
  assert(ncol >= 0);
  int i;
  if (nrow == 0) {
    v = 0;
    return;
  }
  v = new bandmatrixvector*[nrow];
  for (i = 0; i < nrow; i++)
    v[i] = new bandmatrixvector(ncol);
}

bandmatrixmatrix::~bandmatrixmatrix() {
  int i;
  if (v != 0) {
    for (i = 0; i < nrow; i++)
      delete v[i];
    delete[] v;
  }
}

void bandmatrixmatrix::ChangeElement(int row, int col, const bandmatrix& value) {
  assert(0 <= row && row < nrow);
  v[row]->ChangeElement(col, value);
}

void bandmatrixmatrix::AddRows(int addrow, int ncol) {
  assert(addrow > 0);
  int i;
  if (v == 0) {
    nrow = addrow;
    v = new bandmatrixvector*[nrow];
    for (i = 0; i < nrow; i++)
      v[i] = new bandmatrixvector(ncol);
  } else {
    bandmatrixvector** vnew = new bandmatrixvector*[addrow + nrow];
    for (i = 0; i < nrow; i++)
      vnew[i] = v[i];
    for (i = nrow; i < nrow + addrow; i++)
      vnew[i] = new bandmatrixvector(ncol);
    delete[] v;
    v = vnew;
    nrow += addrow;
  }
}

void bandmatrixmatrix::DeleteRow(int row) {
  assert(nrow > 0);
  assert(0 <= row && row < nrow);
  delete v[row];
  bandmatrixvector** vnew = new bandmatrixvector*[nrow - 1];
  int i;
  for (i = 0; i < row; i++)
    vnew[i] = v[i];
  for (i = row; i < nrow - 1; i++)
    vnew[i] = v[i + 1];
  delete[] v;
  v = vnew;
  nrow--;
}

bandmatrix& bandmatrix::operator += (bandmatrix& b) {
  int i, j;
  for (i = max(Minrow(), b.Minrow()); i<min(Maxrow(), b.Maxrow()); i++)
    for (j = max(Mincol(i), b.Mincol(i)); j<min(Maxcol(i), b.Maxcol(i)); j++)
      (*this)[i][j] += b[i][j];
  return *this;
}
