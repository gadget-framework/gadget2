#include "intvector.h"
#include "agebandmatrixratio.h"
#include "commentstream.h"
#include "gadget.h"

AgeBandMatrixRatio::AgeBandMatrixRatio(const AgeBandMatrixRatio& initial)
  : minage(initial.minAge()), nrow(initial.Nrow()) {

  int i;
  if (nrow > 0) {
    v = new PopRatioIndexVector*[nrow];
    for (i = 0; i < nrow; i++)
      v[i] = new PopRatioIndexVector(initial[i + minage]);
  } else
    v = 0;
}

AgeBandMatrixRatio::AgeBandMatrixRatio(int age, const IntVector& minl,
  const IntVector& size) : minage(age), nrow(size.Size()) {

  int i;
  if (nrow > 0) {
    v = new PopRatioIndexVector*[nrow];
    for (i = 0; i < nrow; i++)
      v[i] = new PopRatioIndexVector(size[i], minl[i]);
  } else
    v = 0;
}

AgeBandMatrixRatio::AgeBandMatrixRatio(int age, const PopRatioIndexVector& initial)
  : minage(age), nrow(1) {

  v = new PopRatioIndexVector*[1];
  v[0] = new PopRatioIndexVector(initial);
}

AgeBandMatrixRatio::~AgeBandMatrixRatio() {
  int i;
  if (v != 0) {
    for (i = 0; i < nrow; i++)
      delete v[i];
    delete[] v;
    v = 0;
  }
}

int AgeBandMatrixRatio::numTagExperiments() const {
  if (nrow > 0)
    return (*v[0])[v[0]->minCol()].Size();
  return 0;
}

void AgeBandMatrixRatio::setToZero() {
  int i, j, k;
  for (i = 0; i < nrow; i++) {
    for (j = v[i]->minCol(); j < v[i]->maxCol(); j++) {
      for (k = 0; k < this->numTagExperiments(); k++) {
        *(*v[i])[j][k].N = 0.0;
        (*v[i])[j][k].R = 0.0;
      }
    }
  }
}
