#ifndef multinomial_h
#define multinomial_h

#include "doublevector.h"

class Multinomial;

class Multinomial {
public:
  Multinomial(int value):BIGVALUE(value), error(0), loglikelihood(0.0) {};
  ~Multinomial() {};
  double LogLikelihood(const doublevector& data, const doublevector& dist);
  int Error() { return error; };
  double ReturnLogLikelihood() const { return loglikelihood; };
protected:
  int BIGVALUE;
  int error;
  double loglikelihood;
};

#endif
