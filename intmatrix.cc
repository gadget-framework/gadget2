#include "intmatrix.h"
#include "gadget.h"

#ifndef GADGET_INLINE
#include "intmatrix.icc"
#endif

//constructor for a rectangular intmatrix.
intmatrix::intmatrix(int nr, int nc) {
  nrow = nr;
  v = new intvector*[nr];
  int i;
  for (i = 0; i < nr; i++)
    v[i] = new intvector(nc);
}

//constructor for a rectangular intmatrix with initial value.
intmatrix::intmatrix(int nr, int nc, int value) {
  nrow = nr;
  v = new intvector*[nr];
  int i, j;
  for (i = 0; i < nr; i++)
    v[i] = new intvector(nc);
  for (i = 0; i < nr; i++)
    for (j = 0; j < nc; j++)
      (*v[i])[j] = value;
}

//constructor for a possibly nonrectangular intmatrix
//the rows need not have the same number of columns.
intmatrix::intmatrix(int nr, const intvector& nc) {
  nrow = nr;
  v = new intvector*[nr];
  int i;
  for (i = 0; i < nr; i++)
    v[i] = new intvector(nc[i]);
}

//constructor for intmatrix when all rows do not need to have same number of
//columns, with initial value.
intmatrix::intmatrix(int nr, const intvector& nc, int value) {
  nrow = nr;
  v = new intvector*[nr];
  int i, j;
  for (i = 0; i < nr; i++)
    v[i] = new intvector(nc[i]);
  for (i = 0; i < nr; i++)
    for (j = 0; j < nc[i]; j++)
      (*v[i])[j] = value;
}

intmatrix::intmatrix(const intmatrix& initial) : nrow(initial.nrow) {
  int i;
  if (nrow >= 0) {
    v = new intvector*[nrow];
    for (i = 0; i < nrow; i++)
      v[i] = new intvector(initial[i]);
  } else {
    v = 0;
    nrow = 0;
  }
}

intmatrix::~intmatrix() {
  int i;
  if (v != 0) {
    for (i = 0; i < nrow; i++)
      delete v[i];
    delete[] v;
    v = 0;
  }
}

//Adds rows to a intmatrix.
void intmatrix::AddRows(int add, int length) {
  intvector** vnew = new intvector*[nrow + add];
  int i;
  for (i = 0; i < nrow; i++)
    vnew[i] = v[i];
  delete[] v;
  v = vnew;
  for (i = nrow; i < nrow + add; i++)
    v[i] = new intvector(length);
  nrow += add;
}

//Adds rows to a intmatrix and initializes them.
void intmatrix::AddRows(int add, int length, int initial) {
  int oldnrow = nrow;
  this->AddRows(add, length);
  int i, j;
  for (i = oldnrow; i < nrow; i++)
    for (j = 0; j < length; j++)
      (*v[i])[j] = initial;
}

void intmatrix::DeleteRow(int row) {
  int i;
  assert(nrow > 0);
  assert(0 <= row && row < nrow);
  delete v[row];
  intvector** vnew = new intvector*[nrow -1];
  for (i = 0; i < row; i++)
    vnew[i] = v[i];
  for (i = row; i < nrow - 1; i++)
    vnew[i] = v[i + 1];
  delete[] v;
  v = vnew;
  nrow--;
}
