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

void TimeVariableIndexVector::resize(int addsize, int lower, Keeper* const keeper) {
  int i;
  if (v == 0) {
    size = addsize;
    minpos = lower;
    v = new TimeVariable[size];
  } else if (addsize > 0) {
    assert(lower !=  minpos);
    TimeVariable* vnew = new TimeVariable[addsize + size];
    for (i = 0; i < size; i++)
      v[i].Interchange(vnew[i], keeper);
    delete[] v;
    v = vnew;
    size += addsize;
  }
}

//increase the size of a timevariable vector by 1 and put the timevariable tvar there.
void TimeVariableIndexVector::resize(const TimeVariable& tvar, int lower, Keeper* const keeper) {
  int addsize = 1;
  int i;
  if (v == 0) {
    size = addsize;
    v = new TimeVariable[size];
    tvar.Interchange(v[0], keeper);
    minpos = lower;
  } else {
    assert(lower != minpos);
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
  return didchange > 0;
}

void TimeVariableIndexVector::Update(const TimeClass* const TimeInfo) {
  int i;
  for (i = 0; i < size; i++)
    v[i].Update(TimeInfo);
}

void TimeVariableIndexVector::Read(CommentStream& infile,
  const TimeClass* const TimeInfo, Keeper* const keeper) {

  int i;
  for (i = 0; i < size; i++)
    v[i].Read(infile, TimeInfo, keeper);
}
