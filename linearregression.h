#ifndef linearregression_h
#define linearregression_h

#include "doublevector.h"

class LinearRegression;

class LinearRegression {
public:
  LinearRegression();
  virtual void Fit(const DoubleVector& x, const DoubleVector& y);
  virtual void Fit(const DoubleVector& x, const DoubleVector& y, double slope);
  virtual void Fit(double intercept, const DoubleVector& x, const DoubleVector& y);
  virtual void Fit(const DoubleVector& x, const DoubleVector& y, double slope, double intercept);
  int Error() { return error; };
  double SSE();
  double errorSSE() { return errorLR; };
  double intersection() { return a; };
  double slope() { return b; };
protected:
  int error;
  double sse;  //Sum of squares of errors.
  double a;    //Coefficients for the line y = a + bx.
  double b;
  double errorLR;
};

#endif
