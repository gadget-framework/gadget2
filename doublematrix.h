#ifndef doublematrix_h
#define doublematrix_h

#include "doublevector.h"
#include "intvector.h"

class DoubleMatrix {
public:
  DoubleMatrix() { nrow = 0; v = 0; };
  DoubleMatrix(int nrow, int ncol);
  DoubleMatrix(int nrow, int ncol, double initial);
  DoubleMatrix(int nrow, const IntVector& ncol);
  DoubleMatrix(int nrow, const IntVector& ncol, double initial);
  DoubleMatrix(const DoubleMatrix& initial);
  DoubleMatrix(double* initial, int nr, int nc);
  ~DoubleMatrix();
  int Ncol(int i = 0) const { return v[i]->Size(); };
  int Nrow() const { return nrow; };
  DoubleVector& operator [] (int pos);
  const DoubleVector& operator [] (int pos) const;
  void AddRows(int add, int length, double initial);
  void AddRows(int add, int length);
  void DeleteRow(int row);
  int maxRowSize() const;
  int minRowSize() const;
  int isRectangular() const;
  DoubleMatrix& addMatrix(const DoubleMatrix& d, int nr, int ncol) const;
  DoubleMatrix& subMatrix(const DoubleMatrix& d, int nrow, int ncol) const;
  DoubleMatrix& operator += (const DoubleMatrix& d);
  DoubleMatrix& operator -= (const DoubleMatrix& d);
  DoubleMatrix& operator = (const DoubleMatrix& d);
  DoubleMatrix& operator * (const DoubleMatrix& d) const;
  int operator == (const DoubleMatrix& d) const;
  void setElementsTo(double d);
  DoubleMatrix& operator *= (double d);
  DoubleMatrix& operator * (double d) const;
  DoubleMatrix& operator -= (double d);
  DoubleMatrix& operator - (double d) const;
  DoubleMatrix& operator += (double d);
  DoubleMatrix& operator + (double d) const;
  friend ostream& operator << (ostream& out, const DoubleMatrix& d);
protected:
  int nrow;
  DoubleVector** v;
};

#ifdef GADGET_INLINE
#include "doublematrix.icc"
#endif

#endif
