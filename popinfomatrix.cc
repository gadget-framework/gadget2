#include "popinfomatrix.h"
#include "gadget.h"

#ifndef GADGET_INLINE
#include "popinfomatrix.icc"
#endif

//constructor for a rectangular popinfomatrix.
popinfomatrix::popinfomatrix(int nr, int nc) {
  nrow = nr;
  v = new popinfovector*[nr];
  int i;
  for (i = 0; i < nr; i++)
    v[i] = new popinfovector(nc);
}

//constructor for a rectangular popinfomatrix with initial value.
popinfomatrix::popinfomatrix(int nr, int nc, popinfo value) {
  nrow = nr;
  v = new popinfovector*[nr];
  int i, j;
  for (i = 0; i < nr; i++)
    v[i] = new popinfovector(nc);
  for (i = 0; i < nr; i++) {
    for (j = 0; j < nc; j++)
      (*v[i])[j] = value;
  }
}

//constructor for a possibly nonrectangular popinfomatrix
//the rows need not have the same number of columns.
popinfomatrix::popinfomatrix(int nr, const intvector& nc) {
  nrow = nr;
  v = new popinfovector*[nr];
  int i;
  for (i = 0; i < nr; i++)
    v[i] = new popinfovector(nc[i]);
}

//constructor for popinfomatrix when all rows do not need to have same number of
//columns, with initial value.
popinfomatrix::popinfomatrix(int nr, const intvector& nc, popinfo value) {
  nrow = nr;
  v = new popinfovector*[nr];
  int i, j;
  for (i = 0; i < nr; i++)
    v[i] = new popinfovector(nc[i]);
  for (i = 0; i < nr; i++)
    for (j = 0; j < nc[i]; j++)
      (*v[i])[j] = value;
}

popinfomatrix::popinfomatrix(const popinfomatrix& initial) : nrow(initial.nrow) {
  int i;
  if (nrow >= 0) {
    v = new popinfovector*[nrow];
    for (i = 0; i < nrow; i++)
      v[i] = new popinfovector(initial[i]);
  } else {
    v = 0;
    nrow = 0;
  }
}

popinfomatrix::~popinfomatrix() {
  int i;
  if (v != 0) {
    for (i = 0; i < nrow; i++)
      delete v[i];
    delete[] v;
    v = 0;
  }
}

//Adds rows to a popinfomatrix.
void popinfomatrix::AddRows(int add, int length) {
  popinfovector** vnew = new popinfovector*[nrow + add];
  int i;
  for (i = 0; i < nrow; i++)
    vnew[i] = v[i];
  delete[] v;
  v = vnew;
  for (i = nrow; i < nrow + add; i++)
    v[i] = new popinfovector(length);
  nrow += add;
}

//Adds rows to a popinfomatrix and initializes them.
void popinfomatrix::AddRows(int add, int length, popinfo initial) {
  int oldnrow = nrow;
  this->AddRows(add, length);
  int i, j;
  for (i = oldnrow; i < nrow; i++)
    for (j = 0; j < length; j++)
      (*v[i])[j] = initial;
}

void popinfomatrix::DeleteRow(int row) {
  int i;
  assert(nrow > 0);
  assert(0 <= row && row < nrow);
  delete v[row];
  popinfovector** vnew = new popinfovector*[nrow - 1];
  for (i = 0; i < row; i++)
    vnew[i] = v[i];
  for (i = row; i < nrow - 1; i++)
    vnew[i] = v[i + 1];
  delete[] v;
  v = vnew;
  nrow--;
}
