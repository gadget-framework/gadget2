#include "charptrmatrix.h"
#include "gadget.h"

#ifndef INLINE_VECTORS
#include "charptrmatrix.icc"
#endif

//constructor for a rectangular charptrmatrix.
charptrmatrix::charptrmatrix(int nr, int nc) {
  nrow = nr;
  v = new charptrvector*[nr];
  int i;
  for (i = 0; i < nr; i++)
    v[i] = new charptrvector(nc);
}

//constructor for a rectangular charptrmatrix with initial value.
charptrmatrix::charptrmatrix(int nr, int nc, char* value) {
  nrow = nr;
  v = new charptrvector*[nr];
  int i, j;
  for (i = 0; i < nr; i++)
    v[i] = new charptrvector(nc);
  for (i = 0; i < nr; i++)
    for (j = 0; j < nc; j++)
      (*v[i])[j] = value;
}

//constructor for a possibly nonrectangular charptrmatrix
//the rows need not have the same number of columns.
charptrmatrix::charptrmatrix(int nr, const intvector& nc) {
  nrow = nr;
  v = new charptrvector*[nr];
  int i;
  for (i = 0; i < nr; i++)
    v[i] = new charptrvector(nc[i]);
}

//constructor for charptrmatrix when all rows do not need to have same number of
//columns, with initial value.
charptrmatrix::charptrmatrix(int nr, const intvector& nc, char* value) {
  nrow = nr;
  v = new charptrvector*[nr];
  int i, j;
  for (i = 0; i < nr; i++)
    v[i] = new charptrvector(nc[i]);
  for (i = 0; i < nr; i++)
    for (j = 0; j < nc[i]; j++)
      (*v[i])[j] = value;
}

charptrmatrix::charptrmatrix(const charptrmatrix& initial) : nrow(initial.nrow) {
  int i;
  if (nrow >= 0) {
    v = new charptrvector*[nrow];
    for (i = 0; i < nrow; i++)
      v[i] = new charptrvector(initial[i]);
  } else {
    v = 0;
    nrow = 0;
  }
}

charptrmatrix::~charptrmatrix() {
  int i;
  if (v != 0) {
    for (i = 0; i < nrow; i++)
      delete v[i];
    delete[] v;
  }
}

//Adds rows to a charptrmatrix.
void charptrmatrix::AddRows(int add, int length) {
  charptrvector** vnew = new charptrvector*[nrow + add];
  int i;
  for (i = 0; i < nrow; i++)
    vnew[i] = v[i];
  delete[] v;
  v = vnew;
  for (i = nrow; i < nrow + add; i++)
    v[i] = new charptrvector(length);
  nrow += add;
}

//Adds rows to a charptrmatrix and initializes them.
void charptrmatrix::AddRows(int add, int length, char* initial) {
  int oldnrow = nrow;
  this->AddRows(add, length);
  int i, j;
  for (i = oldnrow; i < nrow; i++)
    for (j = 0; j < length; j++)
      (*v[i])[j] = initial;
}

void charptrmatrix::DeleteRow(int row) {
  assert(nrow > 0);
  int i;
  assert(0 <= row && row < nrow);
  delete v[row];
  charptrvector** vnew = new charptrvector*[nrow - 1];
  for (i = 0; i < row; i++)
    vnew[i] = v[i];
  for (i = row; i < nrow - 1; i++)
    vnew[i] = v[i + 1];
  delete[] v;
  v = vnew;
  nrow--;
}
