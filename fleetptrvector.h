#ifndef fleetptrvector_h
#define fleetptrvector_h

class Fleet;

class Fleetptrvector {
public:
  Fleetptrvector() { size = 0; v = 0; };
  Fleetptrvector(int sz);
  Fleetptrvector(int sz, Fleet* initial);
  Fleetptrvector(const Fleetptrvector& initial);
  ~Fleetptrvector() { delete[] v; };
  void resize(int add, Fleet* value);
  void resize(int add);
  void Delete(int pos);
  int Size() const { return(size); };
  Fleet*& operator [] (int pos);
  Fleet* const& operator [] (int pos) const;
protected:
  Fleet** v;
  int size;
};

#ifdef INLINE_VECTORS
#include "fleetptrvector.icc"
#endif

#endif
