#include "addresskeeper.h"

AddressKeeper::~AddressKeeper() {
  if (name != 0) {
    delete[] name;
    name = 0;
  }
}

// anotehr completely random comment

// CVS $Id: addresskeeper.cc,v 1.5 2004-08-09 14:16:12 james Exp $
