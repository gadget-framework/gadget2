#ifndef charptrmatrix_h
#define charptrmatrix_h

#include "intvector.h"
#include "charptrvector.h"

class charptrmatrix {
public:
  charptrmatrix() { nrow = 0; v = 0; };
  charptrmatrix(int nrow, int ncol);
  charptrmatrix(int nrow, int ncol, char* initial);
  charptrmatrix(int nrow, const intvector& ncol);
  charptrmatrix(int nrow, const intvector& ncol, char* initial);
  charptrmatrix(const charptrmatrix& initial);
  ~charptrmatrix ();
  int Ncol(int i = 0) const { return(v[i]->Size()); };
  int Nrow() const { return nrow; };
  charptrvector& operator [] (int pos);
  const charptrvector& operator [] (int pos) const;
  void AddRows(int add, int length, char* initial);
  void AddRows(int add, int length);
  void DeleteRow(int row);
protected:
  int nrow;
  charptrvector** v;
};

#ifdef INLINE_VECTORS
#include "charptrmatrix.icc"
#endif

#endif
