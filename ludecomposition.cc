#include "ludecomposition.h"
#include "errorhandler.h"

extern ErrorHandler handle;

LUDecomposition::LUDecomposition(const DoubleMatrix& A) {
  if (A.Ncol() != A.Nrow()) {
    handle.LogWarning("Error in ludecomposition - matrix not rectangular");
    exit(EXIT_FAILURE);
  }
  illegal = 0;
  int i, k, j;
  double s;

  size = A.Ncol();
  L = DoubleMatrix(size, size, 0.0);
  U = DoubleMatrix(A);
  logdet = 0.0;

  for (k = 0; k < size; k++) {
    L[k][k] = 1.0;
    if (U[k][k] > 0.0 & logdet < verybig)
      logdet += log(U[k][k]);

    else if (U[k][k] <= 0.0 & logdet < verybig) {
      handle.LogWarning("Warning in ludecomposition - non positive number on matrix diagonal");
      illegal = 1;
    }

    for (i = k + 1; i < size; i++) {
      s = U[i][k] / U[k][k];
      L[i][k] = s;
      U[i][k] = 0.0;

      for (j = k + 1; j < size; j++)
        U[i][j] -= s * U[k][j];
    }
  }
}

//  calculates the solution of Ax=b using the LU decomposition calculated in the constructor
DoubleVector LUDecomposition::Solve(const DoubleVector& b) {
  if (size != b.Size()){
    handle.LogWarning("Error in ludecomposition - sizes not the same");
    exit(EXIT_FAILURE);
  }
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
