#ifndef popinfo_h
#define popinfo_h

//Class popinfo stores Number and Mean weights.
class popinfo {
public:
  double N;
  double W;
  #ifdef INLINE_POPINFO_CC
  inline
  #endif
  popinfo();
  #ifdef INLINE_POPINFO_CC
  inline
  #endif
  popinfo& operator += (const popinfo& a);
  #ifdef INLINE_POPINFO_CC
  inline
  #endif
  void operator -= (double a);
  #ifdef INLINE_POPINFO_CC
  inline
  #endif
  void operator *= (double a);
  #ifdef INLINE_POPINFO_CC
  inline
  #endif
  popinfo operator * (double b);
  #ifdef INLINE_POPINFO_CC
  inline
  #endif
  popinfo& operator = (const popinfo& a);
};

#ifdef INLINE_POPINFO_CC
#include "popinfo.cc"
#endif

#endif
