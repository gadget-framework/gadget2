#ifndef doublematrixptrvector_h
#define doublematrixptrvector_h

class doublematrix;

class doublematrixptrvector {
public:
  doublematrixptrvector() { size = 0; v = 0; };
  doublematrixptrvector(int sz);
  doublematrixptrvector(int sz, doublematrix* initial);
  doublematrixptrvector(const doublematrixptrvector& initial);
  ~doublematrixptrvector() { delete[] v; };
  void resize(int add, doublematrix* value);
  void resize(int add);
  void Delete(int pos);
  int Size () const { return(size); };
  doublematrix*& operator [] (int pos);
  doublematrix* const& operator [] (int pos) const;
protected:
  doublematrix** v;
  int size;
};

#ifdef INLINE_VECTORS
#include "doublematrixptrvector.icc"
#endif

#endif
