#include "doublematrixptrmatrix.h"
#include "gadget.h"

#ifndef INLINE_VECTORS
#include "doublematrixptrmatrix.icc"
#endif

//constructor for a rectangular doublematrixptrmatrix.
doublematrixptrmatrix::doublematrixptrmatrix(int nr, int nc) {
  nrow = nr;
  v = new doublematrixptrvector*[nr];
  int i;
  for (i = 0; i < nr; i++)
    v[i] = new doublematrixptrvector(nc);
}

//constructor for a rectangular doublematrixptrmatrix with initial value.
doublematrixptrmatrix::doublematrixptrmatrix(int nr, int nc, doublematrix* value) {
  nrow = nr;
  v = new doublematrixptrvector*[nr];
  int i, j;
  for (i = 0; i < nr; i++)
    v[i] = new doublematrixptrvector(nc);
  for (i = 0; i < nr; i++) {
    for (j = 0; j < nc; j++)
      (*v[i])[j] = value;
  }
}

//constructor for a possibly nonrectangular doublematrixptrmatrix
//the rows need not have the same number of columns.
doublematrixptrmatrix::doublematrixptrmatrix(int nr, const intvector& nc) {
  nrow = nr;
  v = new doublematrixptrvector*[nr];
  int i;
  for (i = 0; i < nr; i++)
    v[i] = new doublematrixptrvector(nc[i]);
}

//constructor for doublematrixptrmatrix when all rows do not need
//to have same number of columns, with initial value.
doublematrixptrmatrix::doublematrixptrmatrix(int nr, const intvector& nc, doublematrix* value) {
  nrow = nr;
  v = new doublematrixptrvector*[nr];
  int i, j;
  for (i = 0; i < nr; i++)
    v[i] = new doublematrixptrvector(nc[i]);
  for (i = 0; i < nr; i++)
    for (j = 0; j < nc[i]; j++)
      (*v[i])[j] = value;
}

doublematrixptrmatrix::doublematrixptrmatrix(const doublematrixptrmatrix& initial) : nrow(initial.nrow) {
  int i;
  if (nrow >= 0) {
    v = new doublematrixptrvector*[nrow];
    for (i = 0; i < nrow; i++)
      v[i] = new doublematrixptrvector(initial[i]);
  } else {
    v = 0;
    nrow = 0;
  }
}

doublematrixptrmatrix::~doublematrixptrmatrix() {
  int i;
  if (v != 0) {
    for (i = 0; i < nrow; i++)
      delete v[i];
    delete[] v;
  }
}

//Adds rows to a doublematrixptrmatrix.
void doublematrixptrmatrix::AddRows(int add, int length) {
  doublematrixptrvector** vnew = new doublematrixptrvector*[nrow + add];
  int i;
  for (i = 0; i < nrow; i++)
    vnew[i] = v[i];
  delete[] v;
  v = vnew;
  for (i = nrow; i < nrow + add; i++)
    v[i] = new doublematrixptrvector(length);
  nrow += add;
}

//Adds rows to a doublematrixptrmatrix and initializes them.
void doublematrixptrmatrix::AddRows(int add, int length, doublematrix* initial) {
  int oldnrow = nrow;
  this->AddRows(add, length);
  int i, j;
  for (i = oldnrow; i < nrow; i++)
    for (j = 0; j < length; j++)
      (*v[i])[j] = initial;
}

void doublematrixptrmatrix::DeleteRow(int row) {
  assert(nrow > 0);
  assert(0 <= row && row < nrow);
  delete v[row];
  doublematrixptrvector** vnew = new doublematrixptrvector*[nrow - 1];
  int i;
  for (i = 0; i < row; i++)
    vnew[i] = v[i];
  for (i = row; i < nrow - 1; i++)
    vnew[i] = v[i + 1];
  delete[] v;
  v = vnew;
  nrow--;
}
