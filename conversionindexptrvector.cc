#include "conversionindexptrvector.h"
#include "gadget.h"

ConversionIndexPtrVector::ConversionIndexPtrVector(int sz) {
  size = (sz > 0 ? sz : 0);
  if (size > 0)
    v = new ConversionIndex*[size];
  else
    v = 0;
}

ConversionIndexPtrVector::ConversionIndexPtrVector(int sz, ConversionIndex* value) {
  size = (sz > 0 ? sz : 0);
  int i;
  if (size > 0) {
    v = new ConversionIndex*[size];
    for (i = 0; i < size; i++)
      v[i] = value;
  } else
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
  int oldsize = size;
  this->resize(addsize);
  int i;
  if (addsize > 0)
    for (i = oldsize; i < size; i++)
      v[i] = value;
}

void ConversionIndexPtrVector::resize(int addsize) {
  int i;
  if (v == 0) {
    size = addsize;
    v = new ConversionIndex*[size];
  } else if (addsize > 0) {
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
