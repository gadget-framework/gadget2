#ifndef predatorptrvector_h
#define predatorptrvector_h

class Predator;

class Predatorptrvector {
public:
  Predatorptrvector() { size = 0; v = 0; };
  Predatorptrvector(int sz);
  Predatorptrvector(int sz, Predator* initial);
  Predatorptrvector(const Predatorptrvector& initial);
  ~Predatorptrvector();
  void resize(int add, Predator* value);
  void resize(int add);
  void Delete(int pos);
  int Size() const { return size; };
  Predator*& operator [] (int pos);
  Predator* const& operator [] (int pos) const;
protected:
  Predator** v;
  int size;
};

#ifdef GADGET_INLINE
#include "predatorptrvector.icc"
#endif

#endif
