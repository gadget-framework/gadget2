#ifndef doublematrix_h
#define doublematrix_h

#include "doublevector.h"
#include "intvector.h"

class doublematrix {
public:
  doublematrix() { nrow = 0; v = 0; };
  doublematrix(int nrow, int ncol);
  doublematrix(int nrow, int ncol, double initial);
  doublematrix(int nrow, const intvector& ncol);
  doublematrix(int nrow, const intvector& ncol, double initial);
  doublematrix(const doublematrix& initial);
  doublematrix(double* initial, int nr, int nc);
  ~doublematrix();
  int Ncol(int i = 0) const { return(v[i]->Size()); };
  int Nrow() const { return nrow; };
  doublevector& operator [] (int pos);
  const doublevector& operator [] (int pos) const;
  void AddRows(int add, int length, double initial);
  void AddRows(int add, int length);
  void DeleteRow(int row);
  int maxRowSize() const;
  int minRowSize() const;
  int isRectangular() const;
  doublematrix& addMatrix(const doublematrix& d, int nr, int ncol) const;
  doublematrix& subMatrix(const doublematrix& d, int nrow, int ncol) const;
  doublematrix& operator += (const doublematrix& d);
  doublematrix& operator -= (const doublematrix& d);
  doublematrix& operator = (const doublematrix& d);
  doublematrix& operator * (const doublematrix& d) const;
  int operator == (const doublematrix& d) const;
  void setElementsTo(double d);
  doublematrix& operator *= (double d);
  doublematrix& operator * (double d) const;
  doublematrix& operator -= (double d);
  doublematrix& operator - (double d) const;
  doublematrix& operator += (double d);
  doublematrix& operator + (double d) const;
  friend ostream& operator << (ostream& out, const doublematrix& d);
protected:
  int nrow;
  doublevector** v;
};

#ifdef INLINE_VECTORS
#include "doublematrix.icc"
#endif

#endif
