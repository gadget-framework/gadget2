#include "migvariableptrvector.h"
#include "gadget.h"

MigVariableptrvector::MigVariableptrvector(int sz) {
  size = (sz > 0 ? sz : 0);
  if (size > 0)
    v = new MigVariable*[size];
  else
    v = 0;
}

void MigVariableptrvector::resize(int addsize) {
  int i;
  if (v == 0) {
    size = addsize;
    v = new MigVariable*[size];
  } else if (addsize > 0) {
    MigVariable** vnew = new MigVariable*[addsize + size];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
    size += addsize;
  }
}

MigVariable*& MigVariableptrvector::operator [] (int pos) {
  assert(0 <= pos && pos < size);
  return(v[pos]);
}

MigVariable* const& MigVariableptrvector::operator [] (int pos) const {
  assert(0 <= pos && pos < size);
  return(v[pos]);
}
