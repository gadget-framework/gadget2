#ifndef vectorusingkeeper_h
#define vectorusingkeeper_h

#include "keeper.h"

extern void DeleteElementUsingKeeper(doublevector& var,
  Keeper* const keeper, int index);
extern void ResizeVectorUsingKeeper(doublevector& var,
  Keeper* const keeper, int add, double initial = 0);

#endif
