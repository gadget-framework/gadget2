#ifndef catchdataptrvector_h
#define catchdataptrvector_h

#include "catch.h"

class CatchData;

class CatchDataptrvector {
public:
  CatchDataptrvector() { size = 0; v = 0; };
  CatchDataptrvector(int sz);
  CatchDataptrvector(int sz, CatchData* initial);
  CatchDataptrvector(const CatchDataptrvector& initial);
  ~CatchDataptrvector();
  void resize(int add, CatchData* value);
  void resize(int add);
  void Delete(int pos);
  int Size() const { return size; };
  CatchData*& operator [] (int pos);
  CatchData* const& operator [] (int pos) const;
protected:
  CatchData** v;
  int size;
};

#ifdef GADGET_INLINE
#include "catchdataptrvector.icc"
#endif

#endif
