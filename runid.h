#ifndef runid_h
#define runid_h

#include "gadget.h"

class RunId {
public:
  RunId();
  void print(ostream& o);
  ~RunId();
protected:
  const char* hostname;
  const char* timestring;
  struct utsname host;
};

#endif
