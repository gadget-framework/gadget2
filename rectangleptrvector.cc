#include "rectangleptrvector.h"
#include "gadget.h"

RectanglePtrVector::RectanglePtrVector(const RectanglePtrVector& initial) {
  size = initial.size;
  int i;
  if (size > 0) {
    v = new Rectangle*[size];
    for (i = 0; i < size; i++)
      v[i] = initial.v[i];
  } else
    v = 0;
}

RectanglePtrVector::~RectanglePtrVector() {
  if (v != 0) {
    delete[] v;
    v = 0;
  }
}

void RectanglePtrVector::resize(Rectangle* value) {
  int i;
  if (v == 0) {
    v = new Rectangle*[1];
  } else {
    Rectangle** vnew = new Rectangle*[size + 1];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
  }
  v[size] = value;
  size++;
}

void RectanglePtrVector::Delete(int pos) {
  int i;
  if (size > 1) {
    Rectangle** vnew = new Rectangle*[size - 1];
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
