#include "abstrpreystdinfobylength.h"
#include "areatime.h"
#include "poppredator.h"
#include "predstdinfobylength.h"
#include "preystdinfobylength.h"
#include "prey.h"
#include "stockpreystdinfobylength.h"
#include "stockprey.h"
#include "gadget.h"

PredStdInfoByLength::PredStdInfoByLength(const PopPredator* pred, const Prey* pRey, const IntVector& Areas)
  : LivesOnAreas(Areas), preyinfo(new PreyStdInfoByLength(pRey, Areas)),
  predator(pred), prey(pRey) {

  this->InitialiseObjects();
}

PredStdInfoByLength::PredStdInfoByLength(const PopPredator* pred, const StockPrey* pRey, const IntVector& Areas)
  : LivesOnAreas(Areas), preyinfo(new StockPreyStdInfoByLength(pRey, Areas)),
  predator(pred), prey(pRey) {

  this->InitialiseObjects();
}

PredStdInfoByLength::~PredStdInfoByLength() {
  delete preyinfo;
}

void PredStdInfoByLength::InitialiseObjects() {
  while (MortbyLength.Size() > 0) {
    MortbyLength.Delete(0);
    NconbyLength.Delete(0);
    BconbyLength.Delete(0);
  }
  //Create a BandMatrix bm, filled with 0.
  DoubleMatrix dm(predator->numLengthGroups(), prey->numLengthGroups(), 1);
  BandMatrix bm(dm);
  int i, j;
  for (i = 0; i < bm.Nrow(); i++)
    for (j = 0; j < bm.Ncol(i); j++)
      bm[i][j] = 0;
  MortbyLength.resize(areas.Size(), bm);
  NconbyLength.resize(areas.Size(), bm);
  BconbyLength.resize(areas.Size(), bm);

  predLgrpDiv = predator->returnLengthGroupDiv();
  preyLgrpDiv = prey->returnLengthGroupDiv();
}

void PredStdInfoByLength::Sum(const TimeClass* const TimeInfo, int area) {
  const int inarea = AreaNr[area];
  preyinfo->Sum(TimeInfo, area);
  const DoubleVector& NpreyEaten = preyinfo->NconsumptionByLength(area);
  const DoubleVector& BpreyEaten = preyinfo->BconsumptionByLength(area);
  const DoubleVector& TotpreyMort = preyinfo->MortalityByLength(area);
  const BandMatrix& BpredEaten = predator->Consumption(area, prey->Name());
  int predl, preyl;

  for (predl = 0; predl < NconbyLength[inarea].Nrow(); predl++)
    for (preyl = 0; preyl < NconbyLength[inarea].Ncol(); preyl++) {
      //proportion equals the proportion of the predation on preyl that predl acounts for.
      const double proportion = (isZero(BpreyEaten[preyl]) ? 0 : BpredEaten[predl][preyl] / BpreyEaten[preyl]);
      NconbyLength[inarea][predl][preyl] = proportion * NpreyEaten[preyl];
      BconbyLength[inarea][predl][preyl] = proportion * BpreyEaten[preyl];
      MortbyLength[inarea][predl][preyl] = proportion * TotpreyMort[preyl];
    }
}

const BandMatrix& PredStdInfoByLength::NconsumptionByLength(int area) const {
  return NconbyLength[AreaNr[area]];
}

const BandMatrix& PredStdInfoByLength::BconsumptionByLength(int area) const {
  return BconbyLength[AreaNr[area]];
}

const BandMatrix& PredStdInfoByLength::MortalityByLength(int area) const {
  return MortbyLength[AreaNr[area]];
}
