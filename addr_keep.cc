#include "addr_keep.h"

//Of course this simple destructor could be in the header file, but
//the silly Solaris SparcWorks C++ compiler (v. 3.0) can't inline the
//destructor, therefore it is out-of-line.

addr_keep::~addr_keep() {
  delete[] name;
}

