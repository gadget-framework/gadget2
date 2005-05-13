#include "intvector.h"
#include "doublematrix.h"
#include "doubleindexvector.h"
#include "bandmatrix.h"
#include "mathfunc.h"
#include "gadget.h"

BandMatrix::BandMatrix(const BandMatrix& initial)
  : nrow(initial.nrow), minage(initial.minage) {

  v = new DoubleIndexVector*[nrow];
  int i;
  for (i = 0; i < nrow; i++)
    v[i] = new DoubleIndexVector(*(initial.v[i]));
}

BandMatrix::BandMatrix(const IntVector& minl, const IntVector& size,
  int age, double initial) : nrow(size.Size()), minage(age) {

  v = new DoubleIndexVector*[nrow];
  int i;
  for (i = 0; i < nrow; i++)
    v[i] = new DoubleIndexVector(size[i], minl[i], initial);
}

BandMatrix::BandMatrix(const DoubleMatrix& initial, int age, int minl)
  : nrow(initial.Nrow()), minage(age) {

  v = new DoubleIndexVector*[nrow];
  int i, j;
  int lower, upper;
  for (i = 0; i < nrow; i++) {
    lower = 0;
    upper = initial.Ncol(i) - 1;
    while (isZero(initial[i][lower]) && (lower < upper))
      lower++;
    while (isZero(initial[i][upper]) && (upper > lower))
      upper--;
    if (isZero(initial[i][lower]) && (lower == upper)) {
      lower = 0;
      upper = initial.Ncol(i) - 1;
    }
    v[i] = new DoubleIndexVector(upper - lower + 1, lower + minl);
    for (j = lower; j <= upper; j++)
      (*v[i])[j + minl] = initial[i][j];
  }
}

BandMatrix::BandMatrix(const DoubleIndexVector& initial, int age)
  : nrow(1), minage(age) {

  v = new DoubleIndexVector*[nrow];
  v[0] = new DoubleIndexVector(initial);
}

BandMatrix::BandMatrix(int minl, int size, int age, int nr, double initial)
  : nrow(nr), minage(age) {

  v = new DoubleIndexVector*[nrow];
  int i;
  for (i = 0; i < nrow; i++)
    v[i] = new DoubleIndexVector(size, minl, initial);
}

BandMatrix::~BandMatrix() {
  int i;
  if (v != 0) {
    for (i = 0; i < nrow; i++)
      delete v[i];
    delete[] v;
    v = 0;
  }
}

void BandMatrix::Print(ofstream& outfile) const {
  int i, j;
  int maxcol = 0;

  for (i = minage; i < minage + nrow; i++)
    if (v[i - minage]->maxCol() > maxcol)
      maxcol = v[i - minage]->maxCol();

  for (i = minage; i < minage + nrow; i++) {
    outfile << TAB;
    if (v[i - minage]->minCol() > 0)
      for (j = 0; j < v[i - minage]->minCol(); j++)
        outfile << setw(smallwidth) << setprecision(smallprecision) << 0.0 << sep;

    for (j = v[i - minage]->minCol(); j < v[i - minage]->maxCol(); j++)
      outfile << setw(smallwidth) << setprecision(smallprecision) << (*v[i - minage])[j] << sep;

    if (v[i - minage]->maxCol() < maxcol)
      for (j = v[i - minage]->maxCol(); j < maxcol; j++)
        outfile << setw(smallwidth) << setprecision(smallprecision) << 0.0 << sep;

    outfile << endl;
  }
}
