#include "printerptrvector.h"
#include "gadget.h"

#ifndef GADGET_INLINE
#include "printerptrvector.icc"
#endif

PrinterPtrVector::PrinterPtrVector(int sz) {
  size = (sz > 0 ? sz : 0);
  if (size > 0)
    v = new Printer*[size];
  else
    v = 0;
}

PrinterPtrVector::PrinterPtrVector(int sz, Printer* value) {
  size = (sz > 0 ? sz : 0);
  int i;
  if (size > 0) {
    v = new Printer*[size];
    for (i = 0; i < size; i++)
      v[i] = value;
  } else
    v = 0;
}

PrinterPtrVector::PrinterPtrVector(const PrinterPtrVector& initial) {
  size = initial.size;
  int i;
  if (size > 0) {
    v = new Printer*[size];
    for (i = 0; i < size; i++)
      v[i] = initial.v[i];
  } else
    v = 0;
}

PrinterPtrVector::~PrinterPtrVector() {
  if (v != 0) {
    delete[] v;
    v = 0;
  }
}

void PrinterPtrVector::resize(int addsize, Printer* value) {
  int oldsize = size;
  this->resize(addsize);
  int i;
  if (addsize > 0)
    for (i = oldsize; i < size; i++)
      v[i] = value;
}

void PrinterPtrVector::resize(int addsize) {
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

void PrinterPtrVector::Delete(int pos) {
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
