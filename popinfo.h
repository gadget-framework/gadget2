#ifndef popinfo_h
#define popinfo_h

//Class popinfo stores Number and Mean weights.
class popinfo {
public:
  double N;
  double W;
  #ifdef GADGET_INLINE
  inline
  #endif
  popinfo();
  ~popinfo() {};
  #ifdef GADGET_INLINE
  inline
  #endif
  popinfo& operator += (const popinfo& a);
  #ifdef GADGET_INLINE
  inline
  #endif
  void operator -= (double a);
  #ifdef GADGET_INLINE
  inline
  #endif
  void operator *= (double a);
  #ifdef GADGET_INLINE
  inline
  #endif
  popinfo operator * (double b);
  #ifdef GADGET_INLINE
  inline
  #endif
  popinfo& operator = (const popinfo& a);
};

#ifdef GADGET_INLINE
#include "popinfo.cc"
#endif

#endif
