#include "intvector.h"
#include "gadget.h"

IntVector::IntVector(int sz) {
  size = (sz > 0 ? sz : 0);
  if (size > 0)
    v = new int[size];
  else
    v = 0;
}

IntVector::IntVector(int sz, int value) {
  size = (sz > 0 ? sz : 0);
  int i;
  if (size > 0) {
    v = new int[size];
    for (i = 0; i < size; i++)
      v[i] = value;
  } else
    v = 0;
}

IntVector::IntVector(const IntVector& initial) {
  size = initial.size;
  int i;
  if (size > 0) {
    v = new int[size];
    for (i = 0; i < size; i++)
      v[i] = initial.v[i];
  } else
    v = 0;
}

IntVector::~IntVector() {
  if (v != 0) {
    delete[] v;
    v = 0;
  }
}

void IntVector::resize(int addsize, int value) {
  int oldsize = size;
  this->resize(addsize);
  int i;
  if (addsize > 0)
    for (i = oldsize; i < size; i++)
      v[i] = value;
}

void IntVector::resize(int addsize) {
  int i;
  if (v == 0) {
    size = addsize;
    v = new int[size];
  } else if (addsize > 0) {
    int* vnew = new int[addsize + size];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
    size += addsize;
  }
}

void IntVector::Delete(int pos) {
  int i;
  if (size > 1) {
    int* vnew = new int[size - 1];
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

IntVector& IntVector::operator = (const IntVector& iv) {
  int i;
  if (size == iv.size) {
    for (i = 0; i < size; i++)
      v[i] = iv[i];
    return *this;
  }
  if (v != 0) {
    delete[] v;
    v = 0;
  }
  size = iv.size;
  if (size > 0) {
    v = new int[size];
    for (i = 0; i < size; i++)
      v[i] = iv.v[i];
  } else
    v = 0;
  return *this;
}

void IntVector::Reset() {
  if (size > 0) {
    delete[] v;
    v = 0;
    size = 0;
  }
}

int IntVector::readline(CommentStream& infile) {
  char line[MaxStrLength];
  strncpy(line, "", MaxStrLength);
  infile.getLine(line, MaxStrLength);
  if (infile.fail())
    return 0;

  istringstream istr(line);
  istr >> ws;
  while (!istr.eof()) {
    this->resize(1);
    istr >> v[size - 1];
    if (istr.fail() && !istr.eof())
      return 0;
    istr >> ws;
  }
  return 1;
}
