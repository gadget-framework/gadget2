#ifndef timevariablevector_h
#define timevariablevector_h

#include "timevariable.h"

class TimeVariablevector {
public:
  TimeVariablevector() : size(0), v(0) {};
  TimeVariablevector (int sz)
    : size(sz > 0 ? sz : 0), v(sz > 0 ? new TimeVariable[size] : 0) {};
  ~TimeVariablevector() { delete[] v; };
  void resize(int size);
  void resize(int size, Keeper* const keeper);
  void resize(const TimeVariable& tvar, Keeper* const keeper);
  int Size() const { return(size); };
  void Read(CommentStream& infile, const TimeClass* const TimeInfo, Keeper* const keeper);
  TimeVariable& operator [] (int pos);
  const TimeVariable& operator [] (int pos) const;
  int DidChange(const TimeClass* const TimeInfo) const;
  void Update(const TimeClass* const TimeInfo);
protected:
  int size;
  TimeVariable* v;
};

#ifdef INLINE_VECTORS
#include "timevariablevector.icc"
#endif

#endif
