#include "agebandmatrixptrvector.h"

#ifndef INLINE_VECTORS
#include "agebandmatrixptrvector.icc"
#endif

agebandmatrixptrvector::agebandmatrixptrvector(int sz) {
  size = (sz > 0 ? sz : 0);
  if (size > 0)
    v = new Agebandmatrixvector*[size];
  else
    v = 0;
}
agebandmatrixptrvector::agebandmatrixptrvector(int sz, Agebandmatrixvector* value) {
  assert(value != NULL);
  size = (sz > 0 ? sz : 0);
  int i;
  if (size > 0) {
    v = new Agebandmatrixvector*[size];
    for (i = 0; i < size; i++)
      v[i] = value;
  } else
    v = 0;
}

//The function resize add addsize elements to a agebandmatrixptrvector and fills it vith value.
void agebandmatrixptrvector::resize(int addsize, Agebandmatrixvector* value) {
  int oldsize = size;
  this->resize(addsize);
  int i;
  if (addsize > 0)
    for (i = oldsize; i < size; i++)
      v[i] = value;
}

void agebandmatrixptrvector::resize(int addsize) {
  int i;
  if (v == 0) {
    size = addsize;
    v = new Agebandmatrixvector*[size];
  } else if (addsize > 0) {
    Agebandmatrixvector** vnew = new Agebandmatrixvector*[addsize + size];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
    size += addsize;
  }
}

void agebandmatrixptrvector::Delete(int pos) {
  assert(size > 0);
  assert(0 <= pos && pos < size);
  Agebandmatrixvector** vnew = new Agebandmatrixvector*[size - 1];
  int i;
  for (i = 0; i < pos; i++)
    vnew[i] = v[i];
  for (i = pos; i < size - 1; i++)
    vnew[i] = v[i + 1];
  delete[] v;
  v = vnew;
  size--;
}
