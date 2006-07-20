#ifndef ludecomposition_h
#define ludecomposition_h

#include "doublematrix.h"

/**
 * \class LUDecomposition
 * \brief This is the class used to calculate a LU decomposition of a rectangular matrix to solve the matrix equation A = bx
 */
class LUDecomposition {
public:
  /**
   * \brief This is the default LUDecomposition constructor
   */
  LUDecomposition() {};
  /**
   * \brief This is the LUDecomposition constructor for a given DoubleMatrix
   * \param A is the DoubleMatrix that will be decomposed into a lower triangular matrix L and an upper triangular matrix U
   */
  LUDecomposition(const DoubleMatrix& A);
  /**
   * \brief This is the default LUDecomposition destructor
   */
  ~LUDecomposition() {};
  /**
   * \brief This function will solve the matrix equation A = bx and return the resulting DoubleVector
   * \param b is the DoubleVector that is to be used to solve the matrix equation A = bx
   * \return x, the solution of the matrix equation A = bx
   */
  DoubleVector Solve(const DoubleVector& b);
  /**
   * \brief This function will return the log of the determinate of the matrix
   * \return logdet
   */
  double getLogDet() { return logdet; };
  /**
   * \brief This function will return the flag to denote whether the algorithm will fail
   * \return illegal
   */
  int isIllegal() { return illegal; };
private:
  /**
   * \brief This is the lower triangular DoubleMatrix
   */
  DoubleMatrix L;
  /**
   * \brief This is the upper triangular DoubleMatrix
   */
  DoubleMatrix U;
  /**
   * \brief This is the log of the determinate of the matrix
   */
  double logdet;
  /**
   * \brief This is the size of the matrix
   */
  int size;
  /**
   * \brief This is the flag to denote that the matrix has a non-positive number of the diagonal which will cause the LUDecomposition algorithm to fail
   */
  int illegal;
};

#endif
