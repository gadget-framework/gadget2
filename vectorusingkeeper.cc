#include "keeper.h"
#include "doublevector.h"
#include "gadget.h"

//Pre: var is a nonempty DoubleVector, keeper != 0, 0 <= index and index < var.Size()
//Post: The element var[index] has been deleted from var, the size of
//var thus being 1 shorter than before. keeper has been informed of
//the possible changed locations of the elements of var.

void DeleteElementUsingKeeper(DoubleVector& var, Keeper* const keeper, int index) {
  assert(0 <= index && index < var.Size());
  keeper->deleteParameter(var[index]);
  DoubleVector tmp(var.Size() - 1);
  int i;
  for (i = 0; i < index; i++) {
    tmp[i] = var[i];
    keeper->changeVariable(var[i], tmp[i]);
  }
  for (i = index; i < tmp.Size(); i++) {
    tmp[i] = var[i + 1];
    keeper->changeVariable(var[i + 1], tmp[i]);
  }
  var.Delete(index);
  for (i = 0; i < var.Size(); i++)
    keeper->changeVariable(tmp[i], var[i]);
}
