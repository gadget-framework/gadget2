#ifndef bandmatrix_h
#define bandmatrix_h

#include "doublematrix.h"
#include "doubleindexvector.h"

class BandMatrix {
public:
  BandMatrix(const BandMatrix& initial);
  BandMatrix() { minage = 0; nrow = 0; v = 0; };
  BandMatrix(const IntVector& minl, const IntVector& size,
    int minAge = 0, double initial = 0.0);
  BandMatrix(const DoubleMatrix& initial, int minAge = 0, int minl = 0);
  BandMatrix(const DoubleIndexVector& initial, int age);
  BandMatrix(int minl, int lengthsize, int minAge,
    int nrow, double initial = 0.0);
  ~BandMatrix();
  int Nrow() const { return nrow; };
  DoubleIndexVector& operator [] (int row) {
    assert(minage <= row && row < (minage + nrow));
    return *(v[row - minage]);
  };
  const DoubleIndexVector& operator [] (int row) const {
    assert(minage <= row && row < (minage + nrow));
    return *(v[row - minage]);
  };
  BandMatrix& operator += (BandMatrix& b);
  int Ncol(int row) const { return (operator[](row).Size()); };
  int Ncol() const { return (operator[](minage).Size()); };
  int minRow() const { return minage; };
  int maxRow() const { return minage + nrow - 1; };
  int minCol(int row) const { return (operator[](row).minCol()); };
  int maxCol(int row) const { return (operator[](row).maxCol()); };
  int minAge() const { return minage; };
  int maxAge() const { return minage + nrow - 1; };
  int minLength(int age) const { return (operator[](age).minCol()); };
  int maxLength(int age) const { return (operator[](age).maxCol()); };
  void Print(ofstream& outfile) const;
protected:
  DoubleIndexVector** v;
  int nrow;
  int minage;
};

#endif
