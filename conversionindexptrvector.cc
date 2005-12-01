#include "conversionindexptrvector.h"
#include "errorhandler.h"
#include "gadget.h"

extern ErrorHandler handle;

ConversionIndexPtrVector::ConversionIndexPtrVector(int sz) {
  size = (sz > 0 ? sz : 0);
  if (size > 0)
    v = new ConversionIndex*[size];
  else
    v = 0;
}

ConversionIndexPtrVector::ConversionIndexPtrVector(const ConversionIndexPtrVector& initial) {
  size = initial.size;
  int i;
  if (size > 0) {
    v = new ConversionIndex*[size];
    for (i = 0; i < size; i++)
      v[i] = initial.v[i];
  } else
    v = 0;
}

ConversionIndexPtrVector::~ConversionIndexPtrVector() {
  if (v != 0) {
    delete[] v;
    v = 0;
  }
}

void ConversionIndexPtrVector::resize(int addsize, ConversionIndex* value) {
  if (addsize != 1)
    handle.logMessage(LOGFAIL, "Error in conversionindexptrvector - cannot add entries to vector");

  this->resize(addsize);
  v[size - 1] = value;
}

void ConversionIndexPtrVector::resize(int addsize) {
  if (addsize <= 0)
    return;
  int i;
  if (v == 0) {
    size = addsize;
    v = new ConversionIndex*[size];
  } else {
    ConversionIndex** vnew = new ConversionIndex*[addsize + size];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
    size += addsize;
  }
}

void ConversionIndexPtrVector::Delete(int pos) {
  int i;
  if (size > 1) {
    ConversionIndex** vnew = new ConversionIndex*[size - 1];
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
