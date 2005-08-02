#include "migrationareaptrvector.h"
#include "gadget.h"

MigrationAreaPtrVector::MigrationAreaPtrVector(int sz) {
  size = (sz > 0 ? sz : 0);
  if (size > 0)
    v = new MigrationArea*[size];
  else
    v = 0;
}

MigrationAreaPtrVector::MigrationAreaPtrVector(int sz, MigrationArea* value) {
  size = (sz > 0 ? sz : 0);
  int i;
  if (size > 0) {
    v = new MigrationArea*[size];
    for (i = 0; i < size; i++)
      v[i] = value;
  } else
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
  int oldsize = size;
  this->resize(addsize);
  int i;
  if (addsize > 0)
    for (i = oldsize; i < size; i++)
      v[i] = value;
}

void MigrationAreaPtrVector::resize(int addsize) {
  int i;
  if (v == 0) {
    size = addsize;
    v = new MigrationArea*[size];
  } else if (addsize > 0) {
    MigrationArea** vnew = new MigrationArea*[addsize + size];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
    size += addsize;
  }
}

void MigrationAreaPtrVector::Delete(int pos) {
  int i;
  if (size > 1) {
    MigrationArea** vnew = new MigrationArea*[size - 1];
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
