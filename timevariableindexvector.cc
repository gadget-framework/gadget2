#include "timevariableindexvector.h"
#include "keeper.h"
#include "commentstream.h"
#include "gadget.h"

#ifndef GADGET_INLINE
#include "timevariableindexvector.icc"
#endif

TimeVariableIndexVector::~TimeVariableIndexVector() {
  if (v != 0) {
    delete[] v;
    v = 0;
  }
}

TimeVariableIndexVector::TimeVariableIndexVector(int sz, int minp) {
  size = (sz > 0 ? sz : 0);
  minpos = minp;
  if (size > 0)
    v = new TimeVariable[size];
  else
    v = 0;
}

void TimeVariableIndexVector::resize(int addsize, int lower, Keeper* const keeper) {
  int i;
  if (v == 0) {
    size = addsize;
    minpos = lower;
    v = new TimeVariable[size];
  } else if (addsize > 0) {
    TimeVariable* vnew = new TimeVariable[addsize + size];
    for (i = 0; i < size; i++)
      v[i].Interchange(vnew[i], keeper);
    delete[] v;
    v = vnew;
    size += addsize;
  }
}

void TimeVariableIndexVector::resize(const TimeVariable& tvar, int lower, Keeper* const keeper) {
  int addsize = 1;
  int i;
  if (v == 0) {
    size = addsize;
    v = new TimeVariable[size];
    tvar.Interchange(v[0], keeper);
    minpos = lower;
  } else {
    TimeVariable* vnew = new TimeVariable[addsize + size];
    for (i = 0; i < size; i++)
      v[i].Interchange(vnew[i], keeper);
    delete[] v;
    v = vnew;
    tvar.Interchange(v[size], keeper);
    size += addsize;
  }
}

int TimeVariableIndexVector::DidChange(const TimeClass* const TimeInfo) const {
  int didchange = 0;
  int i;
  for (i = 0; i < size; i++)
    didchange = didchange + v[i].DidChange(TimeInfo);
  return (didchange > 0 ? 1 : 0);
}

void TimeVariableIndexVector::Update(const TimeClass* const TimeInfo) {
  int i;
  for (i = 0; i < size; i++)
    v[i].Update(TimeInfo);
}

void TimeVariableIndexVector::read(CommentStream& infile,
  const TimeClass* const TimeInfo, Keeper* const keeper) {

  int i;
  for (i = 0; i < size; i++)
    v[i].read(infile, TimeInfo, keeper);
}
