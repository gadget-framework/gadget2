#include "stockpreyptrvector.h"
#include "gadget.h"

#ifndef INLINE_VECTORS
#include "stockpreyptrvector.icc"
#endif

StockPreyptrvector::StockPreyptrvector(int sz) {
  size = (sz > 0 ? sz : 0);
  if (size > 0)
    v = new StockPrey*[size];
  else
    v = 0;
}

StockPreyptrvector::StockPreyptrvector(int sz, StockPrey* value) {
  size = (sz > 0 ? sz : 0);
  int i;
  if (size > 0) {
    v = new StockPrey*[size];
    for (i = 0; i < size; i++)
      v[i] = value;
  } else
    v = 0;
}

StockPreyptrvector::StockPreyptrvector(const StockPreyptrvector& initial) {
  size = initial.size;
  int i;
  if (size > 0) {
    v = new StockPrey*[size];
    for (i = 0; i < size; i++)
      v[i] = initial.v[i];
  } else
    v = 0;
}

//The function resize add addsize elements to a StockPreyptrvector and fills it vith value.
void StockPreyptrvector::resize(int addsize, StockPrey* value) {
  int oldsize = size;
  this->resize(addsize);
  int i;
  if (addsize > 0)
    for (i = oldsize; i < size; i++)
      v[i] = value;
}

void StockPreyptrvector::resize(int addsize) {
  int i;
  if (v == 0) {
    size = addsize;
    v = new StockPrey*[size];
  } else if (addsize > 0) {
    StockPrey** vnew = new StockPrey*[addsize + size];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
    size += addsize;
  }
}

void StockPreyptrvector::Delete(int pos) {
  assert(size > 0);
  assert(0 <= pos && pos < size);
  StockPrey** vnew = new StockPrey*[size - 1];
  int i;
  for (i = 0; i < pos; i++)
    vnew[i] = v[i];
  for (i = pos; i < size - 1; i++)
    vnew[i] = v[i + 1];
  delete[] v;
  v = vnew;
  size--;
}
