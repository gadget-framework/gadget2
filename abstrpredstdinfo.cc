#include "abstrpredstdinfo.h"

AbstrPredStdInfo::AbstrPredStdInfo(const intvector& Areas, int predminage,
  int predmaxage, int preyminage, int preymaxage) : LivesOnAreas(Areas) {
  intvector minage(predmaxage - predminage + 1, preyminage);
  intvector size(predmaxage - predminage + 1, preymaxage - preyminage + 1);
  bandmatrix bm(minage, size, predminage); //default initialization to 0.
  NconbyAge.resize(areas.Size(), bm);
  BconbyAge.resize(areas.Size(), bm);
  MortbyAge.resize(areas.Size(), bm);
}

AbstrPredStdInfo::~AbstrPredStdInfo() {
}

const bandmatrix& AbstrPredStdInfo::NconsumptionByAge(int area) const {
  return NconbyAge[AreaNr[area]];
}

const bandmatrix& AbstrPredStdInfo::BconsumptionByAge(int area) const {
  return BconbyAge[AreaNr[area]];
}

const bandmatrix& AbstrPredStdInfo::MortalityByAge(int area) const {
  return MortbyAge[AreaNr[area]];
}
