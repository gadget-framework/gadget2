#include "printerptrvector.h"
#include "gadget.h"

#ifndef GADGET_INLINE
#include "printerptrvector.icc"
#endif

Printerptrvector::Printerptrvector(int sz) {
  size = (sz > 0 ? sz : 0);
  if (size > 0)
    v = new Printer*[size];
  else
    v = 0;
}

Printerptrvector::Printerptrvector(int sz, Printer* value) {
  size = (sz > 0 ? sz : 0);
  int i;
  if (size > 0) {
    v = new Printer*[size];
    for (i = 0; i < size; i++)
      v[i] = value;
  } else
    v = 0;
}

Printerptrvector::Printerptrvector(const Printerptrvector& initial) {
  size = initial.size;
  int i;
  if (size > 0) {
    v = new Printer*[size];
    for (i = 0; i < size; i++)
      v[i] = initial.v[i];
  } else
    v = 0;
}

Printerptrvector::~Printerptrvector() {
  if (v != 0) {
    delete[] v;
    v = 0;
  }
}

//The function resize add addsize elements to a Printerptrvector and fills it vith value.
void Printerptrvector::resize(int addsize, Printer* value) {
  int oldsize = size;
  this->resize(addsize);
  int i;
  if (addsize > 0)
    for (i = oldsize; i < size; i++)
      v[i] = value;
}

void Printerptrvector::resize(int addsize) {
  int i;
  if (v == 0) {
    size = addsize;
    v = new Printer*[size];
  } else if (addsize > 0) {
    Printer** vnew = new Printer*[addsize + size];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
    size += addsize;
  }
}

void Printerptrvector::Delete(int pos) {
  assert(size > 0);
  assert(0 <= pos && pos < size);
  Printer** vnew = new Printer*[size - 1];
  int i;
  for (i = 0; i < pos; i++)
    vnew[i] = v[i];
  for (i = pos; i < size - 1; i++)
    vnew[i] = v[i + 1];
  delete[] v;
  v = vnew;
  size--;
}
