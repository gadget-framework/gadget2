#ifndef formulavector_h
#define formulavector_h

#include "formula.h"

class Formulavector {
public:
  Formulavector() : size(0), v(0) {};
  Formulavector(int sz) : size(sz > 0 ? sz : 0), v(sz > 0 ? new Formula[size] : 0) {};
  Formulavector(const Formulavector& initial);
  //AJ 06.10.00 What if have not allocated any memory but doing delete anyway?
  //~Formulavector() { delete[] v; };
  ~Formulavector() { if (v != 0) { delete[] v; }; };
  //Post: If this vector is not empty then have updated keeper with new
  //addresses.. If addSise <= 0 and this is not an empty vector then nothing is done.
  void resize(int, Keeper*);
  int Size() const { return(size); };
  Formula& operator [] (int pos);
  const Formula& operator [] (int pos) const;
  void Inform(Keeper*);
  friend CommentStream& operator >> (CommentStream&, Formulavector&);
protected:
  int size;
  Formula* v;
};

#ifdef INLINE_VECTORS
#include "formulavector.icc"
#endif

#endif
