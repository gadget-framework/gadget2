#include "addresskeeper.h"

AddressKeeper::~AddressKeeper() {
  if (name != 0) {
    delete[] name;
    name = 0;
  }
}

//some random comment

// CVS $Id: addresskeeper.cc,v 1.3 2004-08-09 14:12:30 james Exp $
