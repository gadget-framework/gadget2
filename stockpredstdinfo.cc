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
  : AbstrPredStdInfo(Areas, predator->getAgeLengthKeys(Areas[0]).minAge(),
      predator->getAgeLengthKeys(Areas[0]).maxAge(),
      conprey->getALKPriorToEating(Areas[0]).minAge(),
      conprey->getALKPriorToEating(Areas[0]).maxAge()),
    pred(predator), prey(conprey) {

  preyinfo = new StockPreyStdInfo(conprey, Areas);
  predinfo = new PredStdInfoByLength(predator, conprey, Areas);
}

StockPredStdInfo::StockPredStdInfo(const StockPredator* predator,
  const Prey* conprey, const IntVector& Areas)
  : AbstrPredStdInfo(Areas, predator->getAgeLengthKeys(Areas[0]).minAge(),
      predator->getAgeLengthKeys(Areas[0]).maxAge(), 0, 0),
    pred(predator), prey(conprey) {

  preyinfo = new PreyStdInfo(conprey, Areas);
  predinfo = new PredStdInfoByLength(predator, conprey, Areas);
}

StockPredStdInfo::~StockPredStdInfo() {
  delete preyinfo;
  delete predinfo;
}

void StockPredStdInfo::Sum(const TimeClass* const TimeInfo, int area) {
  int inarea = this->areaNum(area);
  preyinfo->Sum(TimeInfo, area);
  predinfo->Sum(TimeInfo, area);

  int predage, preyage, preyl, predl;
  double B, N, prop;

  const AgeBandMatrix& alk = pred->getAgeLengthKeys(area);
  const PopInfoVector& pn = pred->getPredatorNumber(area);

  int minage, maxage, numlength;
  minage = pred->minAge();
  maxage = pred->maxAge();
  numlength = pred->getLengthGroupDiv()->numLengthGroups();
  IntVector size(maxage - minage + 1, numlength);
  IntVector minlength(maxage - minage + 1, 0);
  BandMatrix Alprop(minlength, size, minage);

  for (predage = Alprop.minRow(); predage <= Alprop.maxRow(); predage++) {
    for (predl = Alprop.minCol(predage); predl < Alprop.maxCol(predage); predl++) {
      if (!(isZero(pn[predl].N)))
        Alprop[predage][predl] = alk[predage][predl].N / pn[predl].N;
      else
        Alprop[predage][predl] = 0.0;
    }
  }

  DoubleVector predBconsbyAge(Alprop.maxRow() - Alprop.minRow() + 1, 0.0);
  const BandMatrix& preyNcons = preyinfo->NconsumptionByAgeAndLength(area);
  const BandMatrix& preyBcons = preyinfo->BconsumptionByAgeAndLength(area);
  const DoubleMatrix& predBcons = pred->getConsumption(area, prey->getName());

  for (predage = NconbyAge[inarea].minRow(); predage <= NconbyAge[inarea].maxRow(); predage++)
    for (preyage = NconbyAge[inarea].minCol(predage); preyage < NconbyAge[inarea].maxCol(predage); preyage++) {
      NconbyAge[inarea][predage][preyage] = 0.0;
      BconbyAge[inarea][predage][preyage] = 0.0;
    }

  for (predage = Alprop.minRow(); predage <= Alprop.maxRow(); predage++) {
    for (preyage = NconbyAge[inarea].minCol(predage);
         preyage < NconbyAge[inarea].maxCol(predage); preyage++) {
      for (preyl = 0; preyl < prey->getLengthGroupDiv()->numLengthGroups(); preyl++) {
        for (predl = 0; predl < pred->getLengthGroupDiv()->numLengthGroups(); predl++) {
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
