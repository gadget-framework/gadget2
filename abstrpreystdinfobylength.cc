#include "prey.h"
#include "abstrpreystdinfobylength.h"
#include "areatime.h"
#include "gadget.h"

AbstrPreyStdInfoByLength::AbstrPreyStdInfoByLength(const Prey* prey,
  const IntVector& areas) : LivesOnAreas(areas) {

  MortbyLength.AddRows(areas.Size(), prey->getLengthGroupDiv()->numLengthGroups(), 0.0);
  NconbyLength.AddRows(areas.Size(), prey->getLengthGroupDiv()->numLengthGroups(), 0.0);
  BconbyLength.AddRows(areas.Size(), prey->getLengthGroupDiv()->numLengthGroups(), 0.0);
}

const DoubleVector& AbstrPreyStdInfoByLength::NconsumptionByLength(int area) const {
  return NconbyLength[this->areaNum(area)];
}

const DoubleVector& AbstrPreyStdInfoByLength::BconsumptionByLength(int area) const {
  return BconbyLength[this->areaNum(area)];
}

const DoubleVector& AbstrPreyStdInfoByLength::MortalityByLength(int area) const {
  return MortbyLength[this->areaNum(area)];
}
