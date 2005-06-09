#include "prey.h"
#include "preystdinfobylength.h"
#include "areatime.h"
#include "gadget.h"

PreyStdInfoByLength::PreyStdInfoByLength(const Prey* p, const IntVector& Areas)
  : AbstrPreyStdInfoByLength(p, Areas), prey(p) {
}

void PreyStdInfoByLength::Sum(const TimeClass* const TimeInfo, int area) {
  int inarea = this->areaNum(area);
  const DoubleVector& cons = prey->getConsumption(area);
  const PopInfoVector& pop = prey->getNumberPriorToEating(area);

  int l;
  double bio;
  double timeratio = 1.0 / TimeInfo->getTimeStepSize();
  for (l = 0; l < cons.Size(); l++) {
    bio = pop[l].W * pop[l].N;
    BconbyLength[inarea][l] = cons[l];
    if (isZero(bio))
      MortbyLength[inarea][l] = 0.0;
    else if (cons[l] >= bio)
      MortbyLength[inarea][l] = MaxMortality;
    else
      MortbyLength[inarea][l] = -log(1.0 - cons[l] / bio) * timeratio;
  }
}
