#include "addresskeeper.h"

AddressKeeper::~AddressKeeper() {
  if (name != 0) {
    delete[] name;
    name = 0;
  }
}

// CVS $Id: addresskeeper.cc,v 1.2 2004-08-09 14:10:36 james Exp $
