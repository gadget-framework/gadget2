#ifndef suitfuncptrvector_h
#define suitfuncptrvector_h

#include "suitfunc.h"

class SuitFunc;

class SuitfuncPtrvector {
public:
  SuitfuncPtrvector() { size = 0; v = 0; };
  SuitfuncPtrvector(int sz);
  SuitfuncPtrvector(int sz, SuitFunc* initial);
  SuitfuncPtrvector(const SuitfuncPtrvector& initial);
  ~SuitfuncPtrvector() { delete[] v; };
  void resize(int add, SuitFunc* suitf);
  void resize(int add);
  void Delete(int pos, Keeper* const keeper);
  int Size() const { return(size); };
  SuitFunc*& operator [] (int pos);
  SuitFunc* const& operator [] (int pos) const;
protected:
  SuitFunc** v;
  int size;
};

#ifdef INLINE_VECTORS
#include "suitfuncptrvector.icc"
#endif

#endif
