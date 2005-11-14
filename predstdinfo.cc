#include "areatime.h"
#include "poppredator.h"
#include "predstdinfobylength.h"
#include "predstdinfo.h"
#include "preystdinfo.h"
#include "prey.h"
#include "stockpreystdinfo.h"
#include "stockprey.h"

PredStdInfo::PredStdInfo(const PopPredator* predator, const Prey* p, const IntVector& Areas)
  : AbstrPredStdInfo(Areas, 0, 0, 0, 0), pred(predator), prey(p) {

  preyinfo = new PreyStdInfo(prey, Areas);
  predinfo = new PredStdInfoByLength(pred, prey, Areas);
}

PredStdInfo::PredStdInfo(const PopPredator* predator, const StockPrey* p, const IntVector& Areas)
  : AbstrPredStdInfo(Areas, 0, 0, p->getALKPriorToEating(Areas[0]).minAge(),
      p->getALKPriorToEating(Areas[0]).maxAge()), pred(predator), prey(p) {

  preyinfo = new StockPreyStdInfo(p, Areas);
  predinfo = new PredStdInfoByLength(pred, prey, Areas);
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
  int inarea = this->areaNum(area);
  const BandMatrix& preyNcons = preyinfo->NconsumptionByAgeAndLength(area);
  const BandMatrix& preyBcons = preyinfo->BconsumptionByAgeAndLength(area);
  const DoubleMatrix& predBcons = pred->getConsumption(area, prey->getName());
  int predage, preyage, predl, preyl;
  double B, N, prop;

  predage = 0;
  for (preyage = NconbyAge[inarea].minCol(predage);
       preyage < NconbyAge[inarea].maxCol(predage); preyage++) {
    NconbyAge[inarea][predage][preyage] = 0.0;
    BconbyAge[inarea][predage][preyage] = 0.0;
    for (preyl = 0; preyl < prey->getLengthGroupDiv()->numLengthGroups(); preyl++) {
      if (!(isZero(preyinfo->BconsumptionByLength(area)[preyl]))) {
        for (predl = 0; predl < pred->getLengthGroupDiv()->numLengthGroups(); predl++) {
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
      MortbyAge[inarea][predage][preyage] = preyinfo->MortalityByAge(area)[preyage] *
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
