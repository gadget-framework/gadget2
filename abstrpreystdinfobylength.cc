#include "prey.h"
#include "abstrpreystdinfobylength.h"
#include "areatime.h"
#include "gadget.h"

AbstrPreyStdInfoByLength::AbstrPreyStdInfoByLength(const Prey* prey,
  const intvector& Areas) : LivesOnAreas(Areas) {

  const int NoAreas = areas.Size();
  MortbyLength.AddRows(NoAreas, prey->NoLengthGroups(), 0);
  NconbyLength.AddRows(NoAreas, prey->NoLengthGroups(), 0);
  BconbyLength.AddRows(NoAreas, prey->NoLengthGroups(), 0);
}

AbstrPreyStdInfoByLength::~AbstrPreyStdInfoByLength() {
}

const doublevector& AbstrPreyStdInfoByLength::NconsumptionByLength(int area) const {
  return NconbyLength[AreaNr[area]];
}

const doublevector& AbstrPreyStdInfoByLength::BconsumptionByLength(int area) const {
  return BconbyLength[AreaNr[area]];
}

const doublevector& AbstrPreyStdInfoByLength::MortalityByLength(int area) const {
  return MortbyLength[AreaNr[area]];
}
