#include "popinfo.h"
#include "mathfunc.h"

PopInfo::PopInfo() {
  N = 0.0;
  W = 0.0;
}

PopInfo& PopInfo::operator = (const PopInfo& a) {
  N = a.N;
  W = a.W;
  return *this;
}

PopInfo& PopInfo::operator += (const PopInfo& a) {
  if (isZero(N + a.N)) {
    W = 0.0;
    N = 0.0;
  } else {
    W = (N * W + a.N * a.W) / (N + a.N);
    N = N + a.N;
  }
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
