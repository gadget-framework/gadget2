#include "conversionindexptrvector.h"
#include "gadget.h"

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

void ConversionIndexPtrVector::resize(ConversionIndex* value) {
  int i;
  if (v == 0) {
    v = new ConversionIndex*[1];
  } else {
    ConversionIndex** vnew = new ConversionIndex*[size + 1];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
  }
  v[size] = value;
  size++;
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
