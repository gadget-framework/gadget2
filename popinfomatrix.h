#ifndef popinfomatrix_h
#define popinfomatrix_h

#include "intvector.h"
#include "popinfovector.h"

class popinfomatrix {
public:
  popinfomatrix() { nrow = 0; v = 0; };
  popinfomatrix(int nrow, int ncol);
  popinfomatrix(int nrow, int ncol, popinfo initial);
  popinfomatrix(int nrow, const intvector& ncol);
  popinfomatrix(int nrow, const intvector& ncol, popinfo initial);
  popinfomatrix(const popinfomatrix& initial);
  ~popinfomatrix();
  int Ncol(int i = 0) const { return(v[i]->Size()); };
  int Nrow() const { return nrow; };
  popinfovector& operator [] (int pos);
  const popinfovector& operator [] (int pos) const;
  void AddRows(int add, int length, popinfo initial);
  void AddRows(int add, int length);
  void DeleteRow(int row);
protected:
  int nrow;
  popinfovector** v;
};

#ifdef INLINE_VECTORS
#include "popinfomatrix.icc"
#endif

#endif
