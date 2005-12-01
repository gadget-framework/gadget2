#include "rectangleptrvector.h"
#include "errorhandler.h"
#include "gadget.h"

extern ErrorHandler handle;

RectanglePtrVector::RectanglePtrVector(int sz) {
  size = (sz > 0 ? sz : 0);
  if (size > 0)
    v = new Rectangle*[size];
  else
    v = 0;
}

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

void RectanglePtrVector::resize(int addsize, Rectangle* value) {
  if (addsize != 1)
    handle.logMessage(LOGFAIL, "Error in rectangleptrvector - cannot add entries to vector");

  this->resize(addsize);
  v[size - 1] = value;
}

void RectanglePtrVector::resize(int addsize) {
  int i;
  if (addsize <= 0)
    return;
  if (v == 0) {
    size = addsize;
    v = new Rectangle*[size];
  } else {
    Rectangle** vnew = new Rectangle*[addsize + size];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
    size += addsize;
  }
}
