#ifndef preyptrvector_h
#define preyptrvector_h

class Prey;

class Preyptrvector {
public:
  Preyptrvector() { size = 0; v = 0; };
  Preyptrvector(int sz);
  Preyptrvector(int sz, Prey* initial);
  Preyptrvector(const Preyptrvector& initial);
  ~Preyptrvector() { delete[] v; };
  void resize(int add, Prey* value);
  void resize(int add);
  void Delete(int pos);
  int Size() const { return(size); };
  Prey*& operator [] (int pos);
  Prey* const& operator [] (int pos) const;
protected:
  Prey** v;
  int size;
};

#ifdef INLINE_VECTORS
#include "preyptrvector.icc"
#endif

#endif
