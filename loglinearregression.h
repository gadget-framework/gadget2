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
  int Error();
  double SSE() const;
  double SSE(const DoubleVector& x, const DoubleVector& y);
  double WeightedSSE(const DoubleVector& x,
    const DoubleVector& y, const DoubleVector& weights);
  double Funcval(double x);
  double LogFuncval(double x);
  double intersection() const;
  double slope() const;
  static double ErrorSSE;
protected:
  void CleanAndTakeLog(const DoubleVector& x, const DoubleVector& y,
    DoubleVector& Xlog, DoubleVector& Ylog);
  LinearRegression LR;
  int error;
};

#endif
