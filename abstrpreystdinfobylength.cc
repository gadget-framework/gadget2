#include "prey.h"
#include "abstrpreystdinfobylength.h"
#include "areatime.h"
#include "gadget.h"

AbstrPreyStdInfoByLength::AbstrPreyStdInfoByLength(const Prey* prey,
  const IntVector& Areas) : LivesOnAreas(Areas) {

  const int NoAreas = areas.Size();
  MortbyLength.AddRows(NoAreas, prey->NoLengthGroups(), 0.0);
  NconbyLength.AddRows(NoAreas, prey->NoLengthGroups(), 0.0);
  BconbyLength.AddRows(NoAreas, prey->NoLengthGroups(), 0.0);
}

AbstrPreyStdInfoByLength::~AbstrPreyStdInfoByLength() {
}

const DoubleVector& AbstrPreyStdInfoByLength::NconsumptionByLength(int area) const {
  return NconbyLength[AreaNr[area]];
}

const DoubleVector& AbstrPreyStdInfoByLength::BconsumptionByLength(int area) const {
  return BconbyLength[AreaNr[area]];
}

const DoubleVector& AbstrPreyStdInfoByLength::MortalityByLength(int area) const {
  return MortbyLength[AreaNr[area]];
}
