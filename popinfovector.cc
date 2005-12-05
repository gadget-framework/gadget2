#include "popinfovector.h"
#include "gadget.h"

PopInfoVector::PopInfoVector(int sz, PopInfo initial) {
  size = (sz > 0 ? sz : 0);
  int i;
  if (size > 0) {
    v = new PopInfo[size];
    for (i = 0; i < size; i++)
      v[i] = initial;
  } else
    v = 0;
}

PopInfoVector::PopInfoVector(const PopInfoVector& initial) {
  size = initial.size;
  int i;
  if (size > 0) {
    v = new PopInfo[size];
    for (i = 0; i < size; i++)
      v[i] = initial.v[i];
  } else
    v = 0;
}

PopInfoVector::~PopInfoVector() {
  if (v != 0) {
    delete[] v;
    v = 0;
  }
}

void PopInfoVector::resizeBlank(int addsize) {
  if (addsize <= 0)
    return;
  int i;
  if (v == 0) {
    size = addsize;
    v = new PopInfo[size];
  } else {
    PopInfo* vnew = new PopInfo[addsize + size];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
    size += addsize;
  }
}

void PopInfoVector::resize(PopInfo value) {
  int i;
  if (v == 0) {
    v = new PopInfo[1];
  } else {
    PopInfo* vnew = new PopInfo[size + 1];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
  }
  v[size] = value;
  size++;
}

void PopInfoVector::Delete(int pos) {
  int i;
  if (size > 1) {
    PopInfo* vnew = new PopInfo[size - 1];
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
