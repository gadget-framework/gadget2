#include "loglinearregression.h"
#include "errorhandler.h"
#include "gadget.h"

extern ErrorHandler handle;

void LogLinearRegression::calcFit(const DoubleVector& x, const DoubleVector& y) {
  error = 0;
  this->calcLog(x, y);
  if (error)
    return;
  LR.calcFit(Xlog, Ylog);
  error = LR.getError();
}

void LogLinearRegression::calcFit(const DoubleVector& x, const DoubleVector& y, double slope) {
  error = 0;
  this->calcLog(x, y);
  if (error)
    return;
  LR.calcFit(Xlog, Ylog, slope);
  error = LR.getError();
}

void LogLinearRegression::calcFit(double intercept, const DoubleVector& x, const DoubleVector& y) {
  error = 0;
  this->calcLog(x, y);
  if (error)
    return;
  LR.calcFit(intercept, Xlog, Ylog);
  error = LR.getError();
}

void LogLinearRegression::calcFit(const DoubleVector& x, const DoubleVector& y, double slope, double intercept) {
  error = 0;
  this->calcLog(x, y);
  if (error)
    return;
  LR.calcFit(Xlog, Ylog, slope, intercept);
  error = LR.getError();
}

double LogLinearRegression::getSSE() {
  if (error)
    return verybig;
  return LR.getSSE();
}

double LogLinearRegression::getIntersection() {
  return LR.getIntersection();
}

double LogLinearRegression::getSlope() {
  return LR.getSlope();
}

void LogLinearRegression::calcLog(const DoubleVector& x, const DoubleVector& y) {

  if ((x.Size() != y.Size()) || (x.Size() == 0)) {
    handle.logMessage(LOGWARN, "Warning in log linear regression - size of vectors not the same");
    error = 1;
    return;
  }

  Xlog.Reset();
  Xlog.resize(x.Size(), 0.0);
  Ylog.Reset();
  Ylog.resize(y.Size(), 0.0);

  int i, l = 0;
  for (i = 0; i < x.Size(); i++, l++) {
    if (isZero(x[i]) && isZero(y[i])) {
      //omit the point (0,0)
      Xlog.Delete(l);
      Ylog.Delete(l);
      l--;
    } else if ((x[i] < verysmall) || (y[i] < verysmall)) {
      handle.logMessage(LOGWARN, "Warning in log linear regession - received invalid values");
      error = 1;
      return;
    } else {
      Xlog[l] = log(x[i]);
      Ylog[l] = log(y[i]);
    }
  }
}
