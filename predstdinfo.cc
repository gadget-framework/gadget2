#include "areatime.h"
#include "poppredator.h"
#include "predstdinfobylength.h"
#include "predstdinfo.h"
#include "preystdinfo.h"
#include "prey.h"
#include "stockpreystdinfo.h"
#include "stockprey.h"

PredStdInfo::PredStdInfo(const PopPredator* pred, const Prey* pRey, const IntVector& Areas)
  : AbstrPredStdInfo(Areas, 0, 0, 0, 0), //prey and pred. min and max age eq. 0
  preyinfo(new PreyStdInfo(pRey, Areas)),
  predinfo(new PredStdInfoByLength(pred, pRey, Areas)),
  predator(pred), prey(pRey) {
}

PredStdInfo::PredStdInfo(const PopPredator* pred, const StockPrey* pRey, const IntVector& Areas)
  : AbstrPredStdInfo(Areas, 0, 0, //pred. min and max age equals 0
  pRey->AlkeysPriorToEating(Areas[0]).minAge(),
  pRey->AlkeysPriorToEating(Areas[0]).maxAge()),
  preyinfo(new StockPreyStdInfo(pRey, Areas)),
  predinfo(new PredStdInfoByLength(pred, pRey, Areas)),
  predator(pred), prey(pRey) {
}

PredStdInfo::~PredStdInfo() {
  delete preyinfo;
  delete predinfo;
}

void PredStdInfo::Sum(const TimeClass* const TimeInfo, int area) {
  //this sum function distributes the predation of the predator on the prey
  //Note that the age group of the predator is fixed to 0.
  preyinfo->Sum(TimeInfo, area);
  predinfo->Sum(TimeInfo, area);
  const int inarea = AreaNr[area];
  const int predage = 0;
  const BandMatrix& preyNcons = preyinfo->NconsumptionByAgeAndLength(area);
  const BandMatrix& preyBcons = preyinfo->BconsumptionByAgeAndLength(area);
  const BandMatrix& predBcons = predator->Consumption(area, prey->Name());
  int preyage, predl, preyl;
  double timeratio, B, N, prop;

  timeratio = 1.0;  //timeratio has been taken into account in the preystdinfo??
  //timeratio = TimeInfo->LengthOfYear() / TimeInfo->LengthOfCurrent();
  for (preyage = NconbyAge[inarea].Mincol(predage);
       preyage < NconbyAge[inarea].Maxcol(predage); preyage++) {
    NconbyAge[inarea][predage][preyage] = 0.0;
    BconbyAge[inarea][predage][preyage] = 0.0;
    for (preyl = 0; preyl < prey->NoLengthGroups(); preyl++) {
      if (!(isZero(preyinfo->BconsumptionByLength(area)[preyl]))) {
        for (predl = 0; predl < predator->NoLengthGroups(); predl++) {
          if (!(isZero(preyBcons[preyage][preyl])) && (!(isZero(predBcons[predl][preyl])))) {
            prop = predBcons[predl][preyl] / preyinfo->BconsumptionByLength(area)[preyl];
            B = prop * preyBcons[preyage][preyl];
            N = prop * preyNcons[preyage][preyl];
            BconbyAge[inarea][predage][preyage] += B;
            NconbyAge[inarea][predage][preyage] += N;
          }
        }
      }
    }

    if (isZero(preyinfo->BconsumptionByAge(area)[preyage]))
      MortbyAge[inarea][predage][preyage] = 0.0;
    else
      MortbyAge[inarea][predage][preyage] = timeratio * preyinfo->MortalityByAge(area)[preyage] *
         BconbyAge[inarea][predage][preyage] / preyinfo->BconsumptionByAge(area)[preyage];
  }
}

const BandMatrix& PredStdInfo::NconsumptionByLength(int area) const {
  return predinfo->NconsumptionByLength(area);
}

const BandMatrix& PredStdInfo::BconsumptionByLength(int area) const {
  return predinfo->BconsumptionByLength(area);
}

const BandMatrix& PredStdInfo::MortalityByLength(int area) const {
  return predinfo->MortalityByLength(area);
}
