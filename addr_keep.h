#ifndef addr_keep_h
#define addr_keep_h

#include "gadget.h"

class addr_keep {
public:
  double* addr;
  char* name;
  addr_keep() { addr = 0; name = 0; };
  ~addr_keep();
  addr_keep(const addr_keep& a)
    : addr(a.addr), name(new char[strlen(a.name) + 1])
    { strcpy(name, a.name); };
  void operator = (double* address) { addr = address; };
  void operator = (double& value) { addr = &value; };
  int operator == (const double& val) { return (addr == &val); };
  int operator == (const double* add) { return (addr == add); };
  void operator = (const char* str1) {
    if (name != 0)
      delete[] name;
    name = new char[strlen(str1) + 1];
    strcpy(name, str1);
    delete[] str1;  //JMB - free memory from strstack->sendall()
  };
  void operator = (const addr_keep a) {
    if (name != 0)
      delete[] name;
    name = new char[strlen(a.name) + 1];
    strcpy(name, a.name);
    addr = a.addr;
  };
protected:
};

#endif
