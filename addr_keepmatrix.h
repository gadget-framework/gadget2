#ifndef addr_keepmatrix_h
#define addr_keepmatrix_h

#include "intvector.h"
#include "addr_keepvector.h"

class addr_keepmatrix {
public:
  addr_keepmatrix () { nrow = 0; v = 0; };
  addr_keepmatrix (int nrow, int ncol);
  addr_keepmatrix (int nrow, int ncol, addr_keep initial);
  addr_keepmatrix (int nrow, const intvector& ncol);
  addr_keepmatrix (int nrow, const intvector& ncol, addr_keep initial);
  addr_keepmatrix (const addr_keepmatrix& initial);
  ~addr_keepmatrix ();
  int Ncol (int i = 0) const { return(v[i]->Size()); };
  int Nrow () const { return nrow; };
  addr_keepvector& operator [] (int pos);
  const addr_keepvector& operator [] (int pos) const;
  void AddRows (int add, int length, addr_keep initial);
  void AddRows (int add, int length);
  void DeleteRow (int row);
protected:
  int nrow;
  addr_keepvector** v;
};

#endif
