#ifndef bandmatrix_h
#define bandmatrix_h

#include "doublematrix.h"
#include "doubleindexvector.h"

class BandMatrix {
public:
  BandMatrix() { minage = 0; nrow = 0; v = 0; };
  BandMatrix(const BandMatrix& initial);
  BandMatrix(const IntVector& minl, const IntVector& size, int age = 0, double initial = 0.0);
  BandMatrix(const DoubleMatrix& initial, int age = 0, int minl = 0);
  BandMatrix(const DoubleIndexVector& initial, int age);
  BandMatrix(int minl, int size, int age, int nr, double initial = 0.0);
  ~BandMatrix();
  DoubleIndexVector& operator [] (int row) {
    assert(minage <= row && row < (minage + nrow));
    return *(v[row - minage]);
  };
  const DoubleIndexVector& operator [] (int row) const {
    assert(minage <= row && row < (minage + nrow));
    return *(v[row - minage]);
  };
  int Nrow() const { return nrow; };
  int Ncol(int row) const { return v[row - minage]->Size(); };
  int minRow() const { return minage; };
  int maxRow() const { return minage + nrow - 1; };
  int minCol(int row) const { return v[row - minage]->minCol(); };
  int maxCol(int row) const { return v[row - minage]->maxCol(); };
  void Print(ofstream& outfile) const;
protected:
  DoubleIndexVector** v;
  int nrow;
  int minage;
};

#endif
