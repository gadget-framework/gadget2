#ifndef runid_h
#define runid_h

#include "gadget.h"

class RunID {
public:
  RunID();
  void print(ostream& o);
  ~RunID();
protected:
  const char* hostname;
  const char* timestring;
  struct utsname host;
};

#endif
