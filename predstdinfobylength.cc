#include "abstrpreystdinfobylength.h"
#include "areatime.h"
#include "poppredator.h"
#include "predstdinfobylength.h"
#include "preystdinfobylength.h"
#include "prey.h"
#include "stockpreystdinfobylength.h"
#include "stockprey.h"
#include "gadget.h"

PredStdInfoByLength::PredStdInfoByLength(const PopPredator* pred, const Prey* pRey, const intvector& Areas)
  : LivesOnAreas(Areas), preyinfo(new PreyStdInfoByLength(pRey, Areas)),
  predator(pred), prey(pRey) {

  this->AdjustObjects();
}

PredStdInfoByLength::PredStdInfoByLength(const PopPredator* pred, const StockPrey* pRey, const intvector& Areas)
  : LivesOnAreas(Areas), preyinfo(new StockPreyStdInfoByLength(pRey, Areas)),
  predator(pred), prey(pRey) {

  this->AdjustObjects();
}

PredStdInfoByLength::~PredStdInfoByLength() {
  delete preyinfo;
}

void PredStdInfoByLength::AdjustObjects() {
  while (MortbyLength.Size() > 0) {
    MortbyLength.Delete(0);
    NconbyLength.Delete(0);
    BconbyLength.Delete(0);
  }
  //Create a bandmatrix bm, filled with 0.
  doublematrix dm(predator->NoLengthGroups(), prey->NoLengthGroups(), 1);
  bandmatrix bm(dm);
  int i, j;
  for (i = 0; i < bm.Nrow(); i++)
    for (j = 0; j < bm.Ncol(i); j++)
      bm[i][j] = 0;
  MortbyLength.resize(areas.Size(), bm);
  NconbyLength.resize(areas.Size(), bm);
  BconbyLength.resize(areas.Size(), bm);
}

void PredStdInfoByLength::Sum(const TimeClass* const TimeInfo, int area) {
  const int inarea = AreaNr[area];
  preyinfo->Sum(TimeInfo, area);
  const doublevector& NpreyEaten = preyinfo->NconsumptionByLength(area);
  const doublevector& BpreyEaten = preyinfo->BconsumptionByLength(area);
  const doublevector& TotpreyMort = preyinfo->MortalityByLength(area);
  const bandmatrix& BpredEaten = predator->Consumption(area, prey->Name());
  int predl, preyl;

  for (predl = 0; predl < NconbyLength[inarea].Nrow(); predl++)
    for (preyl = 0; preyl < NconbyLength[inarea].Ncol(); preyl++) {
      //proportion equals the proportion of the predation on preyl that predl acounts for.
      const double proportion = (iszero(BpreyEaten[preyl]) ? 0 : BpredEaten[predl][preyl] / BpreyEaten[preyl]);
      NconbyLength[inarea][predl][preyl] = proportion * NpreyEaten[preyl];
      BconbyLength[inarea][predl][preyl] = proportion * BpreyEaten[preyl];
      MortbyLength[inarea][predl][preyl] = proportion * TotpreyMort[preyl];
    }
}

const bandmatrix& PredStdInfoByLength::NconsumptionByLength(int area) const {
  return NconbyLength[AreaNr[area]];
}

const bandmatrix& PredStdInfoByLength::BconsumptionByLength(int area) const {
  return BconbyLength[AreaNr[area]];
}

const bandmatrix& PredStdInfoByLength::MortalityByLength(int area) const {
  return MortbyLength[AreaNr[area]];
}
