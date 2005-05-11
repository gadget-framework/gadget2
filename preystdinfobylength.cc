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
  const DoubleVector& Bconsumption = prey->getConsumption(area);

  int l;
  double timeratio;

  timeratio = 1.0 / TimeInfo->getTimeStepSize();
  for (l = 0; l < BconbyLength.Ncol(inarea); l++) {
    BconbyLength[inarea][l] = Bconsumption[l];
    //NconbyLength[inarea][l] = 0.0;  //initialised to zero

    if (isZero(prey->getBiomass(area, l)))
      MortbyLength[inarea][l] = 0.0;
    else if (BconbyLength[inarea][l] >= prey->getBiomass(area, l))
      MortbyLength[inarea][l] = MaxMortality;
    else
      MortbyLength[inarea][l] = -log(1.0 - BconbyLength[inarea][l] / prey->getBiomass(area, l)) * timeratio;
  }
}
