#ifndef ludecomposition_h
#define ludecomposition_h

#include "doublematrix.h"

class LUDecomposition{
public:
  LUDecomposition(const DoubleMatrix&);
  ~LUDecomposition() {};
  DoubleVector Solve(const DoubleVector&);
  double LogDet() { return logdet; };
private:
  DoubleMatrix L;
  DoubleMatrix U;
  double logdet;
  int size;
};

#endif
