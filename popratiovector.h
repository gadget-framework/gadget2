#ifndef popratiovector_h
#define popratiovector_h

#include "conversion.h"
#include "popratio.h"

class popratiovector;

class popratiovector {
public:
  popratiovector() { size = 0; v = 0;};
  popratiovector(int sz);
  popratiovector(int sz, popratio initial);
  popratiovector(const popratiovector& initial);
  ~popratiovector();
  void resize(int add, popratio value);
  void resize(int add, double* num, double rat);
  void resize(int add);
  void Delete(int pos);
  int Size() const { return size; };
  popratio& operator [] (int pos);
  const popratio& operator [] (int pos) const;
  void Sum(const popratiovector* const Number, const ConversionIndex& CI);
protected:
  popratio* v;
  int size;
};

#ifdef GADGET_INLINE
#include "popratiovector.icc"
#endif

#endif
