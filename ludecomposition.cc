#include "ludecomposition.h"
#include "errorhandler.h"
#include "global.h"

LUDecomposition::LUDecomposition(const DoubleMatrix& A) {
  if (A.Ncol() != A.Nrow())
    handle.logMessage(LOGFAIL, "Error in ludecomposition - matrix not rectangular");

  illegal = 0;
  int i, k, j;
  double s, tmp;

  size = A.Nrow();
  L = DoubleMatrix(size, size, 0.0);
  U = DoubleMatrix(A);
  logdet = 0.0;
  tmp = 0.0;

  for (k = 0; k < size; k++) {
    L[k][k] = 1.0;

    if (isZero(U[k][k])) {
      handle.logMessage(LOGWARN, "Warning in ludecomposition - zero on matrix diagonal");
      illegal = 1;
      logdet = verybig;
      tmp = 0.0;

    } else if (U[k][k] > 0.0) {
      logdet += log(U[k][k]);
      tmp = 1.0 / U[k][k];

    } else if (U[k][k] < 0.0 ) {
      handle.logMessage(LOGWARN, "Warning in ludecomposition - negative number on matrix diagonal");
      illegal = 1;
      logdet = verybig;
      tmp = 0.0;
    }

    for (i = k + 1; i < size; i++) {
      s = U[i][k] * tmp;
      L[i][k] = s;
      U[i][k] = 0.0;

      for (j = k + 1; j < size; j++)
        U[i][j] -= s * U[k][j];
    }
  }
}

// calculates the solution of Ax=b using the LU decomposition calculated in the constructor
DoubleVector LUDecomposition::Solve(const DoubleVector& b) {
  if (size != b.Size())
    handle.logMessage(LOGFAIL, "Error in ludecomposition - sizes not the same");

  int i, j;
  double s;
  DoubleVector y(b);
  DoubleVector x(size, 0.0);

  for (i = 0; i < size; i++) {
    s = 0.0;
    for (j = 0; j < i; j++)
      s += L[i][j] * y[j];
    y[i] -= s;
  }

  for (i = size - 1; i >= 0; i--) {
    x[i] = y[i];
    s = 0.0;
    for (j = i + 1; j < size; j++)
      s += U[i][j] * x[j];

    x[i] -= s;
    if (isZero(U[i][i])) {
      handle.logMessage(LOGWARN, "Warning in ludecomposition - divide by zero");
    } else
      x[i] /= U[i][i];
  }
  return x;
}
