#ifndef popratioindexvector_h
#define popratioindexvector_h

#include "popratiovector.h"

class popratioindexvector {
public:
  popratioindexvector() { minpos = 0; size = 0; v = 0; };
  popratioindexvector(int sz, int minp);
  popratioindexvector(int sz, int minpos, const popratiovector& initial);
  popratioindexvector(const popratioindexvector& initial);
  ~popratioindexvector();
  void resize(int addsize, int newminpos, const popratiovector& initial);
  popratiovector& operator [] (int pos);
  const popratiovector& operator [] (int pos) const;
  int Mincol() const { return(minpos); };
  int Maxcol() const { return(minpos + size); };
  int Size() const { return(size); };
protected:
  int minpos;
  int size;
  popratiovector** v;
};

#endif




