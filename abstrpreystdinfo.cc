#include "prey.h"
#include "abstrpreystdinfo.h"
#include "areatime.h"

AbstrPreyStdInfo::AbstrPreyStdInfo(const Prey* prey, const IntVector& Areas,
  int minage, int maxage) : LivesOnAreas(Areas),
  NconbyAge(minage, maxage - minage + 1, 0, Areas.Size(), 0),
  BconbyAge(minage, maxage - minage + 1, 0, Areas.Size(), 0),
  MortbyAge(minage, maxage - minage + 1, 0, Areas.Size(), 0) {

  const int NoAreas = areas.Size();
  BandMatrix bm(0, prey->NoLengthGroups(), minage, maxage - minage + 1, 0);
  NconbyAgeAndLength.resize(NoAreas, bm);
  BconbyAgeAndLength.resize(NoAreas, bm);
  MortbyAgeAndLength.resize(NoAreas, bm);
}

const BandMatrix& AbstrPreyStdInfo::NconsumptionByAgeAndLength(int area) const {
  return NconbyAgeAndLength[AreaNr[area]];
}

const DoubleIndexVector& AbstrPreyStdInfo::NconsumptionByAge(int area) const {
  return NconbyAge[AreaNr[area]];
}

const BandMatrix& AbstrPreyStdInfo::BconsumptionByAgeAndLength(int area) const {
  return BconbyAgeAndLength[AreaNr[area]];
}

const DoubleIndexVector& AbstrPreyStdInfo::BconsumptionByAge(int area) const {
  return BconbyAge[AreaNr[area]];
}

const BandMatrix& AbstrPreyStdInfo::MortalityByAgeAndLength(int area) const {
  return MortbyAgeAndLength[AreaNr[area]];
}

const DoubleIndexVector& AbstrPreyStdInfo::MortalityByAge(int area) const {
  return MortbyAge[AreaNr[area]];
}
