#include "prey.h"
#include "preystdinfobylength.h"
#include "areatime.h"
#include "gadget.h"

PreyStdInfoByLength::PreyStdInfoByLength(const Prey* p, const IntVector& Areas)
  : AbstrPreyStdInfoByLength(p, Areas), prey(p) {
}

PreyStdInfoByLength::~PreyStdInfoByLength() {
}

void PreyStdInfoByLength::Sum(const TimeClass* const TimeInfo, int area) {
  int inarea = this->areaNum(area);
  const DoubleVector& cons = prey->getConsumption(area);
  const DoubleVector& bio = prey->getBiomass(area);

  int l;
  double timeratio = 1.0 / TimeInfo->getTimeStepSize();
  for (l = 0; l < cons.Size(); l++) {
    BconbyLength[inarea][l] = cons[l];

    if (isZero(bio[l]))
      MortbyLength[inarea][l] = 0.0;
    else if (cons[l] >= bio[l])
      MortbyLength[inarea][l] = MaxMortality;
    else
      MortbyLength[inarea][l] = -log(1.0 - cons[l] / bio[l]) * timeratio;
  }
}
