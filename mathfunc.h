#ifndef mathfunc_h
#define mathfunc_h

/* This file uses the non-ansi function lgamma, which is not included when compiling
   with -ansi. To compile with -ansi, and still use the lgamma function, you can for example
   #define _BSD_SOURCE at the top of this file (or _GNU_SOURCE, or XOPEN_SOURCE)
   [AJ&MNAA 15.05.01]
*/

#include "gadget.h"

//-------------------------------------------------------
//Maximum of two numbers
inline int max(int a, int b) {
  return ((a >= b) ? a : b);
}

inline double max(double a, double b) {
  return ((a >= b) ? a : b);
}

//Maximum of three numbers
inline int max(int a, int b, int c) {
  int d = ((a >= b) ? a : b);
  return ((c >= d) ? c : d);
}

//-------------------------------------------------------
//Minimum of two numbers
inline int min(int a, int b) {
  return ((a < b) ? a : b);
}

inline double min(double a, double b) {
  return ((a < b) ? a : b);
}

//Minimum of three numbers
inline int min(int a, int b, int c) {
  int d = ((a < b) ? a : b);
  return ((c < d) ? c : d);
}

//-------------------------------------------------------
//Absolute value of a number
inline double absolute(double a) {
  return ((a < 0) ? -a : a);
}

//-------------------------------------------------------
//Checks to see if a double is very close to zero
inline int iszero(double a) {
  return ((absolute(a) < verysmall) ? 1 : 0);
}

//-------------------------------------------------------
//Calculates the dnorm value, using the mean and std dev
inline double dnorm(double length, double mean, double sdev) {
  double t = (length - mean) / sdev;
  return exp(-(t * t) * 0.5);
}

//-------------------------------------------------------
//Non ANSI functions
double logfactorial(double n);

inline double logGamma(double x) {
  return lgamma(x);
}

#endif
