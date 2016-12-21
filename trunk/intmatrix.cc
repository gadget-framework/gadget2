#include "intmatrix.h"
#include "gadget.h"

IntMatrix::IntMatrix(int nr, int nc, int value) {
  nrow = nr;
  v = new IntVector*[nr];
  int i, j;
  for (i = 0; i < nr; i++)
    v[i] = new IntVector(nc, value);
}

IntMatrix::IntMatrix(const IntMatrix& initial) : nrow(initial.nrow) {
  int i;
  if (nrow >= 0) {
    v = new IntVector*[nrow];
    for (i = 0; i < nrow; i++)
      v[i] = new IntVector(initial[i]);
  } else {
    v = 0;
    nrow = 0;
  }
}

IntMatrix::~IntMatrix() {
  int i;
  if (v != 0) {
    for (i = 0; i < nrow; i++)
      delete v[i];
    delete[] v;
    v = 0;
  }
}

void IntMatrix::AddRows(int add, int length, int value) {
  if (add <= 0)
    return;

  int i;
  if (v == 0) {
   nrow = add;
   v = new IntVector*[nrow];
   for (i = 0; i < nrow; i++)
     v[i] = new IntVector(length, value);

  } else {
    IntVector** vnew = new IntVector*[nrow + add];
    for (i = 0; i < nrow; i++)
      vnew[i] = v[i];
    for (i = nrow; i < nrow + add; i++)
      vnew[i] = new IntVector(length, value);
    delete[] v;
    v = vnew;
    nrow += add;
  }
}

void IntMatrix::Delete(int pos) {
  delete v[pos];
  int i;
  if (nrow > 1) {
    IntVector** vnew = new IntVector*[nrow -1];
    for (i = 0; i < pos; i++)
      vnew[i] = v[i];
    for (i = pos; i < nrow - 1; i++)
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

IntMatrix& IntMatrix::operator = (const IntMatrix& initial) {
  int i;
  if (v != 0) {
    for (i = 0; i < nrow; i++)
      delete v[i];
    delete[] v;
  }

  nrow = initial.nrow;
  if (nrow > 0) {
    v = new IntVector*[nrow];
    for (i = 0; i < nrow; i++)
      v[i] = new IntVector(initial[i]);

  } else
    v = 0;

  return *this;
}

void IntMatrix::Reset() {
  if (nrow > 0) {
    int i;
    for (i = 0; i < nrow; i++)
      delete v[i];
    delete[] v;
    v = 0;
    nrow = 0;
  }
}

void IntMatrix::Print(ofstream& outfile) const {
  int i, j;
  for (i = 0; i < nrow; i++) {
    outfile << TAB;
    for (j = 0; j < v[i]->Size(); j++)
      outfile << setw(smallwidth) << (*v[i])[j] << sep;
    outfile << endl;
  }
}

void IntMatrix::setToZero() {
  int i;
  for (i = 0; i < nrow; i++)
    (*v[i]).setToZero();
}
