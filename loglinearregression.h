#ifndef loglinearregression_h
#define loglinearregression_h

#include "linearregression.h"

class LogLinearRegression;

class LogLinearRegression {
public:
  LogLinearRegression();
  void Fit(const DoubleVector& x, const DoubleVector& y);
  void Fit(const DoubleVector& x, const DoubleVector& y, double slope);
  void Fit(double intercept, const DoubleVector& x, const DoubleVector& y);
  void Fit(const DoubleVector& x, const DoubleVector& y, double slope, double intercept);
  int Error() { return error; };
  double SSE();
  double intersection();
  double slope();
protected:
  void CleanAndTakeLog(const DoubleVector& x, const DoubleVector& y,
    DoubleVector& Xlog, DoubleVector& Ylog);
  LinearRegression LR;
  int error;
};

#endif
