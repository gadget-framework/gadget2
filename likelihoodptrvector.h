#ifndef likelihoodptrvector_h
#define likelihoodptrvector_h

class Likelihood;

class Likelihoodptrvector {
public:
  Likelihoodptrvector() { size = 0; v = 0; };
  Likelihoodptrvector(int sz);
  Likelihoodptrvector(int sz, Likelihood* initial);
  Likelihoodptrvector(const Likelihoodptrvector& initial);
  ~Likelihoodptrvector();
  void resize(int add, Likelihood* value);
  void resize(int add);
  void Delete(int pos);
  int Size() const { return size; };
  Likelihood*& operator [] (int pos);
  Likelihood* const& operator [] (int pos) const;
protected:
  Likelihood** v;
  int size;
};

#ifdef GADGET_INLINE
#include "likelihoodptrvector.icc"
#endif

#endif
