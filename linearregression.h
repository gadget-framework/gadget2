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
  virtual double Funcval(double x);
  int Error() const;
  double SSE() const;
  double intersection() const;
  double slope() const;
protected:
  int error;
  double sse;  //Sum of squares of errors.
  double a;    //Coefficients for the line y = a + bx.
  double b;
};

#endif
