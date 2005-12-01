#include "migrationareaptrvector.h"
#include "errorhandler.h"
#include "gadget.h"

extern ErrorHandler handle;

MigrationAreaPtrVector::MigrationAreaPtrVector(int sz) {
  size = (sz > 0 ? sz : 0);
  if (size > 0)
    v = new MigrationArea*[size];
  else
    v = 0;
}

MigrationAreaPtrVector::MigrationAreaPtrVector(const MigrationAreaPtrVector& initial) {
  size = initial.size;
  int i;
  if (size > 0) {
    v = new MigrationArea*[size];
    for (i = 0; i < size; i++)
      v[i] = initial.v[i];
  } else
    v = 0;
}

MigrationAreaPtrVector::~MigrationAreaPtrVector() {
  if (v != 0) {
    delete[] v;
    v = 0;
  }
}

void MigrationAreaPtrVector::resize(int addsize, MigrationArea* value) {
  if (addsize != 1)
    handle.logMessage(LOGFAIL, "Error in migrationareaptrvector - cannot add entries to vector");

  this->resize(addsize);
  v[size - 1] = value;
}

void MigrationAreaPtrVector::resize(int addsize) {
  if (addsize <= 0)
    return;
  int i;
  if (v == 0) {
    size = addsize;
    v = new MigrationArea*[size];
  } else {
    MigrationArea** vnew = new MigrationArea*[addsize + size];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
    size += addsize;
  }
}
