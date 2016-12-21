#include "lengthprey.h"

LengthPrey::LengthPrey(CommentStream& infile, const char* givenname,
  const IntVector& Areas, const TimeClass* const TimeInfo, Keeper* const keeper)
  : Prey(infile, givenname, Areas, TimeInfo, keeper) {

  type = LENGTHPREY;
}

/* Sum number in Prey length groups. Prey length division is not
 * allowed to be finer than stock length division. */
void LengthPrey::Sum(const PopInfoVector& NumberInArea, int area) {
  int i, inarea = this->areaNum(area);
  preynumber[inarea].Sum(&NumberInArea, *CI);
  total[inarea] = 0.0;
  for (i = 0; i < preynumber.Ncol(inarea); i++) {
    biomass[inarea][i] = preynumber[inarea][i].N * preynumber[inarea][i].W;
    total[inarea] += biomass[inarea][i];
    cons[inarea][i] = 0.0;
  }
}
