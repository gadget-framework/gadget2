#ifndef formulavector_h
#define formulavector_h

#include "formula.h"

class Formulavector {
public:
  Formulavector() : size(0), v(0) {};
  Formulavector(int sz) : size(sz > 0 ? sz : 0), v(sz > 0 ? new Formula[size] : 0) {};
  Formulavector(const Formulavector& initial);
  ~Formulavector();
  void resize(int, Keeper*);
  int Size() const { return size; };
  Formula& operator [] (int pos);
  const Formula& operator [] (int pos) const;
  void Inform(Keeper*);
  friend CommentStream& operator >> (CommentStream&, Formulavector&);
protected:
  int size;
  Formula* v;
};

#ifdef GADGET_INLINE
#include "formulavector.icc"
#endif

#endif
