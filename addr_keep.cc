#include "addr_keep.h"

addr_keep::~addr_keep() {
  if (name != 0) {
    delete[] name;
    name = 0;
  }
}

