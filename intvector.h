#ifndef intvector_h
#define intvector_h

class intvector {
public:
  intvector() { size = 0; v = 0; };
  intvector(int sz);
  intvector(int sz, int initial);
  intvector(const intvector& initial);
  ~intvector();
  void resize(int add, int value);
  void resize(int add);
  void Delete(int pos);
  int Size() const { return size; };
  int& operator [] (int pos);
  const int& operator [] (int pos) const;
  intvector& operator = (const intvector& d);
protected:
  int* v;
  int size;
};

#ifdef GADGET_INLINE
#include "intvector.icc"
#endif

#endif
