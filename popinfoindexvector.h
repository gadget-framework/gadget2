#ifndef popinfoindexvector_h
#define popinfoindexvector_h

#include "popinfo.h"

class popinfoindexvector {
public:
  popinfoindexvector() { minpos = 0; size = 0; v = 0; };
  popinfoindexvector(int sz, int minpos);
  popinfoindexvector(int sz, int minpos, popinfo initial);
  popinfoindexvector(const popinfoindexvector& initial);
  ~popinfoindexvector();
  void resize(int addsize, int newminpos, popinfo value);
  popinfo& operator [] (int pos);
  const popinfo& operator [] (int pos) const;
  int Mincol() const { return minpos; };
  int Maxcol() const { return minpos + size; };
  int Size() const { return size; };
  void Delete(int pos);
protected:
  int minpos;
  int size;
  popinfo* v;
};

#ifdef GADGET_INLINE
#include "popinfoindexvector.icc"
#endif

#endif
