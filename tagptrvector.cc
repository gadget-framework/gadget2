#include "tagptrvector.h"
#include "errorhandler.h"
#include "gadget.h"

extern ErrorHandler handle;

TagPtrVector::TagPtrVector(int sz) {
  size = (sz > 0 ? sz : 0);
  if (size > 0)
    v = new Tags*[size];
  else
    v = 0;
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
}

TagPtrVector::~TagPtrVector() {
  if (v != 0) {
    delete[] v;
    v = 0;
  }
}

void TagPtrVector::resize(int addsize, Tags* value) {
  if (addsize != 1)
    handle.logMessage(LOGFAIL, "Error in tagptrvector - cannot add entries to vector");

  this->resize(addsize);
  v[size - 1] = value;
}

void TagPtrVector::resize(int addsize) {
  if (addsize <= 0)
    return;
  int i;
  if (v == 0) {
    size = addsize;
    v = new Tags*[size];
  } else {
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

void TagPtrVector::updateTags(const TimeClass* const TimeInfo) {
  int year = TimeInfo->getYear();
  int step = TimeInfo->getStep();
  int i;
  for (i = 0; i < size; i++)
    if (v[i]->isWithinPeriod(year, step))
      v[i]->updateTags(year, step);
}

void TagPtrVector::deleteTags(const TimeClass* const TimeInfo) {
  int i;
  for (i = 0; i < size; i++) {
    if ((v[i]->getEndYear() == TimeInfo->getYear()) && (TimeInfo->getStep() == TimeInfo->getLastStep()))
      v[i]->deleteStockTags();

  }
}

void TagPtrVector::deleteAllTags() {
  int i;
  for (i = 0; i < size; i++)
    v[i]->deleteStockTags();
}

void TagPtrVector::deleteAll() {
  if (v != 0) {
    delete[] v;
    v = 0;
  }
  size = 0;
}
