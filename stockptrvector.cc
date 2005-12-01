#include "stockptrvector.h"
#include "errorhandler.h"
#include "gadget.h"

extern ErrorHandler handle;

StockPtrVector::StockPtrVector(int sz) {
  size = (sz > 0 ? sz : 0);
  if (size > 0)
    v = new Stock*[size];
  else
    v = 0;
}

StockPtrVector::StockPtrVector(const StockPtrVector& initial) {
  size = initial.size;
  int i;
  if (size > 0) {
    v = new Stock*[size];
    for (i = 0; i < size; i++)
      v[i] = initial.v[i];
  } else
    v = 0;
}

StockPtrVector::~StockPtrVector() {
  if (v != 0) {
    delete[] v;
    v = 0;
  }
}

void StockPtrVector::resize(int addsize, Stock* value) {
  if (addsize != 1)
    handle.logMessage(LOGFAIL, "Error in baseclassptrvector - cannot add entries to vector");

  this->resize(addsize);
  v[size - 1] = value;
}

void StockPtrVector::resize(int addsize) {
  if (addsize <= 0)
    return;
  int i;
  if (v == 0) {
    size = addsize;
    v = new Stock*[size];
  } else {
    Stock** vnew = new Stock*[addsize + size];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
    size += addsize;
  }
}

void StockPtrVector::Delete(int pos) {
  int i;
  if (size > 1) {
    Stock** vnew = new Stock*[size - 1];
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

void StockPtrVector::Reset() {
  if (size > 0) {
    delete[] v;
    v = 0;
    size = 0;
  }
}

StockPtrVector& StockPtrVector::operator = (const StockPtrVector& sv) {
  int i;
  if (size == sv.size) {
    for (i = 0; i < size; i++)
      v[i] = sv[i];
    return *this;
  }
  if (v != 0) {
    delete[] v;
    v = 0;
  }
  size = sv.size;
  if (size > 0) {
    v = new Stock*[size];
    for (i = 0; i < size; i++)
      v[i] = sv.v[i];
  } else
    v = 0;
  return *this;
}
