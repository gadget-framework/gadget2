#ifndef popratiomatrix_h
#define popratiomatrix_h

#include "intvector.h"
#include "popratiovector.h"

class popratiomatrix {
public:
  popratiomatrix() { nrow = 0; v = 0; };
  popratiomatrix(int nrow, int ncol);
  popratiomatrix(int nrow, int ncol, popratio initial);
  popratiomatrix(int nrow, const intvector& ncol);
  popratiomatrix(int nrow, const intvector& ncol, popratio initial);
  popratiomatrix(const popratiomatrix& initial);
  ~popratiomatrix();
  int Ncol(int i = 0) const { return v[i]->Size(); };
  int Nrow() const { return nrow; };
  popratiovector& operator [] (int pos);
  const popratiovector& operator [] (int pos) const;
  void AddRows(int add, int length, popratio initial);
  void AddRows(int add, int length);
  void DeleteRow(int row);
protected:
  int nrow;
  popratiovector** v;
};

#ifdef GADGET_INLINE
#include "popratiomatrix.icc"
#endif

#endif
