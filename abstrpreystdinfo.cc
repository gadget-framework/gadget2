#include "prey.h"
#include "abstrpreystdinfo.h"
#include "areatime.h"

AbstrPreyStdInfo::AbstrPreyStdInfo(const Prey* prey, const intvector& Areas,
  int minage, int maxage) : LivesOnAreas(Areas),
  NconbyAge(minage, maxage - minage + 1, 0, Areas.Size(), 0),
  BconbyAge(minage, maxage - minage + 1, 0, Areas.Size(), 0),
  MortbyAge(minage, maxage - minage + 1, 0, Areas.Size(), 0) {

  const int NoAreas = areas.Size();
  bandmatrix bm(0, prey->NoLengthGroups(), minage, maxage - minage + 1, 0);
  NconbyAgeAndLength.resize(NoAreas, bm);
  BconbyAgeAndLength.resize(NoAreas, bm);
  MortbyAgeAndLength.resize(NoAreas, bm);
}

AbstrPreyStdInfo::~AbstrPreyStdInfo() {
}

const bandmatrix& AbstrPreyStdInfo::NconsumptionByAgeAndLength(int area) const {
  return NconbyAgeAndLength[AreaNr[area]];
}

const doubleindexvector& AbstrPreyStdInfo::NconsumptionByAge(int area) const {
  return NconbyAge[AreaNr[area]];
}

const bandmatrix& AbstrPreyStdInfo::BconsumptionByAgeAndLength(int area) const {
  return BconbyAgeAndLength[AreaNr[area]];
}

const doubleindexvector& AbstrPreyStdInfo::BconsumptionByAge(int area) const {
  return BconbyAge[AreaNr[area]];
}

const bandmatrix& AbstrPreyStdInfo::MortalityByAgeAndLength(int area) const {
  return MortbyAgeAndLength[AreaNr[area]];
}

const doubleindexvector& AbstrPreyStdInfo::MortalityByAge(int area) const {
  return MortbyAge[AreaNr[area]];
}
