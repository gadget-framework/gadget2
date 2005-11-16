#include "lengthprey.h"

LengthPrey::LengthPrey(const DoubleVector& lengths, const IntVector& Areas,
  double Energy, const char* givenname) : Prey(lengths, Areas, Energy, givenname) {

  type = LENGTHPREYTYPE;
}

/* Sum number in Prey length groups. Prey length division is not
 * allowed to be finer than stock length division. */
void LengthPrey::Sum(const PopInfoVector& NumberInArea, int area) {
  PopInfo sum;
  int i, inarea = this->areaNum(area);

  preynumber[inarea].Sum(&NumberInArea, *CI);
  for (i = 0; i < preynumber.Ncol(inarea); i++) {
    sum += preynumber[inarea][i];
    biomass[inarea][i] = preynumber[inarea][i].N * preynumber[inarea][i].W;
    cons[inarea][i] = 0.0;
  }
  total[inarea] = sum.N * sum.W;
}
