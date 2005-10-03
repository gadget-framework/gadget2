#include "charptrmatrix.h"
#include "gadget.h"

CharPtrMatrix::CharPtrMatrix(int nr, int nc) {
  nrow = nr;
  v = new CharPtrVector*[nr];
  int i;
  for (i = 0; i < nr; i++)
    v[i] = new CharPtrVector(nc);
}

CharPtrMatrix::CharPtrMatrix(int nr, int nc, char* value) {
  nrow = nr;
  v = new CharPtrVector*[nr];
  int i, j;
  for (i = 0; i < nr; i++)
    v[i] = new CharPtrVector(nc);
  for (i = 0; i < nr; i++)
    for (j = 0; j < nc; j++)
      (*v[i])[j] = value;
}

CharPtrMatrix::~CharPtrMatrix() {
  int i;
  if (v != 0) {
    for (i = 0; i < nrow; i++)
      delete v[i];
    delete[] v;
    v = 0;
  }
}

void CharPtrMatrix::AddRows(int add, int length) {
  CharPtrVector** vnew = new CharPtrVector*[nrow + add];
  int i;
  for (i = 0; i < nrow; i++)
    vnew[i] = v[i];
  delete[] v;
  v = vnew;
  for (i = nrow; i < nrow + add; i++)
    v[i] = new CharPtrVector(length);
  nrow += add;
}

void CharPtrMatrix::AddRows(int add, int length, char* initial) {
  int oldnrow = nrow;
  this->AddRows(add, length);
  int i, j;
  for (i = oldnrow; i < nrow; i++)
    for (j = 0; j < length; j++)
      (*v[i])[j] = initial;
}
