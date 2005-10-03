#include "rectangleptrvector.h"
#include "gadget.h"

RectanglePtrVector::RectanglePtrVector(int sz) {
  size = (sz > 0 ? sz : 0);
  if (size > 0)
    v = new Rectangle*[size];
  else
    v = 0;
}

RectanglePtrVector::RectanglePtrVector(int sz, Rectangle* value) {
  size = (sz > 0 ? sz : 0);
  int i;
  if (size > 0) {
    v = new Rectangle*[size];
    for (i = 0; i < size; i++)
      v[i] = value;
  } else
    v = 0;
}

RectanglePtrVector::~RectanglePtrVector() {
  if (v != 0) {
    delete[] v;
    v = 0;
  }
}

void RectanglePtrVector::resize(int addsize, Rectangle* value) {
  int oldsize = size;
  this->resize(addsize);
  int i;
  if (addsize > 0)
    for (i = oldsize; i < size; i++)
      v[i] = value;
}

void RectanglePtrVector::resize(int addsize) {
  int i;
  if (v == 0) {
    size = addsize;
    v = new Rectangle*[size];
  } else if (addsize > 0) {
    Rectangle** vnew = new Rectangle*[addsize + size];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
    size += addsize;
  }
}
