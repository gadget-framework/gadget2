#include "poppredatorptrvector.h"
#include "gadget.h"

#ifndef INLINE_VECTORS
#include "poppredatorptrvector.icc"
#endif

PopPredatorptrvector::PopPredatorptrvector(int sz) {
  size = (sz > 0 ? sz : 0);
  if (size > 0)
    v = new PopPredator*[size];
  else
    v = 0;
}

PopPredatorptrvector::PopPredatorptrvector(int sz, PopPredator* value) {
  size = (sz > 0 ? sz : 0);
  int i;
  if (size > 0) {
    v = new PopPredator*[size];
    for (i = 0; i < size; i++)
      v[i] = value;
  } else
    v = 0;
}

PopPredatorptrvector::PopPredatorptrvector(const PopPredatorptrvector& initial) {
  size = initial.size;
  int i;
  if (size > 0) {
    v = new PopPredator*[size];
    for (i = 0; i < size; i++)
      v[i] = initial.v[i];
  } else
    v = 0;
}

//The function resize add addsize elements to a PopPredatorptrvector and fills it vith value.
void PopPredatorptrvector::resize(int addsize, PopPredator* value) {
  int oldsize = size;
  this->resize(addsize);
  int i;
  if (addsize > 0)
    for (i = oldsize; i < size; i++)
      v[i] = value;
}

void PopPredatorptrvector::resize(int addsize) {
  int i;
  if (v == 0) {
    size = addsize;
    v = new PopPredator*[size];
  } else if (addsize > 0) {
    PopPredator** vnew = new PopPredator*[addsize + size];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
    size += addsize;
  }
}

void PopPredatorptrvector::Delete(int pos) {
  assert(size > 0);
  assert(0 <= pos && pos < size);
  PopPredator** vnew = new PopPredator*[size - 1];
  int i;
  for (i = 0; i < pos; i++)
    vnew[i] = v[i];
  for (i = pos; i < size - 1; i++)
    vnew[i] = v[i + 1];
  delete[] v;
  v = vnew;
  size--;
}
