#ifndef bandmatrixptrmatrix_h
#define bandmatrixptrmatrix_h

#include "bandmatrixptrvector.h"
#include "intvector.h"

class bandmatrix;

class bandmatrixptrmatrix {
public:
  bandmatrixptrmatrix() { nrow = 0; v = 0; };
  bandmatrixptrmatrix(int nrow, int ncol);
  bandmatrixptrmatrix(int nrow, int ncol, bandmatrix* initial);
  bandmatrixptrmatrix(int nrow, const intvector& ncol);
  bandmatrixptrmatrix(int nrow, const intvector& ncol, bandmatrix* initial);
  bandmatrixptrmatrix(const bandmatrixptrmatrix& initial);
  ~bandmatrixptrmatrix();
  int Ncol(int i = 0) const { return(v[i]->Size()); };
  int Nrow() const { return nrow; };
  bandmatrixptrvector& operator [] (int pos);
  const bandmatrixptrvector& operator [] (int pos) const;
  void AddRows(int add, int length, bandmatrix* initial);
  void AddRows(int add, int length);
  void DeleteRow(int row);
protected:
  int nrow;
  bandmatrixptrvector** v;
};

#endif
