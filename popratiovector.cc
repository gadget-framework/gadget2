#include "popratiovector.h"
#include "gadget.h"

PopRatioVector::PopRatioVector(int sz, PopRatio initial) {
  size = (sz > 0 ? sz : 0);
  int i;
  if (size > 0) {
    v = new PopRatio[size];
    for (i = 0; i < size; i++)
      v[i] = initial;
  } else
    v = 0;
}

PopRatioVector::PopRatioVector(const PopRatioVector& initial) {
  size = initial.size;
  int i;
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

void PopRatioVector::resize(PopRatio value) {
  int i;
  if (v == 0) {
    v = new PopRatio[1];
  } else {
    PopRatio* vnew = new PopRatio[size + 1];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
  }
  v[size] = value;
  size++;
}

void PopRatioVector::resize(double* num, double rat) {
  int i;
  if (v == 0) {
    v = new PopRatio[1];
  } else {
    PopRatio* vnew = new PopRatio[size + 1];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
  }
  PopRatio pop;
  v[size] = pop;
  v[size].N = num;
  v[size].R = rat;
  size++;
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
