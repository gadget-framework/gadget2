#include "charptrvector.h"
#include "gadget.h"

CharPtrVector::CharPtrVector(const CharPtrVector& initial) {
  size = initial.size;
  int i;
  if (size > 0) {
    v = new char*[size];
    for (i = 0; i < size; i++)
      v[i] = initial.v[i];
  } else
    v = 0;
}

CharPtrVector::~CharPtrVector() {
  if (v != 0) {
    delete[] v;
    v = 0;
  }
}

void CharPtrVector::resize(char* value) {
  int i;
  if (v == 0) {
    v = new char*[1];
  } else {
    char** vnew = new char*[size + 1];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
  }
  v[size] = value;
  size++;
}

void CharPtrVector::resizeBlank(int addsize) {
  if (addsize <= 0)
    return;
  int i;
  if (v == 0) {
    size = addsize;
    v = new char*[size];
  } else {
    char** vnew = new char*[addsize + size];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
    size += addsize;
  }
}

void CharPtrVector::Delete(int pos) {
  int i;
  if (size > 1) {
    char** vnew = new char*[size - 1];
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

void CharPtrVector::Reset() {
  if (size > 0) {
    delete[] v;
    v = 0;
    size = 0;
  }
}

CharPtrVector& CharPtrVector::operator = (const CharPtrVector& cv) {
  int i;
  if (size == cv.size) {
    for (i = 0; i < size; i++)
      v[i] = cv[i];
    return *this;
  }
  if (v != 0) {
    delete[] v;
    v = 0;
  }
  size = cv.size;
  if (size > 0) {
    v = new char*[size];
    for (i = 0; i < size; i++)
      v[i] = cv.v[i];
  } else
    v = 0;
  return *this;
}
