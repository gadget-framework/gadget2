#ifndef popinfovector_h
#define popinfovector_h

#include "conversion.h"
#include "popinfo.h"

class popinfovector;

class popinfovector {
public:
  popinfovector() { size = 0; v = 0; };
  popinfovector(int sz);
  popinfovector(int sz, popinfo initial);
  popinfovector(const popinfovector& initial);
  ~popinfovector() { delete[] v; };
  void resize(int add, popinfo value);
  void resize(int add);
  void Delete(int pos);
  int Size() const { return(size); };
  popinfo& operator [] (int pos);
  const popinfo& operator [] (int pos) const;
  void Sum(const popinfovector* const Number, const ConversionIndex& CI);
protected:
  popinfo* v;
  int size;
};

#ifdef INLINE_VECTORS
#include "popinfovector.icc"
#endif

#endif
