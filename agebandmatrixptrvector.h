#ifndef agebandmatrixptrvector_h
#define agebandmatrixptrvector_h

#include "agebandmatrix.h"

class agebandmatrixptrvector {
public:
  agebandmatrixptrvector() { size = 0; v = 0; };
  agebandmatrixptrvector(int sz);
  agebandmatrixptrvector(int sz, Agebandmatrix* initial);
  agebandmatrixptrvector(int size1, int Minage, const intvector& minl, const intvector& size2);
  agebandmatrixptrvector(const agebandmatrixptrvector& initial);
  ~agebandmatrixptrvector();
  void resize(int add, Agebandmatrix* value);
  void resize(int add);
  void resize(int add, int minage, const intvector& minl, const intvector& size);
  void resize(int add, int minage, int minl, const popinfomatrix& matr);
  void Delete(int pos);
  int Size() const { return size; };
  Agebandmatrix& operator [] (int pos);
  const Agebandmatrix& operator [] (int pos) const;
  void Migrate(const doublematrix& Migrationmatrix);
protected:
  Agebandmatrix** v;
  int size;
};

#ifdef GADGET_INLINE
#include "agebandmatrixptrvector.icc"
#endif

#endif
