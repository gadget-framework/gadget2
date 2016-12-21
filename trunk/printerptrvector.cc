#include "printerptrvector.h"
#include "gadget.h"

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

void PrinterPtrVector::resize(Printer* value) {
  int i;
  if (v == 0) {
    v = new Printer*[1];
  } else {
    Printer** vnew = new Printer*[size + 1];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
  }
  v[size] = value;
  size++;
}

void PrinterPtrVector::Delete(int pos) {
  int i;
  if (size > 1) {
    Printer** vnew = new Printer*[size - 1];
    for (i = 0; i < pos; i++)
      vnew[i] = v[i];
    for (i = pos; i < size - 1; i++)
      vnew[i] = v[i + 1];
    delete[] v;
    v = vnew;
    size--;
  } else {
    delete[] v;
    v = 0;
    size = 0;
  }
}
