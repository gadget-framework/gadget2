#ifndef multinomial_h
#define multinomial_h

#include "doublevector.h"

class Multinomial;

class Multinomial {
public:
  Multinomial(double value):error(0), bigvalue(value), loglikelihood(0.0) {};
  ~Multinomial() {};
  double LogLikelihood(const DoubleVector& data, const DoubleVector& dist);
  int Error() { return error; };
  double ReturnLogLikelihood() const { return loglikelihood; };
protected:
  int error;
  double bigvalue;
  double loglikelihood;
};

#endif
