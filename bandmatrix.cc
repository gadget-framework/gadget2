#include "intvector.h"
#include "doublematrix.h"
#include "doubleindexvector.h"
#include "bandmatrix.h"
#include "mathfunc.h"
#include "gadget.h"

BandMatrix::BandMatrix(const BandMatrix& initial)
  : nrow(initial.nrow), pos(initial.pos) {

  int i;
  v = new DoubleIndexVector*[nrow];
  for (i = 0; i < nrow; i++)
    v[i] = new DoubleIndexVector(*(initial.v[i]));
}

BandMatrix::BandMatrix(const IntVector& minl, const IntVector& size,
  int minpos, double initial) : nrow(size.Size()), pos(minpos) {

  int i;
  v = new DoubleIndexVector*[nrow];
  for (i = 0; i < nrow; i++)
    v[i] = new DoubleIndexVector(size[i], minl[i], initial);
}

BandMatrix::BandMatrix(int minl, int size, int minpos, int nr, double initial)
  : nrow(nr), pos(minpos) {

  int i;
  v = new DoubleIndexVector*[nrow];
  for (i = 0; i < nrow; i++)
    v[i] = new DoubleIndexVector(size, minl, initial);
}

BandMatrix::BandMatrix(const DoubleMatrix& initial, int minpos, int minl)
  : nrow(initial.Nrow()), pos(minpos) {

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

  for (i = pos; i < pos + nrow; i++)
    if (v[i - pos]->maxCol() > maxcol)
      maxcol = v[i - pos]->maxCol();

  for (i = pos; i < pos + nrow; i++) {
    outfile << TAB;
    if (v[i - pos]->minCol() > 0)
      for (j = 0; j < v[i - pos]->minCol(); j++)
        outfile << setw(smallwidth) << setprecision(smallprecision) << 0.0 << sep;

    for (j = v[i - pos]->minCol(); j < v[i - pos]->maxCol(); j++)
      outfile << setw(smallwidth) << setprecision(smallprecision) << (*v[i - pos])[j] << sep;

    if (v[i - pos]->maxCol() < maxcol)
      for (j = v[i - pos]->maxCol(); j < maxcol; j++)
        outfile << setw(smallwidth) << setprecision(smallprecision) << 0.0 << sep;

    outfile << endl;
  }
}
