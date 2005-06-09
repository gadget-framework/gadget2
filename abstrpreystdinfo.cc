#include "prey.h"
#include "abstrpreystdinfo.h"
#include "areatime.h"

AbstrPreyStdInfo::AbstrPreyStdInfo(const Prey* prey, const IntVector& areas,
  int minage, int maxage) : LivesOnAreas(areas),
  NconbyAge(minage, maxage - minage + 1, 0, areas.Size(), 0.0),
  BconbyAge(minage, maxage - minage + 1, 0, areas.Size(), 0.0),
  MortbyAge(minage, maxage - minage + 1, 0, areas.Size(), 0.0) {

  BandMatrix bm(0, prey->getLengthGroupDiv()->numLengthGroups(), minage, maxage - minage + 1, 0.0);
  NconbyAgeAndLength.resize(areas.Size(), bm);
  BconbyAgeAndLength.resize(areas.Size(), bm);
  MortbyAgeAndLength.resize(areas.Size(), bm);
}

const BandMatrix& AbstrPreyStdInfo::NconsumptionByAgeAndLength(int area) const {
  return NconbyAgeAndLength[this->areaNum(area)];
}

const DoubleIndexVector& AbstrPreyStdInfo::NconsumptionByAge(int area) const {
  return NconbyAge[this->areaNum(area)];
}

const BandMatrix& AbstrPreyStdInfo::BconsumptionByAgeAndLength(int area) const {
  return BconbyAgeAndLength[this->areaNum(area)];
}

const DoubleIndexVector& AbstrPreyStdInfo::BconsumptionByAge(int area) const {
  return BconbyAge[this->areaNum(area)];
}

const BandMatrix& AbstrPreyStdInfo::MortalityByAgeAndLength(int area) const {
  return MortbyAgeAndLength[this->areaNum(area)];
}

const DoubleIndexVector& AbstrPreyStdInfo::MortalityByAge(int area) const {
  return MortbyAge[this->areaNum(area)];
}
