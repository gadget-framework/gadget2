#ifndef charptrvector_h
#define charptrvector_h

class charptrvector {
public:
  charptrvector() { size = 0; v = 0; };
  charptrvector(int sz);
  charptrvector(int sz, char* initial);
  charptrvector(const charptrvector& initial);
  ~charptrvector();
  void resize(int add, char* value);
  void resize(int add);
  void Delete(int pos);
  int Size() const { return size; };
  char*& operator [] (int pos);
  char* const& operator [] (int pos) const;
protected:
  char** v;
  int size;
};

#ifdef GADGET_INLINE
#include "charptrvector.icc"
#endif

#endif
