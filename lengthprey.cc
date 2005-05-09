#include "lengthprey.h"

LengthPrey::LengthPrey(const DoubleVector& lengths, const IntVector& Areas,
  double Energy, const char* givenname) : Prey(lengths, Areas, Energy, givenname) {

  type = LENGTHPREYTYPE;
}

/* Sum number in Prey length groups. Prey length division is not
 * allowed to be finer than stock length division. */
void LengthPrey::Sum(const PopInfoVector& NumberInArea, int area, int CurrentSubstep) {
  int i, inarea = this->areaNum(area);
  tooMuchConsumption[inarea] = 0;

  for (i = 0; i < cons.Ncol(inarea); i++)
    cons[inarea][i] = 0.0;
  for (i = 0; i < Number[inarea].Size(); i++)
    Number[inarea][i].N = 0.0;

  Number[inarea].Sum(&NumberInArea, *CI);

  PopInfo sum;
  for (i = 0; i < Number.Ncol(inarea); i++) {
    sum += Number[inarea][i];
    biomass[inarea][i] = Number[inarea][i].N * Number[inarea][i].W;
  }
  total[inarea] = sum.N * sum.W;

  if (CurrentSubstep == 1) {
    for (i = 0; i < consumption.Ncol(inarea); i++) {
      consumption[inarea][i] = 0.0;
      overconsumption[inarea][i] = 0.0;
    }
    for (i = 0; i < Number[inarea].Size(); i++)
      numberPriorToEating[inarea][i] = Number[inarea][i];
  }
}
