#ifndef GADGET_INLINE
#include "popinfo.h"
#endif

#include "mathfunc.h"

#ifdef GADGET_INLINE
inline
#endif
PopInfo::PopInfo() {
  N = 0;
  W = 0;
}

#ifdef GADGET_INLINE
inline
#endif
PopInfo& PopInfo::operator = (const PopInfo& a) {
  N = a.N;
  W = a.W;
  return *this;
}

#ifdef GADGET_INLINE
inline
#endif
PopInfo& PopInfo::operator += (const PopInfo& a) {
  W = ((isZero(N + a.N)) ? 0 : (N * W + a.N * a.W) / (N + a.N));
  N = N + a.N;
  return *this;
}

#ifdef GADGET_INLINE
inline
#endif
void PopInfo::operator -= (double a) {
  N -= a;
}

#ifdef GADGET_INLINE
inline
#endif
void PopInfo::operator *= (double a) {
  N *= a;
}

#ifdef GADGET_INLINE
inline
#endif
PopInfo PopInfo::operator * (double b) {
  PopInfo c;
  c.N = N * b;
  c.W = W;
  return c;
}
