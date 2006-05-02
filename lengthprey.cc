#include "lengthprey.h"

LengthPrey::LengthPrey(const DoubleVector& lengths, const IntVector& Areas,
  double Energy, const char* givenname) : Prey(lengths, Areas, Energy, givenname) {

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
