#ifndef intmatrix_h
#define intmatrix_h

#include "intvector.h"

class intmatrix {
public:
  intmatrix() { nrow = 0; v = 0; };
  intmatrix(int nrow, int ncol);
  intmatrix(int nrow, int ncol, int initial);
  intmatrix(int nrow, const intvector& ncol);
  intmatrix(int nrow, const intvector& ncol, int initial);
  intmatrix(const intmatrix& initial);
  ~intmatrix();
  int Ncol(int i = 0) const { return(v[i]->Size()); };
  int Nrow() const { return nrow; };
  intvector& operator [] (int pos);
  const intvector& operator [] (int pos) const;
  void AddRows(int add, int length, int initial);
  void AddRows(int add, int length);
  void DeleteRow(int row);
protected:
  int nrow;
  intvector** v;
};

#ifdef INLINE_VECTORS
#include "intmatrix.icc"
#endif

#endif
