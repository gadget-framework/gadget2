#include "formulaindexvector.h"
#include "keeper.h"
#include "gadget.h"

#ifndef GADGET_INLINE
#include "formulaindexvector.icc"
#endif

Formulaindexvector::~Formulaindexvector() {
  if (v != 0) {
    delete[] v;
    v = 0;
  }
}

Formulaindexvector::Formulaindexvector(int sz, int minp) {
  size = (sz > 0 ? sz : 0);
  minpos = minp;
  if (size > 0)
    v = new Formula[size];
  else
    v = 0;
}

void Formulaindexvector::resize(int addsize, int lower, Keeper* keeper) {
  int i;
  if (v == 0) {
    size = addsize;
    minpos = lower;
    v = new Formula[size];
  } else if (addsize > 0) {
    assert(lower <= minpos);
    Formula* vnew = new Formula[size + addsize];
    for (i = 0; i < size; i++)
      v[i].Interchange(vnew[i + minpos - lower], keeper);
    size += addsize;
    delete[] v;
    v = vnew;
    minpos = lower;
  }
}

CommentStream& operator >> (CommentStream& infile, Formulaindexvector& Fvec) {
  if (infile.fail()) {
    infile.makebad();
    return infile;
  }
  int i;
  for (i = Fvec.Mincol(); i < Fvec.Maxcol(); i++) {
    if (!(infile >> Fvec[i])) {
      infile.makebad();
      return infile;
    }
  }
  return infile;
}

void Formulaindexvector::Inform(Keeper* keeper) {
  int i;
  for (i = 0; i < size; i++) {
    ostringstream ostr;
    ostr << minpos + i << ends;
    keeper->AddString(ostr.str());
    v[i].Inform(keeper);
    keeper->ClearLast();
  }
}
