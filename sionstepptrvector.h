#ifndef sionstepptrvector_h
#define sionstepptrvector_h

class SIOnStep;

class SIOnStepptrvector {
 public:
  SIOnStepptrvector() { size = 0; v = 0; };
  SIOnStepptrvector(int sz);
  SIOnStepptrvector(int sz, SIOnStep* initial);
  SIOnStepptrvector(const SIOnStepptrvector& initial);
  ~SIOnStepptrvector() { delete[] v; };
  void resize(int add, SIOnStep* value);
  void resize(int add);
  void Delete(int pos);
  int Size() const { return(size); };
  SIOnStep*& operator [] (int pos);
  SIOnStep* const& operator [] (int pos) const;
protected:
  SIOnStep** v;
  int size;
};

#ifdef INLINE_VECTORS
#include "sionstepptrvector.icc"
#endif

#endif
