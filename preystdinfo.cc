#include "prey.h"
#include "preystdinfo.h"
#include "areatime.h"
#include "gadget.h"

PreyStdInfo::PreyStdInfo(const Prey* p, const IntVector& Areas)
  : AbstrPreyStdInfo(p, Areas, 0, 0), //set minage and maxage to 0.
  PSIByLength(p, Areas), prey(p) {
}

PreyStdInfo::~PreyStdInfo() {
}

void PreyStdInfo::Sum(const TimeClass* const TimeInfo, int area) {

  int l;
  int inarea = this->areaNum(area);
  double timeratio = 1.0 / TimeInfo->getTimeStepSize();

  BconbyAge[inarea][0] = 0.0;
  PSIByLength.Sum(TimeInfo, area);
  for (l = 0; l < prey->numLengthGroups(); l++) {
    BconbyAgeAndLength[inarea][0][l] = PSIByLength.BconsumptionByLength(area)[l];
    BconbyAge[inarea][0] += BconbyAgeAndLength[inarea][0][l];
    MortbyAgeAndLength[inarea][0][l] = PSIByLength.MortalityByLength(area)[l];
  }

  if (isZero(prey->getTotalBiomass(area)))
    MortbyAge[inarea][0] = 0.0;
  else if (BconbyAge[inarea][0] >= prey->getTotalBiomass(area))
    MortbyAge[inarea][0] = MaxMortality;
  else
    MortbyAge[inarea][0] = -log(1.0 - BconbyAge[inarea][0] / prey->getTotalBiomass(area)) * timeratio;
}

const DoubleVector& PreyStdInfo::NconsumptionByLength(int area) const {
  return PSIByLength.NconsumptionByLength(area);
}

const DoubleVector& PreyStdInfo::BconsumptionByLength(int area) const {
  return PSIByLength.BconsumptionByLength(area);
}

const DoubleVector& PreyStdInfo::MortalityByLength(int area) const {
  return PSIByLength.MortalityByLength(area);
}
