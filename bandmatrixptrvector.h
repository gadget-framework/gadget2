#ifndef bandmatrixptrvector_h
#define bandmatrixptrvector_h

class bandmatrix;
//Warning: There is always the possibility that the arguments will be
//changed when creating instances of this class. E.g. we may have to
//pass the template class as a constant reference to a member function

class bandmatrixptrvector {
public:
  bandmatrixptrvector() { size = 0; v = 0; };
  bandmatrixptrvector(int sz);
  bandmatrixptrvector(int sz, bandmatrix* initial);
  ~bandmatrixptrvector();
  void resize(int add, bandmatrix* value);
  void resize(int add);
  void Delete(int pos);
  int Size() const { return size; };
  bandmatrix*& operator [] (int pos);
  bandmatrix* const& operator [] (int pos) const;
protected:
  bandmatrix** v;
  int size;
};

#ifdef GADGET_INLINE
#include "bandmatrixptrvector.icc"
#endif

#endif
