#include "parametervector.h"
#include "gadget.h"

ParameterVector::ParameterVector(int sz) {
  size = (sz > 0 ? sz : 0);
  if (size > 0)
    v = new Parameter[size];
  else
    v = 0;
}

ParameterVector::ParameterVector(const ParameterVector& initial) {
  size = initial.size;
  int i;
  if (size > 0) {
    v = new Parameter[size];
    for (i = 0; i < size; i++)
      v[i] = initial.v[i];
  } else
    v = 0;
}

ParameterVector::~ParameterVector() {
  if (v != 0) {
    delete[] v;
    v = 0;
  }
}

void ParameterVector::resize(Parameter& value) {
  int i;
  if (v == 0) {
    v = new Parameter[1];
  } else {
    Parameter* vnew = new Parameter[size + 1];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
  }
  v[size] = value;
  size++;
}

void ParameterVector::Delete(int pos) {
  int i;
  if (size > 1) {
    Parameter* vnew = new Parameter[size - 1];
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

ParameterVector& ParameterVector::operator = (const ParameterVector& pv) {
  int i;
  if (size == pv.size) {
    for (i = 0; i < size; i++)
      v[i] = pv[i];
    return *this;
  }
  delete[] v;
  size = pv.size;
  if (size > 0) {
    v = new Parameter[size];
    for (i = 0; i < size; i++)
      v[i] = pv.v[i];
  } else
    v = 0;
  return *this;
}
