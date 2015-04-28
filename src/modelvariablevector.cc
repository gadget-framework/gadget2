#include "modelvariablevector.h"
#include "keeper.h"
#include "commentstream.h"
#include "gadget.h"

void ModelVariableVector::setsize(int sz) {
  size = (sz > 0 ? sz : 0);
  if (v != 0) {
    delete[] v;
    v = 0;
  }
  if (size > 0)
    v = new ModelVariable[size];
  else
    v = 0;
}

void ModelVariableVector::resize(int addsize, Keeper* const keeper) {
  if (addsize <= 0)
    return;
  int i;
  if (v == 0) {
    size = addsize;
    v = new ModelVariable[size];
  } else {
    ModelVariable* vnew = new ModelVariable[addsize + size];
    for (i = 0; i < size; i++)
      v[i].Interchange(vnew[i], keeper);
    delete[] v;
    v = vnew;
    size += addsize;
  }
}

ModelVariableVector::~ModelVariableVector() {
  if (v != 0) {
    delete[] v;
    v = 0;
  }
}

void ModelVariableVector::resize(const ModelVariable& mvar, Keeper* const keeper) {
  int i;
  if (v == 0) {
    v = new ModelVariable[1];
  } else {
    ModelVariable* vnew = new ModelVariable[size + 1];
    for (i = 0; i < size; i++)
      v[i].Interchange(vnew[i], keeper);
    delete[] v;
    v = vnew;
  }
  mvar.Interchange(v[size], keeper);
  size++;
}

int ModelVariableVector::didChange(const TimeClass* const TimeInfo) const {
  int i;
  for (i = 0; i < size; i++)
    if (v[i].didChange(TimeInfo))
      return 1;

  return 0;
}

void ModelVariableVector::Update(const TimeClass* const TimeInfo) {
  int i;
  for (i = 0; i < size; i++)
    v[i].Update(TimeInfo);
}

void ModelVariableVector::read(CommentStream& infile,
  const TimeClass* const TimeInfo, Keeper* const keeper) {

  int i;
  for (i = 0; i < size; i++)
    v[i].read(infile, TimeInfo, keeper);
}
