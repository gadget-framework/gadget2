#include "agebandmatrixptrmatrix.h"

#ifndef GADGET_INLINE
#include "agebandmatrixptrmatrix.icc"
#endif

agebandmatrixptrmatrix::agebandmatrixptrmatrix(int sz) {
  size = (sz > 0 ? sz : 0);
  if (size > 0)
    v = new agebandmatrixptrvector*[size];
  else
    v = 0;
}
agebandmatrixptrmatrix::agebandmatrixptrmatrix(int sz, agebandmatrixptrvector* value) {
  assert(value != NULL);
  size = (sz > 0 ? sz : 0);
  int i;
  if (size > 0) {
    v = new agebandmatrixptrvector*[size];
    for (i = 0; i < size; i++)
      v[i] = value;
  } else
    v = 0;
}

agebandmatrixptrmatrix::~agebandmatrixptrmatrix() {
  if (v != 0) {
    delete[] v;
    v = 0;
  }
}

//The function resize add addsize elements to a agebandmatrixptrmatrix and fills it vith value.
void agebandmatrixptrmatrix::resize(int addsize, agebandmatrixptrvector* value) {
  int oldsize = size;
  this->resize(addsize);
  int i;
  if (addsize > 0)
    for (i = oldsize; i < size; i++)
      v[i] = value;
}

void agebandmatrixptrmatrix::resize(int addsize) {
  int i;
  if (v == 0) {
    size = addsize;
    v = new agebandmatrixptrvector*[size];
  } else if (addsize > 0) {
    agebandmatrixptrvector** vnew = new agebandmatrixptrvector*[addsize + size];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
    size += addsize;
  }
}

void agebandmatrixptrmatrix::Delete(int pos) {
  assert(size > 0);
  assert(0 <= pos && pos < size);
  agebandmatrixptrvector** vnew = new agebandmatrixptrvector*[size - 1];
  int i;
  for (i = 0; i < pos; i++)
    vnew[i] = v[i];
  for (i = pos; i < size - 1; i++)
    vnew[i] = v[i + 1];
  delete[] v;
  v = vnew;
  size--;
}
