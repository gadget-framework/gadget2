#include "mathfunc.h"

//function to find the logarithm of n factorial
double logFactorial(double n) {
  return lgamma(n + 1);
}
