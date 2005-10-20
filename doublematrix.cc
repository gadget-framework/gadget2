#include "doublematrix.h"
#include "mathfunc.h"
#include "gadget.h"

DoubleMatrix::DoubleMatrix(int nr, int nc) {
  nrow = (nr > 0 ? nr : 0);
  int i;
  if (nrow > 0) {
    v = new DoubleVector*[nrow];
    for (i = 0; i < nrow; i++)
      v[i] = new DoubleVector(nc);
  } else
    v = 0;
}

DoubleMatrix::DoubleMatrix(int nr, int nc, double value) {
  nrow = nr;
  v = new DoubleVector*[nr];
  int i, j;
  for (i = 0; i < nr; i++)
    v[i] = new DoubleVector(nc);
  for (i = 0; i < nr; i++) {
    for (j = 0; j < nc; j++)
      (*v[i])[j] = value;
  }
}

DoubleMatrix::DoubleMatrix(int nr, const IntVector& nc) {
  nrow = nr;
  v = new DoubleVector*[nr];
  int i;
  for (i = 0; i < nr; i++)
    v[i] = new DoubleVector(nc[i]);
}

DoubleMatrix::DoubleMatrix(int nr, const IntVector& nc, double value) {
  nrow = nr;
  v = new DoubleVector*[nr];
  int i, j;
  for (i = 0; i < nr; i++)
    v[i] = new DoubleVector(nc[i]);
  for (i = 0; i < nr; i++)
    for (j = 0; j < nc[i]; j++)
      (*v[i])[j] = value;
}

DoubleMatrix::DoubleMatrix(const DoubleMatrix& initial) : nrow(initial.nrow) {
  int i;
  if (nrow >= 0) {
    v = new DoubleVector*[nrow];
    for (i = 0; i < nrow; i++)
      v[i] = new DoubleVector(initial[i]);
  } else {
    v = 0;
    nrow = 0;
  }
}

DoubleMatrix::~DoubleMatrix() {
  int i;
  if (v != 0) {
    for (i = 0; i < nrow; i++)
      delete v[i];
    delete[] v;
    v = 0;
  }
}

void DoubleMatrix::AddRows(int add, int length) {
  if (v == 0)
    nrow = 0;
  DoubleVector** vnew = new DoubleVector*[nrow + add];
  int i;
  for (i = 0; i < nrow; i++)
    vnew[i] = v[i];
  delete[] v;
  v = vnew;
  for (i = nrow; i < nrow + add; i++)
    v[i] = new DoubleVector(length);
  nrow += add;
}

void DoubleMatrix::AddRows(int add, int length, double initial) {
  if (v == 0)
    nrow = 0;
  int oldnrow = nrow;
  this->AddRows(add, length);
  int i, j;
  for (i = oldnrow; i < nrow; i++)
    for (j = 0; j < length; j++)
      (*v[i])[j] = initial;
}

void DoubleMatrix::DeleteRow(int row) {
  delete v[row];
  int i;
  if (nrow > 1) {
    DoubleVector** vnew = new DoubleVector*[nrow - 1];
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

DoubleMatrix& DoubleMatrix::operator = (const DoubleMatrix& d) {
  int i;
  if (v != 0) {
    for (i = 0; i < nrow; i++)
      delete v[i];
    delete[] v;
  }

  nrow = d.nrow;
  if (nrow > 0) {
    v = new DoubleVector*[nrow];
    for (i = 0; i < nrow; i++)
      v[i] = new DoubleVector(d[i]);

  } else
    v = 0;

  return *this;
}

void DoubleMatrix::Reset() {
  if (nrow > 0) {
    int i;
    for (i = 0; i < nrow; i++)
      delete v[i];
    delete[] v;
    v = 0;
    nrow = 0;
  }
}

void DoubleMatrix::Print(ofstream& outfile) const {
  int i, j;
  for (i = 0; i < nrow; i++) {
    outfile << TAB;
    for (j = 0; j < v[i]->Size(); j++)
      outfile << setw(smallwidth) << setprecision(smallprecision) << (*v[i])[j] << sep;
    outfile << endl;
  }
}
