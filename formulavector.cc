#include "formulavector.h"

FormulaVector::FormulaVector(const FormulaVector& initial) {
  int i;
  size = initial.size;
  if (size > 0) {
    v = new Formula[size];
    for (i = 0; i < size; i++)
      v[i] = initial.v[i];
  } else
    v = 0;
}

FormulaVector::FormulaVector(int sz, Formula initial) {
  int i;
  size = (sz > 0 ? sz : 0);
  if (size > 0) {
    v = new Formula[size];
    for (i = 0; i < size; i++)
      v[i] = initial;
  } else
    v = 0;
}

FormulaVector::FormulaVector(int sz, double initial) {
  int i;
  size = (sz > 0 ? sz : 0);
  if (size > 0) {
    v = new Formula[size];
    for (i = 0; i < size; i++)
      v[i].setValue(initial);
  } else
    v = 0;
}

FormulaVector::~FormulaVector() {
  if (v != 0) {
    delete[] v;
    v = 0;
  }
}

void FormulaVector::resize(int addsize, Keeper* keeper) {
  if (addsize <= 0)
    return;

  int i;
  if (v == 0) {
    size = addsize;
    v = new Formula[size];
  } else {
    Formula* vnew = new Formula[size + addsize];
    for (i = 0; i < size; i++)
      v[i].Interchange(vnew[i], keeper);
    delete[] v;
    v = vnew;
    size += addsize;
  }
}

void FormulaVector::Inform(Keeper* keeper) {
  int i;
  for (i = 0; i < size; i++)
    v[i].Inform(keeper);
}

void FormulaVector::Delete(int pos, Keeper* keeper) {
  int i;
  if (size > 1) {
    Formula* vnew = new Formula[size - 1];
    for (i = 0; i < pos; i++)
      v[i].Interchange(vnew[i], keeper);
    for (i = pos; i < size - 1; i++)
      v[i + 1].Interchange(vnew[i], keeper);
    delete[] v;
    v = vnew;
    size--;
  } else {
    delete[] v;
    v = 0;
    size = 0;
  }
}
