#include "bandmatrixptrmatrix.h"
#include "gadget.h"

//constructor for a rectangular bandmatrixptrmatrix.
bandmatrixptrmatrix::bandmatrixptrmatrix(int nr, int nc) {
  nrow = nr;
  int i;
  v = new bandmatrixptrvector*[nr];
  for (i = 0; i < nr; i++)
    v[i] = new bandmatrixptrvector(nc);
}

//constructor for a rectangular bandmatrixptrmatrix with initial value.
bandmatrixptrmatrix::bandmatrixptrmatrix(int nr, int nc, bandmatrix* value) {
  nrow = nr;
  v = new bandmatrixptrvector*[nr];
  int i, j;
  for (i = 0; i < nr; i++)
    v[i] = new bandmatrixptrvector(nc);
  for (i = 0; i < nr; i++) {
    for (j = 0; j < nc; j++)
      (*v[i])[j] = value;
  }
}

//constructor for a possibly nonrectangular bandmatrixptrmatrix - the rows need not
//have the same number of columns.
bandmatrixptrmatrix::bandmatrixptrmatrix(int nr, const intvector& nc) {
  nrow = nr;
  v = new bandmatrixptrvector*[nr];
  int i;
  for (i = 0; i < nr; i++)
    v[i] = new bandmatrixptrvector(nc[i]);
}

//constructor for bandmatrixptrmatrix when all rows do not need to have same number of
//columns, with initial value.
bandmatrixptrmatrix::bandmatrixptrmatrix(int nr, const intvector& nc, bandmatrix* value) {
  nrow = nr;
  v = new bandmatrixptrvector*[nr];
  int i, j;
  for (i = 0; i < nr; i++)
    v[i] = new bandmatrixptrvector(nc[i]);
  for (i = 0; i < nr; i++)
    for (j = 0; j < nc[i]; j++)
      (*v[i])[j] = value;
}

bandmatrixptrmatrix::bandmatrixptrmatrix(const bandmatrixptrmatrix& initial) : nrow(initial.nrow) {
  int i;
  if (nrow >= 0) {
    v = new bandmatrixptrvector*[nrow];
    for (i = 0; i < nrow; i++)
      v[i] = new bandmatrixptrvector(initial[i]);
  } else {
    v = 0;
    nrow = 0;
  }
}

bandmatrixptrmatrix::~bandmatrixptrmatrix() {
  int i;
  if (v != 0) {
    for (i = 0; i < nrow; i++)
      delete v[i];
    delete[] v;
  }
}

bandmatrixptrvector& bandmatrixptrmatrix::operator [] (int pos) {
  assert(0 <= pos && pos < nrow);
  return *v[pos];
}

const bandmatrixptrvector& bandmatrixptrmatrix::operator [] (int pos) const {
  assert(0 <= pos && pos < nrow);
  return *v[pos];
}

//Adds rows to a bandmatrixptrmatrix.
void bandmatrixptrmatrix::AddRows(int add, int length) {
  bandmatrixptrvector** vnew = new bandmatrixptrvector*[nrow + add];
  int i;
  for (i = 0; i < nrow; i++)
    vnew[i] = v[i];
  delete[] v;
  v = vnew;
  for (i = nrow; i < nrow + add; i++)
    v[i] = new bandmatrixptrvector(length);
  nrow += add;
}

//Adds rows to a bandmatrixptrmatrix and initializes them.
void bandmatrixptrmatrix::AddRows(int add, int length, bandmatrix* initial) {
  int oldnrow = nrow;
  this->AddRows(add, length);
  int i, j;
  for (i = oldnrow; i < nrow; i++)
    for (j = 0; j < length; j++)
      (*v[i])[j] = initial;
}

void bandmatrixptrmatrix::DeleteRow(int row) {
  int i;
  assert(nrow > 0);
  assert(0 <= row && row < nrow);
  delete v[row];
  bandmatrixptrvector** vnew = new bandmatrixptrvector*[nrow -1];
  for (i = 0; i < row; i++)
    vnew[i] = v[i];
  for (i = row; i < nrow - 1; i++)
    vnew[i] = v[i + 1];
  delete[] v;
  v = vnew;
  nrow--;
}
