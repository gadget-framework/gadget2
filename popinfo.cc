#include "popinfo.h"
#include "mathfunc.h"

PopInfo& PopInfo::operator = (const PopInfo& a) {
  N = a.N;
  W = a.W;
  return *this;
}

PopInfo& PopInfo::operator += (const PopInfo& a) {
  if (isZero(N + a.N)) {
    W = 0.0;
    N = 0.0;
  } else if (isZero(a.N)) {
    //adding a zero popinfo, so don't do anything
  } else if (isZero(N)) {
    W = a.W;
    N = a.N;
  } else {
    W = (N * W + a.N * a.W) / (N + a.N);
    N = N + a.N;
  }
  return *this;
}

PopInfo PopInfo::operator * (double a) {
  PopInfo c;
  c.N = N * a;
  c.W = W;
  return c;
}
