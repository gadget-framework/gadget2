#include "stockpreystdinfo.h"
#include "intvector.h"
#include "stockprey.h"
#include "areatime.h"
#include "gadget.h"

StockPreyStdInfo::StockPreyStdInfo(const StockPrey* p, const IntVector& Areas)
  : AbstrPreyStdInfo(p, Areas, p->AlkeysPriorToEating(Areas[0]).Minage(),
    p->AlkeysPriorToEating(Areas[0]).Maxage()),
  SPByLength(p, Areas), prey(p) {
}

StockPreyStdInfo::~StockPreyStdInfo() {
}

void StockPreyStdInfo::Sum(const TimeClass* const TimeInfo, int area) {
  SPByLength.Sum(TimeInfo, area);
  const int inarea = AreaNr[area];
  int age;
  for (age = NconbyAge.Mincol(inarea); age < NconbyAge.Maxcol(inarea); age++) {
    NconbyAge[inarea][age] = 0.0;
    BconbyAge[inarea][age] = 0.0;
  }
  const AgeBandMatrix& Alk = prey->AlkeysPriorToEating(area);
  PopInfo nullpop;

  PopInfoIndexVector PopByAge(Alk.Maxage() - Alk.Minage() + 1, Alk.Minage(), nullpop);
  PopInfoVector PopByLength(SPByLength.BconsumptionByLength(area).Size(), nullpop);
  Alk.Colsum(PopByLength);

  int l = 0;
  double timeratio, tmp;

  timeratio = TimeInfo->LengthOfYear() / TimeInfo->LengthOfCurrent();
  for (age = NconbyAge.Mincol(inarea); age < NconbyAge.Maxcol(inarea); age++) {
    for (l = Alk.Minlength(age); l < Alk.Maxlength(age); l++) {
      PopByAge[age] += Alk[age][l];
      if (Alk[age][l].N != 0 && SPByLength.BconsumptionByLength(area)[l] != 0) {
        tmp = Alk[age][l].N * SPByLength.BconsumptionByLength(area)[l] / (PopByLength[l].N * PopByLength[l].W);

        BconbyAgeAndLength[inarea][age][l] = tmp * Alk[age][l].W;
        NconbyAgeAndLength[inarea][age][l] = tmp;

        NconbyAge[inarea][age] += NconbyAgeAndLength[inarea][age][l];
        BconbyAge[inarea][age] += BconbyAgeAndLength[inarea][age][l];
        if (iszero(Alk[age][l].N))
          MortbyAgeAndLength[inarea][age][l] = 0.0;
        else {
          if (NconbyAgeAndLength[inarea][age][l] >= Alk[age][l].N)
            MortbyAgeAndLength[inarea][age][l] = MAX_MORTALITY;
          else
            MortbyAgeAndLength[inarea][age][l]
              = -log(1 - NconbyAgeAndLength[inarea][age][l] / Alk[age][l].N) * timeratio;
        }

      } else {
        NconbyAgeAndLength[inarea][age][l] = 0.0;
        BconbyAgeAndLength[inarea][age][l] = 0.0;
        MortbyAgeAndLength[inarea][age][l] = 0.0;
      }
    }
    if (PopByAge[age].N > 0) {
      if (PopByAge[age].N <= NconbyAge[inarea][age])
        MortbyAge[inarea][age] = MAX_MORTALITY;
      else
        MortbyAge[inarea][age]
          = - log(1 - NconbyAge[inarea][age] / PopByAge[age].N) * timeratio;
    } else
      MortbyAge[inarea][age] = 0.0;
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

const LengthGroupDivision* StockPreyStdInfo::ReturnPreyLengthGroupDiv() const {
  return SPByLength.ReturnPreyLengthGroupDiv();
}
