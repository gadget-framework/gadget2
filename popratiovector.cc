#include "popratiovector.h"
#include "charptrvector.h"
#include "gadget.h"

PopRatioVector::PopRatioVector(int sz) {
  size = (sz > 0 ? sz : 0);
  if (size > 0)
    v = new PopRatio[size];
  else
    v = 0;
}

PopRatioVector::PopRatioVector(int sz, PopRatio value) {
  size = (sz > 0 ? sz : 0);
  int i = 0;
  if (size > 0) {
    v = new PopRatio[size];
    for (i = 0; i < size; i++)
      v[i] = value;
  } else
    v = 0;
}

PopRatioVector::PopRatioVector(const PopRatioVector& initial) {
  size = initial.size;
  int i = 0;
  if (size > 0) {
    v = new PopRatio[size];
    for (i = 0; i < size; i++)
      v[i] = initial.v[i];
  } else
    v = 0;
}

PopRatioVector::~PopRatioVector() {
  if (v != 0) {
    delete[] v;
    v = 0;
  }
}

void PopRatioVector::resize(int addsize, PopRatio value) {
  int oldsize = size;
  this->resize(addsize);
  int i;
  if (addsize > 0)
    for (i = oldsize; i < size; i++)
      v[i] = value;
}

void PopRatioVector::resize(int addsize, double* num, double rat) {
  int oldsize = size;
  this->resize(addsize);
  int i;
  PopRatio pop;
  if (addsize > 0) {
    for (i = oldsize; i < size; i++) {
      v[i] = pop;
      v[i].N = num;
      v[i].R = rat;
    }
  }
}

void PopRatioVector::resize(int addsize) {
  int i;
  if (v == 0) {
    size = addsize;
    v = new PopRatio[size];
  } else if (addsize > 0) {
    PopRatio* vnew = new PopRatio[addsize + size];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
    size += addsize;
  }
}

void PopRatioVector::Delete(int pos) {
  int i;
  if (size > 1) {
    PopRatio* vnew = new PopRatio[size - 1];
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
