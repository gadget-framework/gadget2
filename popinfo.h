#ifndef popinfo_h
#define popinfo_h

//Class PopInfo stores Number and Mean weights.
class PopInfo {
public:
  double N;
  double W;
  #ifdef GADGET_INLINE
  inline
  #endif
  PopInfo();
  ~PopInfo() {};
  #ifdef GADGET_INLINE
  inline
  #endif
  PopInfo& operator += (const PopInfo& a);
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
  PopInfo operator * (double b);
  #ifdef GADGET_INLINE
  inline
  #endif
  PopInfo& operator = (const PopInfo& a);
};

#ifdef GADGET_INLINE
#include "popinfo.cc"
#endif

#endif
