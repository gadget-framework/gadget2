#include "popratiomatrix.h"
#include "gadget.h"

#ifndef GADGET_INLINE
#include "popratiomatrix.icc"
#endif

//constructor for a rectangular popinfomatrix.
popratiomatrix::popratiomatrix(int nr, int nc) {
  nrow = nr;
  v = new popratiovector*[nr];
  int i;
  for (i = 0; i < nr; i++)
    v[i] = new popratiovector(nc);
}

//constructor for a rectangular popinfomatrix with initial value.
popratiomatrix::popratiomatrix(int nr, int nc, popratio value) {
  nrow = nr;
  v = new popratiovector*[nr];
  int i, j;
  for (i = 0; i < nr; i++)
    v[i] = new popratiovector(nc);
  for (i = 0; i < nr; i++)
    for (j = 0; j < nc; j++)
      (*v[i])[j] = value;
}

//constructor for a possibly nonrectangular popinfomatrix
//the rows need not have the same number of columns.
popratiomatrix::popratiomatrix(int nr, const intvector& nc) {
  nrow = nr;
  v = new popratiovector*[nr];
  int i;
  for (i = 0; i < nr; i++)
    v[i] = new popratiovector(nc[i]);
}

//constructor for popinfomatrix when all rows do not need to have same number of
//columns, with initial value.
popratiomatrix::popratiomatrix(int nr, const intvector& nc, popratio value) {
  nrow = nr;
  v = new popratiovector*[nr];
  int i, j;
  for (i = 0; i < nr; i++)
    v[i] = new popratiovector(nc[i]);
  for (i = 0; i < nr; i++)
    for (j = 0; j < nc[i]; j++)
      (*v[i])[j] = value;
}

popratiomatrix::popratiomatrix(const popratiomatrix& initial) : nrow(initial.nrow) {
  int i;
  if (nrow >= 0) {
    v = new popratiovector*[nrow];
    for (i = 0; i < nrow; i++)
      v[i] = new popratiovector(initial[i]);
  } else {
    v = 0;
    nrow = 0;
  }
}

popratiomatrix::~popratiomatrix() {
  int i;
  if (v != 0) {
    for (i = 0; i < nrow; i++)
      delete v[i];
    delete[] v;
    v = 0;
  }
}

//Adds rows to a popinfomatrix.
void popratiomatrix::AddRows(int add, int length) {
  popratiovector** vnew = new popratiovector*[nrow + add];
  int i;
  for (i = 0; i < nrow; i++)
    vnew[i] = v[i];
  delete[] v;
  v = vnew;
  for (i = nrow; i < nrow + add; i++)
    v[i] = new popratiovector(length);
  nrow += add;
}

//Adds rows to a popinfomatrix and initializes them.
void popratiomatrix::AddRows(int add, int length, popratio initial) {
  int oldnrow = nrow;
  this->AddRows(add, length);
  int i, j;
  for (i = oldnrow; i < nrow; i++)
    for (j = 0; j < length; j++)
      (*v[i])[j] = initial;
}

void popratiomatrix::DeleteRow(int row) {
  assert(nrow > 0);
  assert(0 <= row && row < nrow);
  delete v[row];
  popratiovector** vnew = new popratiovector*[nrow - 1];
  int i;
  for (i = 0; i < row; i++)
    vnew[i] = v[i];
  for (i = row; i < nrow - 1; i++)
    vnew[i] = v[i + 1];
  delete[] v;
  v = vnew;
  nrow--;
}
