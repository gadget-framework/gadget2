#include "areatime.h"
#include "predstdinfobylength.h"
#include "preystdinfo.h"
#include "prey.h"
#include "stockpredator.h"
#include "stockpredstdinfo.h"
#include "stockpreystdinfo.h"
#include "stockprey.h"


StockPredStdInfo::StockPredStdInfo(const StockPredator* predator,
  const StockPrey* conprey, const IntVector& Areas)
  : AbstrPredStdInfo(Areas, predator->Alproportion(Areas[0]).minRow(),
    predator->Alproportion(Areas[0]).maxRow(),
    conprey->AlkeysPriorToEating(Areas[0]).minAge(),
    conprey->AlkeysPriorToEating(Areas[0]).maxAge()),
    pred(predator), prey(conprey) {

  preyinfo = new StockPreyStdInfo(conprey, Areas);
  predinfo = new PredStdInfoByLength(pred, prey, Areas);
}

StockPredStdInfo::StockPredStdInfo(const StockPredator* predator,
  const Prey* conprey, const IntVector& Areas)
  : AbstrPredStdInfo(Areas, predator->Alproportion(Areas[0]).minRow(),
    predator->Alproportion(Areas[0]).maxRow(), 0, 0),
    pred(predator), prey(conprey) {

  preyinfo = new PreyStdInfo(prey, Areas);
  predinfo = new PredStdInfoByLength(pred, prey, Areas);
}

StockPredStdInfo::~StockPredStdInfo() {
  delete preyinfo;
  delete predinfo;
}

void StockPredStdInfo::Sum(const TimeClass* const TimeInfo, int area) {
  int inarea = this->areaNum(area);
  preyinfo->Sum(TimeInfo, area);
  predinfo->Sum(TimeInfo, area);

  const BandMatrix& Alprop = pred->Alproportion(area);
  DoubleVector predBconsbyAge(Alprop.maxRow() - Alprop.minRow() + 1, 0.0);
  const BandMatrix& preyNcons = preyinfo->NconsumptionByAgeAndLength(area);
  const BandMatrix& preyBcons = preyinfo->BconsumptionByAgeAndLength(area);
  const BandMatrix& predBcons = pred->getConsumption(area, prey->getName());

  int predage, preyage, preyl, predl;
  double B, N, prop;

  for (predage = NconbyAge[inarea].minRow(); predage <= NconbyAge[inarea].maxRow(); predage++)
    for (preyage = NconbyAge[inarea].minCol(predage); preyage < NconbyAge[inarea].maxCol(predage); preyage++) {
      NconbyAge[inarea][predage][preyage] = 0.0;
      BconbyAge[inarea][predage][preyage] = 0.0;
    }

  for (predage = Alprop.minRow(); predage <= Alprop.maxRow(); predage++) {
    for (preyage = NconbyAge[inarea].minCol(predage);
         preyage < NconbyAge[inarea].maxCol(predage); preyage++) {
      for (preyl = 0; preyl < prey->numLengthGroups(); preyl++) {
        for (predl = 0; predl < pred->numLengthGroups(); predl++) {
          if (!(isZero(preyBcons[preyage][preyl])) && (!(isZero(predBcons[predl][preyl])))) {
            prop = predBcons[predl][preyl] * Alprop[predage][predl] / preyinfo->BconsumptionByLength(area)[preyl];
            B = prop * preyBcons[preyage][preyl];
            N = prop * preyNcons[preyage][preyl];
            BconbyAge[inarea][predage][preyage] += B;
            NconbyAge[inarea][predage][preyage] += N;
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
}

const BandMatrix& StockPredStdInfo::NconsumptionByLength(int area) const {
  return predinfo->NconsumptionByLength(area);
}

const BandMatrix& StockPredStdInfo::BconsumptionByLength(int area) const {
  return predinfo->BconsumptionByLength(area);
}

const BandMatrix& StockPredStdInfo::MortalityByLength(int area) const {
  return predinfo->MortalityByLength(area);
}
