#ifndef loglinearregression_h
#define loglinearregression_h

#include "linearregression.h"

class LogLinearRegression;

class LogLinearRegression {
public:
  LogLinearRegression();
  void Fit(const doublevector& x, const doublevector& y);
  void Fit(const doublevector& x, const doublevector& y, double slope);
  void Fit(const doublevector& x, const doublevector& y, double slope, double intercept);
  int Error();
  double SSE() const;
  double SSE(const doublevector& x, const doublevector& y);
  double WeightedSSE(const doublevector& x,
    const doublevector& y, const doublevector& weights);
  double Funcval(double x);
  double LogFuncval(double x);
  double intersection() const;
  double slope() const;
  static double ErrorSSE;
protected:
  void CleanAndTakeLog(const doublevector& x, const doublevector& y,
    doublevector& Xlog, doublevector& Ylog);
  LinearRegression LR;
  int error;
};

#endif
