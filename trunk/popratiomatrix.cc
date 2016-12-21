#include "popratiomatrix.h"
#include "gadget.h"

PopRatioMatrix::PopRatioMatrix(int nr, int nc, PopRatio value) {
  nrow = nr;
  v = new PopRatioVector*[nr];
  int i, j;
  for (i = 0; i < nr; i++)
    v[i] = new PopRatioVector(nc, value);
}

PopRatioMatrix::PopRatioMatrix(const PopRatioMatrix& initial) : nrow(initial.nrow) {
  int i;
  if (nrow >= 0) {
    v = new PopRatioVector*[nrow];
    for (i = 0; i < nrow; i++)
      v[i] = new PopRatioVector(initial[i]);
  } else {
    v = 0;
    nrow = 0;
  }
}

PopRatioMatrix::~PopRatioMatrix() {
  int i;
  if (v != 0) {
    for (i = 0; i < nrow; i++)
      delete v[i];
    delete[] v;
    v = 0;
  }
}

void PopRatioMatrix::AddRows(int add, int length, PopRatio value) {
  if (add <= 0)
    return;

  int i;
  if (v == 0) {
   nrow = add;
   v = new PopRatioVector*[nrow];
   for (i = 0; i < nrow; i++)
     v[i] = new PopRatioVector(length, value);

  } else {
    PopRatioVector** vnew = new PopRatioVector*[nrow + add];
    for (i = 0; i < nrow; i++)
      vnew[i] = v[i];
    for (i = nrow; i < nrow + add; i++)
      vnew[i] = new PopRatioVector(length, value);
    delete[] v;
    v = vnew;
    nrow += add;
  }
}

void PopRatioMatrix::Delete(int pos) {
  delete v[pos];
  int i;
  if (nrow > 1) {
    PopRatioVector** vnew = new PopRatioVector*[nrow - 1];
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
