#ifndef ludecomposition_h
#define ludecomposition_h

#include "doublematrix.h"

class LUDecomposition{
public:
  /**
   * \brief This is the default LUDecomposition constructor
   */
  LUDecomposition() {};
  LUDecomposition(const DoubleMatrix&);
  /**
   * \brief This is the default LUDecomposition destructor
   */
  ~LUDecomposition() {};
  DoubleVector Solve(const DoubleVector&);
  double LogDet() { return logdet; };
  int IsIllegal() { return illegal; };
  int Ncol() { return L.Ncol(); };
private:
  /**
   * \brief This is the lower triangular DoubleMatrix
   */
  DoubleMatrix L;
  DoubleMatrix U;
  double logdet;
  int size;
  int illegal;
};

#endif
