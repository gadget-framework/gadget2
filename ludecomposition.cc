#include "ludecomposition.h"

LUDecomposition::LUDecomposition(const DoubleMatrix& A){
  assert(A.Ncol() == A.Nrow());

  int i, k, j;
  double s;

  size = A.Ncol();
  L = DoubleMatrix(size, size, 0.0);
  U = DoubleMatrix(A);
  logdet = 0.0;

  for (k = 0; k < size; k++) {
    L[k][k] = 1.0;
    logdet += log(U[k][k]);
    for (i = k + 1; i < size; i++) {
      s = U[i][k] / U[k][k];
      L[i][k] = s;
      U[i][k] = 0.0;
      for (j = k + 1; j < size; j++)
        U[i][j] -= s * U[k][j];
    }
  }
}

//calculates the solution of Ax=b using the LU decomposition calculated in the constructor
DoubleVector LUDecomposition::Solve(const DoubleVector& b){
  assert(size == b.Size());

  int i, j;
  double s;
  DoubleVector y = DoubleVector(b);
  DoubleVector x = DoubleVector(size, 0.0);

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
    x[i] /= U[i][i];
  }
  return x;
}
