#include "bandmatrixptrvector.h"
#include "gadget.h"

bandmatrixptrvector::bandmatrixptrvector(int sz) {
  size = sz;
  if (size > 0)
    v = new bandmatrix*[size];
  else
    v = 0;
}

bandmatrixptrvector::bandmatrixptrvector(int sz, bandmatrix* value) {
  size = sz;
  int i;
  if (size > 0) {
    v = new bandmatrix*[size];
    for (i = 0; i < size; i++)
      v[i] = value;
  } else
    v = 0;
}

//The function resize add addsize elements to a bandmatrixptrvector and fills it vith value.
void bandmatrixptrvector::resize(int addsize, bandmatrix* value) {
  int oldsize = size;
  this->resize(addsize);
  int i;
  if (addsize > 0)
    for (i = oldsize; i < size; i++)
      v[i] = value;
}

void bandmatrixptrvector::resize(int addsize) {
  int i;
  if (v == 0) {
    size = addsize;
    v = new bandmatrix*[size];
  } else if (addsize > 0) {
    bandmatrix** vnew = new bandmatrix*[addsize + size];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
    size += addsize;
  }
}

void bandmatrixptrvector::Delete(int pos) {
  assert(size > 0);
  assert(0 <= pos && pos < size);
  bandmatrix** vnew = new bandmatrix*[size - 1];
  int i;
  for (i = 0; i < pos; i++)
    vnew[i] = v[i];
  for (i = pos; i < size - 1; i++)
    vnew[i] = v[i + 1];
  delete[] v;
  v = vnew;
  size--;
}

bandmatrix*& bandmatrixptrvector::operator [] (int pos) {
  assert(0 <= pos && pos < size);
  return(v[pos]);
}

bandmatrix* const& bandmatrixptrvector::operator [] (int pos) const {
  assert(0 <= pos && pos < size);
  return(v[pos]);
}
