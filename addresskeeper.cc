#include "addresskeeper.h"

AddressKeeper::~AddressKeeper() {
  if (name != 0) {
    delete[] name;
    name = 0;
  }
}

// CVS $Id: addresskeeper.cc,v 1.4 2004-08-09 14:13:35 james Exp $
