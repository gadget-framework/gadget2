#include "stockpreystdinfo.h"
#include "intvector.h"
#include "stockprey.h"
#include "areatime.h"
#include "gadget.h"

StockPreyStdInfo::StockPreyStdInfo(const StockPrey* p, const IntVector& Areas)
  : AbstrPreyStdInfo(p, Areas, p->getALKPriorToEating(Areas[0]).minAge(),
      p->getALKPriorToEating(Areas[0]).maxAge()), SPByLength(p, Areas), prey(p) {
}

void StockPreyStdInfo::Sum(const TimeClass* const TimeInfo, int area) {
  SPByLength.Sum(TimeInfo, area);
  int inarea = this->areaNum(area);
  int age, l;
  double timeratio, tmp;

  for (age = NconbyAge.minCol(inarea); age < NconbyAge.maxCol(inarea); age++) {
    NconbyAge[inarea][age] = 0.0;
    BconbyAge[inarea][age] = 0.0;
  }
  const AgeBandMatrix& alk = prey->getALKPriorToEating(area);
  PopInfo nullpop;

  PopInfoIndexVector PopByAge(alk.maxAge() - alk.minAge() + 1, alk.minAge(), nullpop);
  PopInfoVector PopByLength(SPByLength.BconsumptionByLength(area).Size(), nullpop);
  alk.sumColumns(PopByLength);

  timeratio = 1.0 / TimeInfo->getTimeStepSize();
  for (age = NconbyAge.minCol(inarea); age < NconbyAge.maxCol(inarea); age++) {
    for (l = alk.minLength(age); l < alk.maxLength(age); l++) {
      PopByAge[age] += alk[age][l];

      if (isZero(alk[age][l].N)) {
        NconbyAgeAndLength[inarea][age][l] = 0.0;
        BconbyAgeAndLength[inarea][age][l] = 0.0;
        MortbyAgeAndLength[inarea][age][l] = 0.0;

      } else {
        tmp = alk[age][l].N * SPByLength.BconsumptionByLength(area)[l] / (PopByLength[l].N * PopByLength[l].W);

        BconbyAgeAndLength[inarea][age][l] = tmp * alk[age][l].W;
        NconbyAgeAndLength[inarea][age][l] = tmp;

        NconbyAge[inarea][age] += NconbyAgeAndLength[inarea][age][l];
        BconbyAge[inarea][age] += BconbyAgeAndLength[inarea][age][l];
        if (NconbyAgeAndLength[inarea][age][l] >= alk[age][l].N)
          MortbyAgeAndLength[inarea][age][l] = MaxMortality;
        else
          MortbyAgeAndLength[inarea][age][l]
            = -log(1.0 - NconbyAgeAndLength[inarea][age][l] / alk[age][l].N) * timeratio;
      }
    }

    if (isZero(PopByAge[age].N))
      MortbyAge[inarea][age] = 0.0;
    else if (NconbyAge[inarea][age] >= PopByAge[age].N)
      MortbyAge[inarea][age] = MaxMortality;
    else
      MortbyAge[inarea][age] = -log(1.0 - NconbyAge[inarea][age] / PopByAge[age].N) * timeratio;
  }
}

const DoubleVector& StockPreyStdInfo::NconsumptionByLength(int area) const {
  return SPByLength.NconsumptionByLength(area);
}

const DoubleVector& StockPreyStdInfo::BconsumptionByLength(int area) const {
  return SPByLength.BconsumptionByLength(area);
}

const DoubleVector& StockPreyStdInfo::MortalityByLength(int area) const {
  return SPByLength.MortalityByLength(area);
}
