#ifndef agebandmatrixptrvector_h
#define agebandmatrixptrvector_h

#include "agebandm.h"

class agebandmatrixptrvector {
public:
  agebandmatrixptrvector() { size = 0; v = 0; };
  agebandmatrixptrvector(int sz);
  agebandmatrixptrvector(int sz, Agebandmatrixvector* initial);
  agebandmatrixptrvector(const agebandmatrixptrvector& initial);
  ~agebandmatrixptrvector();
  void resize(int add, Agebandmatrixvector* value);
  void resize(int add);
  void Delete(int pos);
  int Size() const { return size; };
  Agebandmatrixvector*& operator [] (int pos);
  Agebandmatrixvector* const& operator [] (int pos) const;
protected:
  Agebandmatrixvector** v;
  int size;
};

#ifdef GADGET_INLINE
#include "agebandmatrixptrvector.icc"
#endif

#endif
