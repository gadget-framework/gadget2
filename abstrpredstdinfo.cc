#include "abstrpredstdinfo.h"

AbstrPredStdInfo::AbstrPredStdInfo(const IntVector& Areas, int predminage,
  int predmaxage, int preyminage, int preymaxage) : LivesOnAreas(Areas) {
  IntVector minage(predmaxage - predminage + 1, preyminage);
  IntVector size(predmaxage - predminage + 1, preymaxage - preyminage + 1);
  BandMatrix bm(minage, size, predminage); //default initialization to 0.
  NconbyAge.resize(areas.Size(), bm);
  BconbyAge.resize(areas.Size(), bm);
  MortbyAge.resize(areas.Size(), bm);
}

AbstrPredStdInfo::~AbstrPredStdInfo() {
}

const BandMatrix& AbstrPredStdInfo::NconsumptionByAge(int area) const {
  return NconbyAge[AreaNr[area]];
}

const BandMatrix& AbstrPredStdInfo::BconsumptionByAge(int area) const {
  return BconbyAge[AreaNr[area]];
}

const BandMatrix& AbstrPredStdInfo::MortalityByAge(int area) const {
  return MortbyAge[AreaNr[area]];
}
