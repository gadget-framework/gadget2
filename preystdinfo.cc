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
  //We never have to change the values of NconByAge and NconByAgeAndLength,
  //since they are initialised to 0.
  PSIByLength.Sum(TimeInfo, area);
  int inarea = this->areaNum(area);
  int l, age;
  double timeratio;

  age = 0;
  BconbyAge[inarea][age] = 0.0;
  timeratio = TimeInfo->LengthOfYear() / TimeInfo->LengthOfCurrent();
  for (l = 0; l < prey->numLengthGroups(); l++) {
    BconbyAgeAndLength[inarea][age][l] = PSIByLength.BconsumptionByLength(area)[l];
    BconbyAge[inarea][age] += BconbyAgeAndLength[inarea][age][l];
    MortbyAgeAndLength[inarea][age][l] = PSIByLength.MortalityByLength(area)[l];
  }

  if (isZero(prey->Biomass(area)))
    MortbyAge[inarea][age] = 0.0;
  else if (BconbyAge[inarea][age] >= prey->Biomass(area))
    MortbyAge[inarea][age] = MaxMortality;
  else
    MortbyAge[inarea][age] = -log(1.0 - BconbyAge[inarea][age] / prey->Biomass(area)) * timeratio;
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
