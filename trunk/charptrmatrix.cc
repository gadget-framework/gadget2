#include "charptrmatrix.h"
#include "gadget.h"

CharPtrMatrix::~CharPtrMatrix() {
  int i;
  if (v != 0) {
    for (i = 0; i < nrow; i++)
      delete v[i];
    delete[] v;
    v = 0;
  }
}

void CharPtrMatrix::resize() {
  int i;
  if (v == 0) {
    v = new CharPtrVector*[1];
  } else {
    CharPtrVector** vnew = new CharPtrVector*[nrow + 1];
    for (i = 0; i < nrow; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
  }
  v[nrow] = new CharPtrVector();
  nrow++;
}
