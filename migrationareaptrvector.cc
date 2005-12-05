#include "migrationareaptrvector.h"
#include "gadget.h"

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

void MigrationAreaPtrVector::resize(MigrationArea* value) {
  int i;
  if (v == 0) {
    v = new MigrationArea*[1];
  } else {
    MigrationArea** vnew = new MigrationArea*[size + 1];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
  }
  v[size] = value;
  size++;
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
