#include "linearregression.h"
#include "mathfunc.h"
#include "gadget.h"

LinearRegression::LinearRegression() {
  error = 0;
  sse = 0;
  a = 0;
  b = 0;
  SSEerror = 1e+10;
}

void LinearRegression::Fit(const DoubleVector& x, const DoubleVector& y) {
  error = 0;  //begin with cleaning up error status.
  if ((x.Size() != y.Size()) || (x.Size() == 0)) {
    error = 1;
    return;
  }
  double Xmean = 0.0;
  double Ymean = 0.0;
  int i;
  for (i = 0; i < x.Size(); i++) {
    Xmean += x[i];
    Ymean += y[i];
  }
  Xmean /= x.Size();
  Ymean /= y.Size();

  //Now we have calculated the mean and can proceed to calculate the fit.
  double nominator = 0.0;
  double denom = 0.0;
  for (i = 0; i < x.Size(); i++) {
    nominator += (x[i] - Xmean) * (y[i] - Ymean);
    denom += (x[i] - Xmean) * (x[i] - Xmean);
  }

  if (isZero(denom)) {
    b = 0.0;
    a = Ymean;
  } else {
    b = nominator / denom;
    a = Ymean - b * Xmean;
  }

  //Now we can calculate the sum of squares of errors.
  double tmp;
  sse = 0.0;
  for (i = 0; i < x.Size(); i++) {
    tmp = y[i] - (a + b * x[i]);
    sse += tmp * tmp;
  }
}

void LinearRegression::Fit(const DoubleVector& x, const DoubleVector& y, double slope) {
  error = 0;  //begin with cleaning up error status.
  if ((x.Size() != y.Size()) || (x.Size() == 0)) {
    error = 1;
    return;
  }
  double Xmean = 0.0;
  double Ymean = 0.0;
  int i;
  for (i = 0; i < x.Size(); i++) {
    Xmean += x[i];
    Ymean += y[i];
  }
  Xmean /= x.Size();
  Ymean /= y.Size();

  //Now we have calculated the mean and can proceed to calculate the fit.
  b = slope;
  a = Ymean - b * Xmean;

  //Now we can calculate the sum of squares of errors.
  double tmp;
  sse = 0.0;
  for (i = 0; i < x.Size(); i++) {
    tmp = y[i] - (a + b * x[i]);
    sse += tmp * tmp;
  }
}

void LinearRegression::Fit(double intercept, const DoubleVector& x, const DoubleVector& y) {
  error = 0;  //begin with cleaning up error status.
  if ((x.Size() != y.Size()) || (x.Size() == 0)) {
    error = 1;
    return;
  }
  double Xmean = 0.0;
  double Ymean = 0.0;
  int i;
  for (i = 0; i < x.Size(); i++) {
    Xmean += x[i];
    Ymean += y[i];
  }
  Xmean /= x.Size();
  Ymean /= y.Size();

  //Now we have calculated the mean and can proceed to calculate the fit.
  if (isZero(Xmean))
    b = 0.0;
  else
    b = (Ymean - intercept) / Xmean;

  a = intercept;

  //Now we can calculate the sum of squares of errors.
  double tmp;
  sse = 0.0;
  for (i = 0; i < x.Size(); i++) {
    tmp = y[i] - (a + b * x[i]);
    sse += tmp * tmp;
  }
}

void LinearRegression::Fit(const DoubleVector& x, const DoubleVector& y, double slope, double intercept) {
  error = 0;  //begin with cleaning up error status.
  if ((x.Size() != y.Size()) || (x.Size() == 0)) {
    error = 1;
    return;
  }
  b = slope;
  a = intercept;

  //Now we can calculate the sum of squares of errors.
  int i;
  double tmp;
  sse = 0.0;
  for (i = 0; i < x.Size(); i++) {
    tmp = y[i] - (a + b * x[i]);
    sse += tmp * tmp;
  }
}

double LinearRegression::SSE() {
  if (error)
    return SSEerror;
  else
    return sse;
}
