#ifndef agebandmatrixptrmatrix_h
#define agebandmatrixptrmatrix_h

#include "agebandmatrixptrvector.h"

class agebandmatrixptrmatrix {
public:
  agebandmatrixptrmatrix() { size = 0; v = 0; };
  agebandmatrixptrmatrix(int sz);
  agebandmatrixptrmatrix(int sz, agebandmatrixptrvector* initial);
  agebandmatrixptrmatrix(const agebandmatrixptrmatrix& initial);
  ~agebandmatrixptrmatrix();
  void resize(int add, agebandmatrixptrvector* value);
  void resize(int add);
  void Delete(int pos);
  int Size() const { return size; };
  agebandmatrixptrvector& operator [] (int pos);
  const agebandmatrixptrvector& operator [] (int pos) const;
protected:
  agebandmatrixptrvector** v;
  int size;
};

#ifdef GADGET_INLINE
#include "agebandmatrixptrmatrix.icc"
#endif

#endif
