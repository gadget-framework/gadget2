#include "addr_keepmatrix.h"
#include "gadget.h"

#ifndef GADGET_INLINE
#include "addr_keepmatrix.icc"
#endif

//constructor for a rectangular addr_keepmatrix.
addr_keepmatrix::addr_keepmatrix(int nr, int nc) {
  nrow = nr;
  int i;
  v = new addr_keepvector*[nr];
  for (i = 0; i < nr; i++)
    v[i] = new addr_keepvector(nc);
}

//constructor for a rectangular addr_keepmatrix with initial value.
addr_keepmatrix::addr_keepmatrix(int nr, int nc, addr_keep value) {
  nrow = nr;
  v = new addr_keepvector*[nr];
  int i, j;
  for (i = 0; i < nr; i++)
    v[i] = new addr_keepvector(nc);
  for (i = 0; i < nr; i++) {
    for (j = 0; j < nc; j++)
      (*v[i])[j] = value;
  }
}

//constructor for a possibly nonrectangular addr_keepmatrix
//the rows need not have the same number of columns.
addr_keepmatrix::addr_keepmatrix(int nr, const intvector& nc) {
  nrow = nr;
  int i;
  v = new addr_keepvector*[nr];
  for (i = 0; i < nr; i++)
    v[i] = new addr_keepvector(nc[i]);
}

//constructor for addr_keepmatrix when all rows do not need to have same number of
//columns, with initial value.
addr_keepmatrix::addr_keepmatrix(int nr, const intvector& nc, addr_keep value) {
  nrow = nr;
  v = new addr_keepvector*[nr];
  int i, j;
  for (i = 0; i < nr; i++)
    v[i] = new addr_keepvector(nc[i]);
  for (i = 0; i < nr; i++)
    for (j = 0; j < nc[i]; j++)
      (*v[i])[j] = value;
}

addr_keepmatrix::addr_keepmatrix(const addr_keepmatrix& initial) : nrow(initial.nrow) {
  int i;
  if (nrow >= 0) {
    v = new addr_keepvector*[nrow];
    for (i = 0; i < nrow; i++)
      v[i] = new addr_keepvector(initial[i]);
  } else {
    v = 0;
    nrow = 0;
  }
}

addr_keepmatrix::~addr_keepmatrix() {
  int i;
  if (v != 0) {
    for (i = 0; i < nrow; i++)
      delete v[i];
    delete[] v;
    v = 0;
  }
}

//Adds rows to a addr_keepmatrix.
void addr_keepmatrix::AddRows(int add, int length) {
  addr_keepvector** vnew = new addr_keepvector* [nrow + add];
  int i;
  for (i = 0; i < nrow; i++)
    vnew[i] = v[i];
  delete[] v;
  v = vnew;
  for (i = nrow; i < nrow + add; i++)
    v[i] = new addr_keepvector(length);
  nrow += add;
}

//Adds rows to a addr_keepmatrix and initializes them.
void addr_keepmatrix::AddRows(int add, int length, addr_keep initial) {
  int oldnrow = nrow;
  int i, j;
  this->AddRows(add, length);
  for (i = oldnrow; i < nrow; i++)
    for (j = 0; j < length; j++)
      (*v[i])[j] = initial;
}

void addr_keepmatrix::DeleteRow(int row) {
  int i;
  assert(nrow > 0);
  assert(0 <= row && row < nrow);
  delete v[row];
  addr_keepvector** vnew = new addr_keepvector*[nrow - 1];
  for (i = 0; i < row; i++)
    vnew[i] = v[i];
  for (i = row; i < nrow - 1; i++)
    vnew[i] = v[i + 1];
  delete[] v;
  v = vnew;
  nrow--;
}
