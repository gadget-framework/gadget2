#ifndef addr_keepvector_h
#define addr_keepvector_h

#include "addr_keep.h"

class addr_keepvector {
public:
  addr_keepvector () { size = 0; v = 0; };
  addr_keepvector (int sz);
  addr_keepvector (int sz, addr_keep initial);
  addr_keepvector (const addr_keepvector& initial);
  ~addr_keepvector () { delete[] v; };
  void resize (int add, addr_keep value);
  void resize (int add);
  void Delete (int pos);
  int Size () const { return(size); };
  addr_keep& operator [] (int pos);
  const addr_keep& operator [] (int pos) const;
protected:
  addr_keep* v;
  int size;
};

#ifdef INLINE_VECTORS
#include "addr_keepvector.icc"
#endif

#endif
