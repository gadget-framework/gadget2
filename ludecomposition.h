#ifndef ludecomposition_h
#define ludecomposition_h

#include "doublematrix.h"

class LUDecomposition{
public:
  LUDecomposition() {};
  LUDecomposition(const DoubleMatrix&);
  ~LUDecomposition() {};
  DoubleVector Solve(const DoubleVector&);
  double LogDet() { return logdet; };
  int IsIllegal() { return illegal; };
  int Ncol() { return L.Ncol(); };
private:
  DoubleMatrix L;
  DoubleMatrix U;
  double logdet;
  int size;
  int illegal;
};

#endif
