#ifndef strstack_h
#define strstack_h

#include "charptrvector.h"

class StrStack {
  int sz;
  CharPtrVector v;
public:
  StrStack();
  ~StrStack();
  void OutOfStack();
  void PutInStack(const char*);
  void ClearStack();
  char* SendAll() const;
  char* SendTop() const;
  char* Send(int i) const;
};

#endif
