#include "loglinearregression.h"
#include "gadget.h"

void LogLinearRegression::Fit(const DoubleVector& x, const DoubleVector& y) {
  error = 0;
  DoubleVector Xlog(x);
  DoubleVector Ylog(y);
  this->CleanAndTakeLog(x, y, Xlog, Ylog);
  if (error)
    return;
  LR.Fit(Xlog, Ylog);
  error = LR.Error();
}

void LogLinearRegression::Fit(const DoubleVector& x, const DoubleVector& y, double slope) {
  error = 0;
  DoubleVector Xlog(x);
  DoubleVector Ylog(y);
  this->CleanAndTakeLog(x, y, Xlog, Ylog);
  if (error)
    return;
  LR.Fit(Xlog, Ylog, slope);
  error = LR.Error();
}

void LogLinearRegression::Fit(double intercept, const DoubleVector& x, const DoubleVector& y) {
  error = 0;
  DoubleVector Xlog(x);
  DoubleVector Ylog(y);
  this->CleanAndTakeLog(x, y, Xlog, Ylog);
  if (error)
    return;
  LR.Fit(intercept, Xlog, Ylog);
  error = LR.Error();
}

void LogLinearRegression::Fit(const DoubleVector& x, const DoubleVector& y, double slope, double intercept) {
  error = 0;
  DoubleVector Xlog(x);
  DoubleVector Ylog(y);
  this->CleanAndTakeLog(x, y, Xlog, Ylog);
  if (error)
    return;
  LR.Fit(Xlog, Ylog, slope, intercept);
  error = LR.Error();
}

double LogLinearRegression::SSE() {
  if (error)
    return LR.errorSSE();
  else
    return LR.SSE();
}

double LogLinearRegression::intersection() {
  return LR.intersection();
}

double LogLinearRegression::slope() {
  return LR.slope();
}

void LogLinearRegression::CleanAndTakeLog(const DoubleVector& x,
  const DoubleVector& y, DoubleVector& Xlog, DoubleVector& Ylog) {

  if ((x.Size() != y.Size()) || (Xlog.Size() != x.Size()) || (Xlog.Size() != Ylog.Size())) {
    error = 1;
    return;
  }

  int i, l = 0;
  for (i = 0; i < x.Size(); i++, l++) {
    if (isZero(x[i]) && isZero(y[i])) {
      //omit the point (0,0)
      Xlog.Delete(l);
      Ylog.Delete(l);
      l--;
    } else if ((x[i] < 0) || (y[i] < 0) || (isZero(x[i])) || (isZero(y[i]))) {
      cerr << "Error in LLR - received illegal values of x (" << x[i] << ") and y (" << y[i] << ")\n";
      error = 1;
      return;
    } else {
      Xlog[l] = log(x[i]);
      Ylog[l] = log(y[i]);
    }
  }
}
