#ifndef baseclassptrvector_h
#define baseclassptrvector_h

class BaseClass;

class BaseClassptrvector {
public:
  BaseClassptrvector() { size = 0; v = 0; };
  BaseClassptrvector(int sz);
  BaseClassptrvector(int sz, BaseClass* initial);
  BaseClassptrvector(const BaseClassptrvector& initial);
  ~BaseClassptrvector() { delete[] v; };
  void resize(int add, BaseClass* value);
  void resize(int add);
  void Delete(int pos);
  int Size() const { return(size); };
  BaseClass*& operator [] (int pos);
  BaseClass* const& operator [] (int pos) const;
protected:
  BaseClass** v;
  int size;
};

#ifdef INLINE_VECTORS
#include "baseclassptrvector.icc"
#endif

#endif
