#ifndef timevariableindexvector_h
#define timevariableindexvector_h

#include "timevariable.h"

class TimeVariableIndexVector {
public:
  TimeVariableIndexVector() : size(0), minpos(0), v(0) {};
  TimeVariableIndexVector(int sz, int minp)
    : size(sz > 0 ? sz : 0), minpos(minp), v(sz > 0 ? new TimeVariable[size] : 0) {};
  ~TimeVariableIndexVector() { delete[] v; };
  void resize(int size, int newminpos, Keeper* const keeper);
  void resize(const TimeVariable& tvar, int newminpos, Keeper* const keeper);
  int Size() const { return(size); };
  void Read(CommentStream& infile, const TimeClass* const TimeInfo, Keeper* const keeper);
  TimeVariable& operator [] (int pos);
  const TimeVariable& operator [] (int pos) const;
  int DidChange(const TimeClass* const TimeInfo) const;
  void Update(const TimeClass* const TimeInfo);
  int Mincol() const { return(minpos); };
  int Maxcol() const { return(minpos + size); };
protected:
  int size;
  int minpos;
  TimeVariable* v;
};

#ifdef INLINE_VECTORS
#include "timevariableindexvector.icc"
#endif

#endif
