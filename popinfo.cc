#ifndef INLINE_POPINFO_CC
#include "popinfo.h"
#endif

#include "mathfunc.h"

#ifdef INLINE_POPINFO_CC
inline
#endif
popinfo::popinfo() {
  N = 0;
  W = 0;
}

#ifdef INLINE_POPINFO_CC
inline
#endif
popinfo& popinfo::operator = (const popinfo& a) {
  N = a.N;
  W = a.W;
  return *this;
}

#ifdef INLINE_POPINFO_CC
inline
#endif
popinfo& popinfo::operator += (const popinfo& a) {
  W = ((iszero(N + a.N)) ? 0 : (N * W + a.N * a.W) / (N + a.N));
  N = N + a.N;
  return *this;
}

#ifdef INLINE_POPINFO_CC
inline
#endif
void popinfo::operator -= (double a) {
  N -= a;
}

#ifdef INLINE_POPINFO_CC
inline
#endif
void popinfo::operator *= (double a) {
  N *= a;
}

#ifdef INLINE_POPINFO_CC
inline
#endif
popinfo popinfo::operator * (double b) {
  popinfo c;
  c.N = N * b;
  c.W = W;
  return c;
}
