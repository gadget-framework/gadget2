#ifndef doublematrixptrmatrix_h
#define doublematrixptrmatrix_h

#include "intvector.h"
#include "doublematrixptrvector.h"

class doublematrix;

class doublematrixptrmatrix {
public:
  doublematrixptrmatrix() { nrow = 0; v = 0; };
  doublematrixptrmatrix(int nrow, int ncol);
  doublematrixptrmatrix(int nrow, int ncol, doublematrix* initial);
  doublematrixptrmatrix(int nrow, const intvector& ncol);
  doublematrixptrmatrix(int nrow, const intvector& ncol, doublematrix* initial);
  doublematrixptrmatrix(const doublematrixptrmatrix& initial);
  ~doublematrixptrmatrix();
  int Ncol(int i = 0) const { return v[i]->Size(); };
  int Nrow() const { return nrow; };
  doublematrixptrvector& operator [] (int pos);
  const doublematrixptrvector& operator [] (int pos) const;
  void AddRows(int add, int length, doublematrix* initial);
  void AddRows(int add, int length);
  void DeleteRow(int row);
protected:
  int nrow;
  doublematrixptrvector** v;
};

#ifdef GADGET_INLINE
#include "doublematrixptrmatrix.icc"
#endif

#endif
