#ifndef extravector_h
#define extravector_h

#include "popinfoindexvector.h"
#include "doubleindexvector.h"
#include "conversion.h"

extern void PopinfoAdd(PopInfoIndexVector& target, const PopInfoIndexVector& Addition,
  const ConversionIndex& CI, double ratio, const DoubleIndexVector& Ratio);

extern void PopinfoAdd(PopInfoIndexVector& target, const PopInfoIndexVector& Addition,
  const ConversionIndex& CI, double ratio = 1);

#endif
