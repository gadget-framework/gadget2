#ifndef catchdistributionptrvector_h
#define catchdistributionptrvector_h

class CatchDistribution;

class CatchDistributionptrvector {
public:
  CatchDistributionptrvector() { size = 0; v = 0; };
  CatchDistributionptrvector(int sz);
  CatchDistributionptrvector(int sz, CatchDistribution* initial);
  CatchDistributionptrvector(const CatchDistributionptrvector& initial);
  ~CatchDistributionptrvector() { delete[] v; };
  void resize(int add, CatchDistribution* value);
  void resize(int add);
  void Delete(int pos);
  int Size() const { return(size); };
  CatchDistribution*& operator [] (int pos);
  CatchDistribution* const& operator [] (int pos) const;
protected:
  CatchDistribution** v;
  int size;
};

#ifdef INLINE_VECTORS
#include "catchdistributionptrvector.icc"
#endif

#endif
