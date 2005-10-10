#ifndef mathfunc_h
#define mathfunc_h

#include "gadget.h"

/**
 * \brief This is the value of pi used in the simulation
 */
const double pivalue = atan(1.0) * 4.0;

/**
 * \brief This function will calculate the maximum of 2 numbers
 * \param a is the first number
 * \param b is the second number
 * \return a if a is greater than b, b otherwise
 */
inline int max(int a, int b) {
  return ((a > b) ? a : b);
}

/**
 * \brief This function will calculate the maximum of 2 numbers
 * \param a is the first number
 * \param b is the second number
 * \return a if a is greater than b, b otherwise
 */
inline double max(double a, double b) {
  return ((a > b) ? a : b);
}

/**
 * \brief This function will calculate the maximum of 3 numbers
 * \param a is the first number
 * \param b is the second number
 * \param c is the third number
 * \return a if a is greater than b and c, b if b is greater than a and c, c otherwise
 */
inline int max(int a, int b, int c) {
  int d = ((a > b) ? a : b);
  return ((c > d) ? c : d);
}

/**
 * \brief This function will calculate the minimum of 2 numbers
 * \param a is the first number
 * \param b is the second number
 * \return a if a is less than b, b otherwise
 */
inline int min(int a, int b) {
  return ((a < b) ? a : b);
}

/**
 * \brief This function will calculate the minimum of 2 numbers
 * \param a is the first number
 * \param b is the second number
 * \return a if a is less than b, b otherwise
 */
inline double min(double a, double b) {
  return ((a < b) ? a : b);
}

/**
 * \brief This function will calculate the minimum of 3 numbers
 * \param a is the first number
 * \param b is the second number
 * \param c is the third number
 * \return a if a is less than b and c, b if b is less than a and c, c otherwise
 */
inline int min(int a, int b, int c) {
  int d = ((a < b) ? a : b);
  return ((c < d) ? c : d);
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
 * \brief This function will calculate the value of the logarithm of n factorial
 * \param n is the number that the logfactorial will be calculated for
 * \return the value of log (n!)
 * \note This function is not an ANSI function and so will not compile correctly if gadget is compiled with the -ansi flag, unless we include #define _GNU_SOURCE in this file
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
  double exprep = 0.0;
  if (n > verysmall)
    exprep = 1.0;
  else if (n < -25.0)
    exprep = rathersmall;
  else
    exprep = exp(n);
  return exprep;
}

/**
 * \brief This function will generate a random number in the range 0.0 to 1.0
 * \return random number
 * \note This function generates uniformly-distributed doubles in the range 0.0 to 1.0
 */
inline double randomNumber() {
  int r = rand();
  double k = r % 32767;
  return k / 32767.0;
}

#endif
