#ifndef intvector_h
#define intvector_h

class intvector {
public:
  intvector() { size = 0; v = 0; };
  intvector(int sz);
  intvector(int sz, int initial);
  intvector(const intvector& initial);
  ~intvector() { delete[] v; };
  void resize(int add, int value);
  void resize(int add);
  void Delete(int pos);
  int Size() const { return(size); };
  int& operator [] (int pos);
  const int& operator [] (int pos) const;
protected:
  int* v;
  int size;
};

#ifdef INLINE_VECTORS
#include "intvector.icc"
#endif

#endif
