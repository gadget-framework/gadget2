#include "conversionindexptrvector.h"
#include "gadget.h"

#ifndef INLINE_VECTORS
#include "conversionindexptrvector.icc"
#endif

ConversionIndexptrvector::ConversionIndexptrvector(int sz) {
  size = (sz > 0 ? sz : 0);
  if (size > 0)
    v = new ConversionIndex*[size];
  else
    v = 0;
}

ConversionIndexptrvector::ConversionIndexptrvector(int sz, ConversionIndex* value) {
  size = (sz > 0 ? sz : 0);
  int i;
  if (size > 0) {
    v = new ConversionIndex*[size];
    for (i = 0; i < size; i++)
      v[i] = value;
  } else
    v = 0;
}

ConversionIndexptrvector::ConversionIndexptrvector(const ConversionIndexptrvector& initial) {
  size = initial.size;
  int i;
  if (size > 0) {
    v = new ConversionIndex*[size];
    for (i = 0; i < size; i++)
      v[i] = initial.v[i];
  } else
    v = 0;
}

//The function resize add addsize elements to a ConversionIndexptrvector and fills it vith value.
void ConversionIndexptrvector::resize(int addsize, ConversionIndex* value) {
  int oldsize = size;
  this->resize(addsize);
  int i;
  if (addsize > 0)
    for (i = oldsize; i < size; i++)
      v[i] = value;
}

void ConversionIndexptrvector::resize(int addsize) {
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

void ConversionIndexptrvector::Delete(int pos) {
  assert(size > 0);
  assert(0 <= pos && pos < size);
  ConversionIndex** vnew = new ConversionIndex*[size - 1];
  int i;
  for (i = 0; i < pos; i++)
    vnew[i] = v[i];
  for (i = pos; i < size - 1; i++)
    vnew[i] = v[i + 1];
  delete[] v;
  v = vnew;
  size--;
}
