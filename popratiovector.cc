#include "popratiovector.h"
#include "charptrvector.h"
#include "gadget.h"

#ifndef INLINE_VECTORS
#include "popratiovector.icc"
#endif

popratiovector::popratiovector(int sz) {
  size = (sz > 0 ? sz : 0);
  if (size > 0)
    v = new popratio[size];
  else
    v = 0;
}

popratiovector::popratiovector(int sz, popratio value) {
  size = (sz > 0 ? sz : 0);
  int i = 0;
  if (size > 0) {
    v = new popratio[size];
    for (i = 0; i < size; i++)
      v[i] = value;
  } else
    v = 0;
}

popratiovector::popratiovector(const popratiovector& initial) {
  size = initial.size;
  int i = 0;
  if (size > 0) {
    v = new popratio[size];
    for (i = 0; i < size; i++)
      v[i] = initial.v[i];
  } else
    v = 0;
}

//The function resize add addsize elements to a popinfovector and fills it vith value.
void popratiovector::resize(int addsize, popratio value) {
  int oldsize = size;
  this->resize(addsize);
  int i;
  if (addsize > 0)
    for (i = oldsize; i < size; i++)
      v[i] = value;
}

void popratiovector::resize(int addsize, double* num, double rat) {
  int oldsize = size;
  this->resize(addsize);
  int i;
  popratio pop;
  if (addsize > 0) {
    for (i = oldsize; i < size; i++) {
      v[i] = pop;
      v[i].N = num;
      v[i].R = rat;
    }
  }
}

void popratiovector::resize(int addsize) {
  int i;
  if (v == 0) {
    size = addsize;
    v = new popratio[size];
  } else if (addsize > 0) {
    popratio* vnew = new popratio[addsize + size];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];

    delete[] v;
    v = vnew;
    size += addsize;
  }
}

void popratiovector::Delete(int pos) {
  assert(size > 0);
  assert(0 <= pos && pos < size);
  popratio* vnew = new popratio[size - 1];
  int i;
  for (i = 0; i < pos; i++)
    vnew[i] = v[i];
  for (i = pos; i < size - 1; i++)
    vnew[i] = v[i + 1];
  delete[] v;
  v = vnew;
  size--;
}
