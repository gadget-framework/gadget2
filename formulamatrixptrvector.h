#ifndef formulamatrixptrvector_h
#define formulamatrixptrvector_h

#include "formulamatrix.h"

class Formulamatrixptrvector {
public:
  Formulamatrixptrvector() { size = 0; v = 0; };
  Formulamatrixptrvector(int sz);
  Formulamatrixptrvector(int sz, Formulamatrix* initial);
  Formulamatrixptrvector(const Formulamatrixptrvector& initial);
  ~Formulamatrixptrvector();
  //AJ 26.10.00 Check if need to inform keeper about new address when resizing and deleting....
  void resize(int add, Formulamatrix* value);
  void resize(int add);
  void Delete(int pos);
  int Size() const { return size; };
  Formulamatrix*& operator [] (int pos);
  Formulamatrix* const& operator [] (int pos) const;
protected:
  Formulamatrix** v;
  int size;
};

#ifdef GADGET_INLINE
#include "formulamatrixptrvector.icc"
#endif

#endif

