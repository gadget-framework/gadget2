#ifndef mathfunc_h
#define mathfunc_h

#include "gadget.h"

/**
 * \brief This is the value of pi used in the simulation
 */
const double pivalue = atan(1.0) * 4.0;

/**
 * \brief This function will calculate the maximum of 2 numbers (integers)
 * \param a is the first number
 * \param b is the second number
 * \return a if a is greater than b, b otherwise
 */
inline int max(int a, int b) {
  return ((a > b) ? a : b);
}

/**
 * \brief This function will calculate the maximum of 2 numbers (doubles)
 * \param a is the first number
 * \param b is the second number
 * \return a if a is greater than b, b otherwise
 */
inline double max(double a, double b) {
  return ((a > b) ? a : b);
}

/**
 * \brief This function will calculate the minimum of 2 numbers (integers)
 * \param a is the first number
 * \param b is the second number
 * \return a if a is less than b, b otherwise
 */
inline int min(int a, int b) {
  return ((a < b) ? a : b);
}

/**
 * \brief This function will calculate the minimum of 2 numbers (doubles)
 * \param a is the first number
 * \param b is the second number
 * \return a if a is less than b, b otherwise
 */
inline double min(double a, double b) {
  return ((a < b) ? a : b);
}

/**
 * \brief This function will check to see if a number is very close to zero
 * \param a is the number that is being checked
 * \return 1 if the number is very close to zero, 0 otherwise
 * \note This function replaces 'a == 0' to take account of numerical inaccuracies when calculating the exact value of a double
 */
inline int isZero(double a) {
  return ((fabs(a) < verysmall) ? 1 : 0);
}

/**
 * \brief This function will check to see if two numbers are equal
 * \param a is the first number that is being checked
 * \param b is the second number that is being checked
 * \return 1 if the two numbers are equal, 0 otherwise
 * \note This function replaces 'a == b' to take account of numerical inaccuracies when calculating the exact value of a double
 */
inline int isEqual(double a, double b) {
  return ((fabs(a - b) < verysmall) ? 1 : 0);
}

/**
 * \brief This function will check to see if a number is rather small (or zero)
 * \param a is the number that is being checked
 * \return 1 if the number is rather small, 0 otherwise
 */
inline int isSmall(double a) {
  return ((fabs(a) < rathersmall) ? 1 : 0);
}

/**
 * \brief This function will calculate the value of the logarithm of n factorial
 * \param n is the number that the logfactorial will be calculated for
 * \return the value of log(n!)
 * \note This function is not an ANSI function and so will not compile correctly if gadget is compiled with the \c -ansi flag, unless we include \c \#define \c __GNU_SOURCE in this file
 */
inline double logFactorial(double n) {
  return lgamma(n + 1.0);
}

/**
 * \brief This function will calculate the value of the exponential of n
 * \param n is the number that the exponential will be calculated for
 * \return the value of exp (n)
 * \note This function replaces 'exp' to return a value in the range 0.0 to 1.0
 */
inline double expRep(double n) {
  if (n > verysmall)
    return 1.0;
  else if (n < -25.0)
    return rathersmall;
  return exp(n);
}

/**
 * \brief This function will generate a random number in the range 0.0 to 1.0
 * \return random number
 * \note This function generates uniformly-distributed doubles in the range 0.0 to 1.0
 */
inline double randomNumber() {
  int r = rand();
  double k = r % 32767;
  return (k / 32767.0);
}

/**
 * \brief This function will calculate the calculate the effective annual mortality caused by a given predation on a specified population during a timestep
 * \param pred is the number that is removed from the population by the predation
 * \param pop is the population size (before predation)
 * \param t is the inverse of proportion of the year covered by the current timestep
 * \return calculated mortality
 */
inline double calcMortality(double pred, double pop, double t) {
  if (pred < verysmall)
    return 0.0;
  else if (pred > pop)
    return verybig;
  return (-log(1.0 - (pred / pop)) * t);
}

#endif
