#ifndef hasname_h
#define hasname_h

#include "gadget.h"

class HasName {
public:
  HasName() { name = new char[1]; name[0] = '\0'; };
  virtual ~HasName() { delete[] name; };
  HasName(const char* givenname) {
    name = new char[strlen(givenname) + 1];
    strcpy(name, givenname);
  };
  const char* Name() const { return name; };
private:
  char* name;
};

#endif
