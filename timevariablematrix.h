#ifndef timevariablematrix_h
#define timevariablematrix_h

#include "timevariablevector.h"

class TimeVariablematrix {
public:
  TimeVariablematrix() { nrow = 0; v = 0; };
  ~TimeVariablematrix();
  int Ncol(int i = 0) const { return v[i]->Size(); };
  int Nrow() const { return nrow; };
  TimeVariablevector& operator [] (int pos);
  const TimeVariablevector& operator [] (int pos) const;
  void AddRows(const TimeVariablevector& tvar, Keeper* const keeper);
  void DeleteRow(int row, Keeper* const keeper);
protected:
  int nrow;
  TimeVariablevector** v;
};

#ifdef GADGET_INLINE
#include "timevariablematrix.icc"
#endif

#endif
