#ifndef mathfunc_h
#define mathfunc_h

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
inline int isZero(double a) {
  return ((absolute(a) < verysmall) ? 1 : 0);
}

//-------------------------------------------------------
//Non ANSI functions
//The function lgamma() is not an ansi function and os is
//not included if gadget is compiled with the -ansi option
//unless the following is included (at the opt of this file)
//#define _BSD_SOURCE (or _GNU_SOURCE or XOPEN_SOURCE)

//Calculate the logarithm of n factorial
inline double logFactorial(double n) {
  return lgamma(n + 1);
}

#endif
