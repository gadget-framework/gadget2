#include "addresskeeper.h"

AddressKeeper::~AddressKeeper() {
  if (name != 0) {
    delete[] name;
    name = 0;
  }
}

