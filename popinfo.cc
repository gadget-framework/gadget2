#include "popinfo.h"
#include "mathfunc.h"

PopInfo::PopInfo() {
  N = 0;
  W = 0;
}

PopInfo& PopInfo::operator = (const PopInfo& a) {
  N = a.N;
  W = a.W;
  return *this;
}

PopInfo& PopInfo::operator += (const PopInfo& a) {
  W = ((isZero(N + a.N)) ? 0 : (N * W + a.N * a.W) / (N + a.N));
  N = N + a.N;
  return *this;
}

void PopInfo::operator -= (double a) {
  N -= a;
}

void PopInfo::operator *= (double a) {
  N *= a;
}

PopInfo PopInfo::operator * (double b) {
  PopInfo c;
  c.N = N * b;
  c.W = W;
  return c;
}
