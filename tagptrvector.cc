#include "tagptrvector.h"

#ifndef GADGET_INLINE
#include "tagptrvector.icc"
#endif

TagPtrVector::TagPtrVector(int sz) {
  size = (sz > 0 ? sz : 0);
  if (size > 0)
    v = new Tags*[size];
  else
    v = 0;
  index = 0;
}

TagPtrVector::TagPtrVector(int sz, Tags* value) {
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

TagPtrVector::TagPtrVector(const TagPtrVector& initial) {
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

TagPtrVector::~TagPtrVector() {
  if (v != 0) {
    delete[] v;
    v = 0;
  }
}

void TagPtrVector::resize(int addsize, Tags* value) {
  int oldsize = size;
  this->resize(addsize);
  int i;
  if (addsize > 0)
    for (i = oldsize; i < size; i++)
      v[i] = value;
}

//Set the names of all tagged stocks
void TagPtrVector::SetTaggedStocks(const CharPtrVector Names) {
  int i;
  for (i = 0; i < Names.Size(); i++) {
    tagstocknames.resize(1);
    tagstocknames[i] = new char[strlen(Names[i]) + 1];
    strcpy(tagstocknames[i], Names[i]);
  }
}

void TagPtrVector::resize(int addsize) {
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

void TagPtrVector::Delete(int pos) {
  int i;
  if (size > 1) {
    Tags** vnew = new Tags*[size - 1];
    for (i = 0; i < pos; i++)
      vnew[i] = v[i];
    for (i = pos; i < size - 1; i++)
      vnew[i] = v[i + 1];
    delete[] v;
    v = vnew;
    size--;
  } else {
    delete[] v;
    v = 0;
    size = 0;
  }
}

void TagPtrVector::UpdateTags(const TimeClass* const TimeInfo) {
  for (index = 0; index < size; index++) {
    if ((v[index]->getTagYear() == TimeInfo->CurrentYear()) && (v[index]->getTagStep() == TimeInfo->CurrentStep()))
      v[index]->Update();

  }
}

void TagPtrVector::DeleteTags(const TimeClass* const TimeInfo) {
  for (index = 0; index < size; index++) {
    if ((v[index]->getEndYear() == TimeInfo->CurrentYear()) && (v[index]->getEndStep() == TimeInfo->CurrentStep()))
      v[index]->DeleteFromStock();

  }
}

void TagPtrVector::DeleteAll() {
  if (v != 0) {
    delete [] v;
    v = 0;
  }
  size = 0;
  index = 0;
}
