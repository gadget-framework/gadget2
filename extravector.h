#ifndef extravector_h
#define extravector_h

#include "popinfoindexvector.h"
#include "doubleindexvector.h"
#include "conversion.h"

extern void PopinfoAdd(popinfoindexvector& target, const popinfoindexvector& Addition,
  const ConversionIndex& CI, double ratio, const doubleindexvector& Ratio);

extern void PopinfoAdd(popinfoindexvector& target, const popinfoindexvector& Addition,
  const ConversionIndex& CI, double ratio = 1);

#endif
