#include "timevariablevector.h"
#include "keeper.h"
#include "commentstream.h"
#include "gadget.h"

#ifndef INLINE_VECTORS
#include "timevariablevector.icc"
#endif

void TimeVariablevector::resize(int addsize) {
  assert(v == 0);
  size = addsize;
  v = new TimeVariable[size];
}

void TimeVariablevector::resize(int addsize, Keeper* const keeper) {
  int i;
  if (v == 0) {
    size = addsize;
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

//increase the size of a timevariable vector by 1 and put the timevariable tvar there.
void TimeVariablevector::resize(const TimeVariable& tvar, Keeper* const keeper) {
  int addsize = 1;
  int i;
  if (v == 0) {
    size = addsize;
    v = new TimeVariable[size];
    tvar.Interchange(v[0], keeper);
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

int TimeVariablevector::DidChange(const TimeClass* const TimeInfo) const {
  int didchange = 0;
  int i;
  for (i = 0; i < size; i++)
    didchange += v[i].DidChange(TimeInfo);
  return didchange;
}

void TimeVariablevector::Update(const TimeClass* const TimeInfo) {
  int i;
  for (i = 0; i < size; i++)
    v[i].Update(TimeInfo);
}

void TimeVariablevector::Read(CommentStream& infile,
  const TimeClass* const TimeInfo, Keeper* const keeper) {

  int i;
  for (i = 0; i < size; i++)
    v[i].Read(infile, TimeInfo, keeper);
}
