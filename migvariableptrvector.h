#ifndef migvariableptrvector_h
#define migvariableptrvector_h

#include "migvariables.h"

class MigVariableptrvector {
public:
  MigVariableptrvector() { size = 0; v = 0; };
  MigVariableptrvector(int sz);
  ~MigVariableptrvector();
  void resize(int add);
  void Delete(int pos);
  int Size() const { return size; };
  MigVariable*& operator [] (int pos);
  MigVariable* const& operator [] (int pos) const;
protected:
  MigVariable** v;
  int size;
};

#ifdef GADGET_INLINE
#include "migvariableptrvector.icc"
#endif

#endif
