#include "timevariablevector.h"
#include "keeper.h"
#include "commentstream.h"
#include "gadget.h"

void TimeVariableVector::resize(int addsize) {
  int i;
  if (v == 0) {
    size = addsize;
    v = new TimeVariable[size];
  } else if (addsize > 0) { 
    //this loses the old values and should never happen ...
    TimeVariable* vnew = new TimeVariable[addsize + size];
    delete[] v;
    v = vnew;
    size += addsize;
  }
}

TimeVariableVector::TimeVariableVector(int sz) {
  size = (sz > 0 ? sz : 0);
  if (size > 0)
    v = new TimeVariable[size];
  else
    v = 0;
}

void TimeVariableVector::resize(int addsize, Keeper* const keeper) {
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

TimeVariableVector::~TimeVariableVector() {
  if (v != 0) {
    delete[] v;
    v = 0;
  }
}

void TimeVariableVector::resize(const TimeVariable& tvar, Keeper* const keeper) {
  int i;
  if (v == 0) {
    size = 1;
    v = new TimeVariable[size];
    tvar.Interchange(v[0], keeper);
  } else {
    TimeVariable* vnew = new TimeVariable[size + 1];
    for (i = 0; i < size; i++)
      v[i].Interchange(vnew[i], keeper);
    delete[] v;
    v = vnew;
    tvar.Interchange(v[size], keeper);
    size++;
  }
}

int TimeVariableVector::DidChange(const TimeClass* const TimeInfo) const {
  int didchange = 0;
  int i;
  for (i = 0; i < size; i++)
    didchange += v[i].DidChange(TimeInfo);
  return (didchange > 0 ? 1 : 0);
}

void TimeVariableVector::Update(const TimeClass* const TimeInfo) {
  int i;
  for (i = 0; i < size; i++)
    v[i].Update(TimeInfo);
}

void TimeVariableVector::read(CommentStream& infile,
  const TimeClass* const TimeInfo, Keeper* const keeper) {

  int i;
  for (i = 0; i < size; i++)
    v[i].read(infile, TimeInfo, keeper);
}
