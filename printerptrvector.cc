#include "printerptrvector.h"
#include "errorhandler.h"
#include "gadget.h"

extern ErrorHandler handle;

PrinterPtrVector::PrinterPtrVector(int sz) {
  size = (sz > 0 ? sz : 0);
  if (size > 0)
    v = new Printer*[size];
  else
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
  if (addsize != 1)
    handle.logMessage(LOGFAIL, "Error in printerptrvector - cannot add entries to vector");

  this->resize(addsize);
  v[size - 1] = value;
}

void PrinterPtrVector::resize(int addsize) {
  if (addsize <= 0)
    return;
  int i;
  if (v == 0) {
    size = addsize;
    v = new Printer*[size];
  } else {
    Printer** vnew = new Printer*[addsize + size];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
    size += addsize;
  }
}
