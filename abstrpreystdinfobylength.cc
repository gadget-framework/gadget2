#include "prey.h"
#include "abstrpreystdinfobylength.h"
#include "areatime.h"
#include "gadget.h"

AbstrPreyStdInfoByLength::AbstrPreyStdInfoByLength(const Prey* prey,
  const IntVector& Areas) : LivesOnAreas(Areas) {

  MortbyLength.AddRows(areas.Size(), prey->numLengthGroups(), 0.0);
  NconbyLength.AddRows(areas.Size(), prey->numLengthGroups(), 0.0);
  BconbyLength.AddRows(areas.Size(), prey->numLengthGroups(), 0.0);
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
