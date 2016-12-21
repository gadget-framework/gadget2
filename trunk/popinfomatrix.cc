#include "popinfomatrix.h"
#include "gadget.h"

PopInfoMatrix::PopInfoMatrix(int nr, int nc, PopInfo value) {
  nrow = nr;
  v = new PopInfoVector*[nr];
  int i, j;
  for (i = 0; i < nr; i++)
    v[i] = new PopInfoVector(nc, value);
}

PopInfoMatrix::PopInfoMatrix(const PopInfoMatrix& initial) : nrow(initial.nrow) {
  int i;
  if (nrow >= 0) {
    v = new PopInfoVector*[nrow];
    for (i = 0; i < nrow; i++)
      v[i] = new PopInfoVector(initial[i]);
  } else {
    v = 0;
    nrow = 0;
  }
}

PopInfoMatrix::~PopInfoMatrix() {
  int i;
  if (v != 0) {
    for (i = 0; i < nrow; i++)
      delete v[i];
    delete[] v;
    v = 0;
  }
}

void PopInfoMatrix::AddRows(int add, int length, PopInfo value) {
  if (add <= 0)
    return;

  int i;
  if (v == 0) {
   nrow = add;
   v = new PopInfoVector*[nrow];
   for (i = 0; i < nrow; i++)
     v[i] = new PopInfoVector(length, value);

  } else {
    PopInfoVector** vnew = new PopInfoVector*[nrow + add];
    for (i = 0; i < nrow; i++)
      vnew[i] = v[i];
    for (i = nrow; i < nrow + add; i++)
      vnew[i] = new PopInfoVector(length, value);
    delete[] v;
    v = vnew;
    nrow += add;
  }
}

void PopInfoMatrix::Delete(int pos) {
  delete v[pos];
  int i;
  if (nrow > 1) {
    PopInfoVector** vnew = new PopInfoVector*[nrow - 1];
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
