#include "tagptrvector.h"

#ifndef GADGET_INLINE
#include "tagptrvector.icc"
#endif

Tagptrvector::Tagptrvector(int sz) {
  size = (sz > 0 ? sz : 0);
  if (size > 0)
    v = new Tags*[size];
  else
    v = 0;
  index = 0;
}

Tagptrvector::Tagptrvector(int sz, Tags* value) {
  size = (sz > 0 ? sz : 0);
  int i;
  if (size > 0) {
    v = new Tags*[size];
    for (i = 0; i < size; i++)
      v[i] = value;
  } else
    v = 0;

  index = 0;
}

Tagptrvector::Tagptrvector(const Tagptrvector& initial) {
  size = initial.size;
  int i;
  if (size > 0) {
    v = new Tags*[size];
    for (i = 0; i < size; i++)
      v[i] = initial.v[i];
  } else
    v = 0;

  index = 0;
}

Tagptrvector::~Tagptrvector() {
  if (v != 0) {
    delete[] v;
    v = 0;
  }
}

//The function resize add addsize elements to a Tagptrvector and fills it with value.
void Tagptrvector::resize(int addsize, Tags* value) {
  int oldsize = size;
  this->resize(addsize);
  int i;
  if (addsize > 0)
    for (i = oldsize; i < size; i++)
      v[i] = value;
}

//Set the names of all tagged stocks
void Tagptrvector::SetTaggedStocks(const charptrvector Names) {
  int i;
  for (i = 0; i < Names.Size(); i++) {
    tagstocknames.resize(1);
    tagstocknames[i] = new char[strlen(Names[i]) + 1];
    strcpy(tagstocknames[i], Names[i]);
  }
}

void Tagptrvector::resize(int addsize) {
  int i;
  if (v == 0) {
    size = addsize;
    v = new Tags*[size];
  } else if (addsize > 0) {
    Tags** vnew = new Tags*[addsize + size];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
    size += addsize;
  }
}

void Tagptrvector::Delete(int pos) {
  assert(size > 0);
  assert(0 <= pos && pos < size);
  Tags** vnew = new Tags*[size - 1];
  int i;
  for (i = 0; i < pos; i++)
    vnew[i] = v[i];
  for (i = pos; i < size - 1; i++)
    vnew[i] = v[i + 1];
  delete[] v;
  v = vnew;
  size--;
}

void Tagptrvector::UpdateTags(const TimeClass* const TimeInfo) {
  for (index = 0; index < size; index++) {
    if ((v[index]->getTagYear() == TimeInfo->CurrentYear()) && (v[index]->getTagStep() == TimeInfo->CurrentStep()))
      v[index]->Update();

  }
}

void Tagptrvector::DeleteTags(const TimeClass* const TimeInfo) {
  for (index = 0; index < size; index++) {
    if ((v[index]->getEndYear() == TimeInfo->CurrentYear()) && (v[index]->getEndStep() == TimeInfo->CurrentStep()))
      v[index]->DeleteFromStock();

  }
}

void Tagptrvector::DeleteAll() {
  if (v != 0) {
    delete [] v;
    v = 0;
  }
  size = 0;
  index = 0;
}
