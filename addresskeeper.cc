#include "addresskeeper.h"

AddressKeeper::~AddressKeeper() {
  if (name != 0) {
    delete[] name;
    name = 0;
  }
}

// CVS $Id: addresskeeper.cc,v 1.6 2004-08-09 14:16:43 james Exp $
