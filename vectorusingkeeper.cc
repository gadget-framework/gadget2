#include "keeper.h"
#include "doublevector.h"
#include "gadget.h"

//Pre: var is a nonempty doublevector, keeper != 0, 0 <= index and index < var.Size()
//Post: The element var[index] has been deleted from var, the size of
//var thus being 1 shorter than before. keeper has been informed of
//the possible changed locations of the elements of var.

void DeleteElementUsingKeeper(doublevector& var, Keeper* const keeper, int index) {
  assert(0 <= index && index < var.Size());
  keeper->DeleteParam(var[index]);
  doublevector tmp(var.Size() - 1);
  int i;
  for (i = 0; i < index; i++) {
    tmp[i] = var[i];
    keeper->ChangeVariable(var[i], tmp[i]);
  }
  for (i = index; i < tmp.Size(); i++) {
    tmp[i] = var[i + 1];
    keeper->ChangeVariable(var[i + 1], tmp[i]);
  }
  var.Delete(index);
  for (i = 0; i < var.Size(); i++)
    keeper->ChangeVariable(tmp[i], var[i]);
}
