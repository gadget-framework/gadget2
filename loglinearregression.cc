#include "loglinearregression.h"
#include "gadget.h"

double LogLinearRegression::ErrorSSE = 1e10;

LogLinearRegression::LogLinearRegression() : error(0) {
}

void LogLinearRegression::Fit(const doublevector& x, const doublevector& y) {
  error = 0;
  doublevector Xlog(x);
  doublevector Ylog(y);
  this->CleanAndTakeLog(x, y, Xlog, Ylog);
  if (error)
    return;
  LR.Fit(Xlog, Ylog);
  error = LR.Error();
}

void LogLinearRegression::Fit(const doublevector& x, const doublevector& y, double slope) {
  error = 0;
  doublevector Xlog(x);
  doublevector Ylog(y);
  this->CleanAndTakeLog(x, y, Xlog, Ylog);
  if (error)
    return;
  LR.Fit(Xlog, Ylog, slope);
  error = LR.Error();
}

void LogLinearRegression::Fit(const doublevector& x, const doublevector& y, double slope, double intercept) {
  error = 0;
  doublevector Xlog(x);
  doublevector Ylog(y);
  this->CleanAndTakeLog(x, y, Xlog, Ylog);
  if (error)
    return;
  LR.Fit(Xlog, Ylog, slope, intercept);
  error = LR.Error();
}

double LogLinearRegression::SSE() const {
  if (error)
    return LogLinearRegression::ErrorSSE;
  else
    return LR.SSE();
}

double LogLinearRegression::SSE(const doublevector& x, const doublevector& y) {
  if (x.Size() != y.Size() || error) {
    error = 1;
    return LogLinearRegression::ErrorSSE;
  }
  double tmp, sum = 0.0;
  int i;
  for (i = 0; i < x.Size(); i++) {
    if (!(iszero(x[i]) && iszero(y[i]))) {
      if (x[i] <= 0 || y[i] <= 0) {
        error = 1;
        return LogLinearRegression::ErrorSSE;
      }
      tmp = log(y[i]) - LR.Funcval(log(x[i]));
      sum += tmp * tmp;
    }
  }
  return sum;
}

double LogLinearRegression::WeightedSSE(const doublevector& x,
  const doublevector& y, const doublevector& weights) {

  if (x.Size() != y.Size() || x.Size() != weights.Size() || error) {
    error = 1;
    return LogLinearRegression::ErrorSSE;
  }
  double tmp, sum = 0.0;
  int i;
  for (i = 0; i < x.Size(); i++) {
    if (!(iszero(x[i]) && iszero(y[i]))) {
      if (x[i] <= 0 || y[i] <= 0) {
        error = 1;
        return LogLinearRegression::ErrorSSE;
      }
      tmp = log(y[i]) - LR.Funcval(log(x[i]));
      sum += weights[i] * tmp * tmp;
    }
  }
  return sum;
}

int LogLinearRegression::Error() {
  return error;
}

double LogLinearRegression::Funcval(double x) {
  if (x > 0)
    return exp(LR.Funcval(log(x)));
  else {
    error = 1;
    return 0.0;
  }
}

double LogLinearRegression::LogFuncval(double x) {
  if (x > 0)
    return LR.Funcval(log(x));
  else {
    error = 1;
    return 0.0;
  }
}

double LogLinearRegression::intersection() const {
  return LR.intersection();
}

double LogLinearRegression::slope() const {
  return LR.slope();
}

void LogLinearRegression::CleanAndTakeLog(const doublevector& x,
  const doublevector& y, doublevector& Xlog, doublevector& Ylog) {

  if (x.Size() != y.Size() || Xlog.Size() != x.Size() || Xlog.Size() != Ylog.Size()) {
    error = 1;
    return;
  }

  int i, l = 0;
  for (i = 0; i < x.Size(); i++, l++) {
    if (iszero(x[i]) && iszero(y[i])) {
      //omit the point (0,0)
      Xlog.Delete(l);
      Ylog.Delete(l);
      l--;
    } else if (x[i] < 0 || y[i] < 0 || iszero(x[i]) || iszero(y[i])) {
      cerr << "Error in LLR - received illegal values of x (" << x[i] << ") and y (" << y[i] << ")\n";
      error = 1;
      return;
    } else {
      Xlog[l] = log(x[i]);
      Ylog[l] = log(y[i]);
    }
  }
}
