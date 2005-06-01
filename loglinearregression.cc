#include "loglinearregression.h"
#include "errorhandler.h"
#include "gadget.h"

extern ErrorHandler handle;

void LogLinearRegression::calcFit(const DoubleVector& x, const DoubleVector& y) {
  error = 0;
  DoubleVector Xlog(x);
  DoubleVector Ylog(y);
  this->calcLog(x, y, Xlog, Ylog);
  if (error)
    return;
  LR.calcFit(Xlog, Ylog);
  error = LR.getError();
}

void LogLinearRegression::calcFit(const DoubleVector& x, const DoubleVector& y, double slope) {
  error = 0;
  DoubleVector Xlog(x);
  DoubleVector Ylog(y);
  this->calcLog(x, y, Xlog, Ylog);
  if (error)
    return;
  LR.calcFit(Xlog, Ylog, slope);
  error = LR.getError();
}

void LogLinearRegression::calcFit(double intercept, const DoubleVector& x, const DoubleVector& y) {
  error = 0;
  DoubleVector Xlog(x);
  DoubleVector Ylog(y);
  this->calcLog(x, y, Xlog, Ylog);
  if (error)
    return;
  LR.calcFit(intercept, Xlog, Ylog);
  error = LR.getError();
}

void LogLinearRegression::calcFit(const DoubleVector& x, const DoubleVector& y, double slope, double intercept) {
  error = 0;
  DoubleVector Xlog(x);
  DoubleVector Ylog(y);
  this->calcLog(x, y, Xlog, Ylog);
  if (error)
    return;
  LR.calcFit(Xlog, Ylog, slope, intercept);
  error = LR.getError();
}

double LogLinearRegression::getSSE() {
  if (error)
    return verybig;
  else
    return LR.getSSE();
}

double LogLinearRegression::getIntersection() {
  return LR.getIntersection();
}

double LogLinearRegression::getSlope() {
  return LR.getSlope();
}

void LogLinearRegression::calcLog(const DoubleVector& x,
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
      if (handle.getLogLevel() >= LOGWARN)
        handle.logMessage(LOGWARN, "Warning in LLR - received invalid values");
      error = 1;
      return;
    } else {
      Xlog[l] = log(x[i]);
      Ylog[l] = log(y[i]);
    }
  }
}
