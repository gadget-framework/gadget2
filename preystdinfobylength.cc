#include "prey.h"
#include "preystdinfobylength.h"
#include "areatime.h"
#include "gadget.h"

PreyStdInfoByLength::PreyStdInfoByLength(const Prey* p, const IntVector& Areas)
  : AbstrPreyStdInfoByLength(p, Areas), prey(p) {
}

PreyStdInfoByLength::~PreyStdInfoByLength() {
}

void PreyStdInfoByLength::Sum(const TimeClass* const TimeInfo, int area) {
  const int inarea = AreaNr[area];
  const DoubleVector& Bconsumption = prey->Bconsumption(area);
  assert(BconbyLength.Ncol() == Bconsumption.Size());
  int l;
  for (l = 0; l < BconbyLength.Ncol(inarea); l++) {
    BconbyLength[inarea][l] = Bconsumption[l];
    NconbyLength[inarea][l] = 0;
    if (iszero(prey->Biomass(area, l)))
      MortbyLength[inarea][l] = 0;
    else if (prey->Biomass(area, l) <= BconbyLength[inarea][l])
      MortbyLength[inarea][l] = MAX_MORTALITY;
    else
      MortbyLength[inarea][l] = -log(1 - BconbyLength[inarea][l] /
        prey->Biomass(area, l)) * TimeInfo->LengthOfYear() /
        TimeInfo->LengthOfCurrent();
  }
}
