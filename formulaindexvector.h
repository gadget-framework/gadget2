#ifndef formulaindexvector_h
#define formulaindexvector_h

#include "formula.h"

class Keeper;

class Formulaindexvector {
public:
  Formulaindexvector () : minpos(0), size(0), v(0) {};
  Formulaindexvector (int sz, int minp) : minpos(minp), size(sz),
    v(sz > 0 ? new Formula[sz] : 0) {};
  ~Formulaindexvector() { delete[] v; };
  void resize(int addsize, int newminpos, Keeper* keeper);
  Formula& operator [] (int pos);
  const Formula& operator [] (int pos) const;
  friend CommentStream& operator >> (CommentStream&, Formulaindexvector&);
  void Inform(Keeper* keeper);
  int Mincol() const { return(minpos); };
  int Maxcol() const { return(minpos+size); };
  int Size() const { return(size); };
private:
  int minpos;
  int size;
  Formula* v;
};

#endif
