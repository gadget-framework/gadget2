#include "intvector.h"
#include "doublematrix.h"
#include "doubleindexvector.h"
#include "bandmatrix.h"
#include "mathfunc.h"
#include "gadget.h"

#ifndef GADGET_INLINE
#include "bandmatrix.icc"
#endif

BandMatrix::BandMatrix(const BandMatrix& initial)
  : nrow(initial.nrow), minage(initial.minAge()) {

  v = new DoubleIndexVector*[nrow];
  int i;
  for (i = 0; i < nrow; i++)
    v[i] = new DoubleIndexVector(*(initial.v[i]));
}

BandMatrix::BandMatrix(const IntVector& minl, const IntVector& size,
  int minAge, double initial) : nrow(size.Size()), minage(minAge) {

  assert(size.Size() == minl.Size());
  v = new DoubleIndexVector*[nrow];
  int i;
  for (i = 0; i < nrow; i++)
    v[i] = new DoubleIndexVector(size[i], minl[i], initial);
}

BandMatrix::BandMatrix(const DoubleMatrix& initial, int minAge, int minl)
  : nrow(initial.Nrow()), minage(minAge) {

  v = new DoubleIndexVector*[nrow];
  int i, j;
  int lower, upper;
  for (i = 0; i < nrow; i++) {
    lower = 0;
    upper = initial.Ncol(i) - 1;
    while (isZero(initial[i][lower]) && (lower < upper))
      lower++;
    while (isZero(initial[i][upper]) && (upper > lower))
      upper--;
    if (isZero(initial[i][lower]) && (lower == upper)) {
      lower = 0;
      upper = initial.Ncol(i) - 1;
    }
    v[i] = new DoubleIndexVector(upper - lower + 1, lower + minl);
    for (j = lower; j <= upper; j++)
      (*v[i])[j + minl] = initial[i][j];
  }
}

BandMatrix::BandMatrix(const DoubleIndexVector& initial, int age)
  : nrow(1), minage(age) {

  v = new DoubleIndexVector*[nrow];
  v[0] = new DoubleIndexVector(initial);
}

BandMatrix::BandMatrix(int minl, int lengthsize, int minAge, int Nrow,
  double initial) : nrow(Nrow), minage(minAge) {

  v = new DoubleIndexVector*[nrow];
  int i;
  for (i = 0; i < nrow; i++)
    v[i] = new DoubleIndexVector(lengthsize, minl, initial);
}

BandMatrix::~BandMatrix() {
  int i;
  if (v != 0) {
    for (i = 0; i < nrow; i++)
      delete v[i];
    delete[] v;
    v = 0;
  }
}

void BandMatrix::sumColumns(DoubleVector& Result) const {
  int i, j;
  for (i = 0; i < nrow; i++)
    for (j = v[i]->minCol(); j < v[i]->maxCol(); j++)
      Result[j] += (*v[i])[j];
}

void BandMatrix::Print(ofstream& outfile) const {
  int i, j;
  int maxcol = 0;

  for (i = minage; i < minage + nrow; i++)
    if (v[i - minage]->maxCol() > maxcol)
      maxcol = v[i - minage]->maxCol();

  for (i = minage; i < minage + nrow; i++) {
    outfile << TAB;
    if (v[i - minage]->minCol() > 0) {
      for (j = 0; j < v[i - minage]->minCol(); j++) {
        outfile.precision(smallprecision);
        outfile.width(smallwidth);
        outfile << 0.0 << sep;
      }
    }
    for (j = v[i- minage]->minCol(); j < v[i - minage]->maxCol(); j++) {
      outfile.precision(smallprecision);
      outfile.width(smallwidth);
      outfile << (*v[i - minage])[j] << sep;
    }
    if (v[i - minage]->maxCol() < maxcol) {
      for (j = v[i - minage]->maxCol(); j < maxcol; j++) {
        outfile.precision(smallprecision);
        outfile.width(smallwidth);
        outfile << 0.0 << sep;
      }
    }
    outfile << endl;
  }
}

BandMatrixVector::BandMatrixVector(int sz) : size(sz) {
  assert(size >= 0);
  int i;
  if (size == 0) {
    v = 0;
    return;
  }
  v = new BandMatrix*[size];
  for (i = 0; i < size; i++)
    v[i] = 0;
}

BandMatrixVector::~BandMatrixVector() {
  int i;
  if (v != 0) {
    for (i = 0; i < size; i++)
      delete v[i];
    delete[] v;
    v = 0;
  }
}

void BandMatrixVector::ChangeElement(int nr, const BandMatrix& value) {
  if (v[nr] != 0)
    delete v[nr];
  v[nr] = new BandMatrix(value);
}

void BandMatrixVector::resize(int addsize) {
  assert(addsize > 0);
  int i;
  if (v == 0) {
    size = addsize;
    v = new BandMatrix*[size];
    for (i = 0; i < size; i++)
      v[i] = 0;
  } else {
    BandMatrix** vnew = new BandMatrix*[addsize + size];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    for (i = size; i < size + addsize; i++)
      vnew[i] = 0;
    delete[] v;
    v = vnew;
    size += addsize;
  }
}

void BandMatrixVector::resize(int addsize, const BandMatrix& initial) {
  assert(addsize > 0);
  int i;
  if (v == 0) {
    size = addsize;
    v = new BandMatrix*[size];
    for (i = 0; i < size; i++)
      v[i] = new BandMatrix(initial);
  } else if (addsize > 0) {
    BandMatrix** vnew = new BandMatrix*[addsize + size];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    for (i = size; i < size + addsize; i++)
      vnew[i] = new BandMatrix(initial);
    delete[] v;
    v = vnew;
    size += addsize;
  }
}

void BandMatrixVector::Delete(int pos) {
  delete v[pos];
  int i;
  if (size > 1) {
    BandMatrix** vnew = new BandMatrix*[size - 1];
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

BandMatrixMatrix::BandMatrixMatrix(int NroW, int ncol) : nrow(NroW) {
  assert(nrow >= 0);
  assert(ncol >= 0);
  int i;
  if (nrow == 0) {
    v = 0;
    return;
  }
  v = new BandMatrixVector*[nrow];
  for (i = 0; i < nrow; i++)
    v[i] = new BandMatrixVector(ncol);
}

BandMatrixMatrix::~BandMatrixMatrix() {
  int i;
  if (v != 0) {
    for (i = 0; i < nrow; i++)
      delete v[i];
    delete[] v;
    v = 0;
  }
}

void BandMatrixMatrix::ChangeElement(int row, int col, const BandMatrix& value) {
  assert(0 <= row && row < nrow);
  v[row]->ChangeElement(col, value);
}

void BandMatrixMatrix::AddRows(int addrow, int ncol) {
  assert(addrow > 0);
  int i;
  if (v == 0) {
    nrow = addrow;
    v = new BandMatrixVector*[nrow];
    for (i = 0; i < nrow; i++)
      v[i] = new BandMatrixVector(ncol);
  } else {
    BandMatrixVector** vnew = new BandMatrixVector*[addrow + nrow];
    for (i = 0; i < nrow; i++)
      vnew[i] = v[i];
    for (i = nrow; i < nrow + addrow; i++)
      vnew[i] = new BandMatrixVector(ncol);
    delete[] v;
    v = vnew;
    nrow += addrow;
  }
}

void BandMatrixMatrix::DeleteRow(int row) {
  delete v[row];
  int i;
  if (nrow > 1) {
    BandMatrixVector** vnew = new BandMatrixVector*[nrow - 1];
    for (i = 0; i < row; i++)
      vnew[i] = v[i];
    for (i = row; i < nrow - 1; i++)
      vnew[i] = v[i + 1];
    delete[] v;
    v = vnew;
    nrow--;
  } else {
    delete[] v;
    v = 0;
    nrow = 0;
  }
}

BandMatrix& BandMatrix::operator += (BandMatrix& b) {
  int i, j;
  for (i = max(minRow(), b.minRow()); i < min(maxRow(), b.maxRow()); i++)
    for (j = max(minCol(i), b.minCol(i)); j < min(maxCol(i), b.maxCol(i)); j++)
      (*this)[i][j] += b[i][j];
  return *this;
}
