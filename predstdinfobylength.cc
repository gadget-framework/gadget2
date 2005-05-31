#include "abstrpreystdinfobylength.h"
#include "areatime.h"
#include "poppredator.h"
#include "predstdinfobylength.h"
#include "preystdinfobylength.h"
#include "prey.h"
#include "stockpreystdinfobylength.h"
#include "stockprey.h"
#include "gadget.h"

PredStdInfoByLength::PredStdInfoByLength(const PopPredator* pred, const Prey* p, const IntVector& Areas)
  : LivesOnAreas(Areas), preyinfo(new PreyStdInfoByLength(p, Areas)),
  predator(pred), prey(p) {

  predLgrpDiv = predator->getLengthGroupDiv();
  preyLgrpDiv = prey->getLengthGroupDiv();

  DoubleMatrix dm(predLgrpDiv->numLengthGroups(), preyLgrpDiv->numLengthGroups(), 0.0);
  BandMatrix bm(dm);
  int i, j;
  for (i = 0; i < bm.Nrow(); i++)
    for (j = 0; j < bm.Ncol(i); j++)
      bm[i][j] = 0.0;

  MortbyLength.resize(areas.Size(), bm);
  NconbyLength.resize(areas.Size(), bm);
  BconbyLength.resize(areas.Size(), bm);
}

PredStdInfoByLength::PredStdInfoByLength(const PopPredator* pred, const StockPrey* p, const IntVector& Areas)
  : LivesOnAreas(Areas), preyinfo(new StockPreyStdInfoByLength(p, Areas)),
  predator(pred), prey(p) {

  predLgrpDiv = predator->getLengthGroupDiv();
  preyLgrpDiv = prey->getLengthGroupDiv();

  DoubleMatrix dm(predLgrpDiv->numLengthGroups(), preyLgrpDiv->numLengthGroups(), 0.0);
  BandMatrix bm(dm);
  int i, j;
  for (i = 0; i < bm.Nrow(); i++)
    for (j = 0; j < bm.Ncol(i); j++)
      bm[i][j] = 0.0;

  MortbyLength.resize(areas.Size(), bm);
  NconbyLength.resize(areas.Size(), bm);
  BconbyLength.resize(areas.Size(), bm);
}

PredStdInfoByLength::~PredStdInfoByLength() {
  delete preyinfo;
}

void PredStdInfoByLength::Sum(const TimeClass* const TimeInfo, int area) {
  int inarea = this->areaNum(area);
  preyinfo->Sum(TimeInfo, area);
  const DoubleVector& NpreyEaten = preyinfo->NconsumptionByLength(area);
  const DoubleVector& BpreyEaten = preyinfo->BconsumptionByLength(area);
  const DoubleVector& TotpreyMort = preyinfo->MortalityByLength(area);
  const BandMatrix& BpredEaten = predator->getConsumption(area, prey->getName());
  int predl, preyl;
  double proportion;

  for (predl = 0; predl < predLgrpDiv->numLengthGroups(); predl++) {
    for (preyl = 0; preyl < preyLgrpDiv->numLengthGroups(); preyl++) {
      if (isZero(BpreyEaten[preyl])) {
        NconbyLength[inarea][predl][preyl] = 0.0;
        BconbyLength[inarea][predl][preyl] = 0.0;
        MortbyLength[inarea][predl][preyl] = 0.0;
      } else {
        //proportion equals the proportion of the predation on preyl that predl acounts for.
        proportion = BpredEaten[predl][preyl] / BpreyEaten[preyl];
        NconbyLength[inarea][predl][preyl] = proportion * NpreyEaten[preyl];
        BconbyLength[inarea][predl][preyl] = proportion * BpreyEaten[preyl];
        MortbyLength[inarea][predl][preyl] = proportion * TotpreyMort[preyl];
      }
    }
  }
}

const BandMatrix& PredStdInfoByLength::NconsumptionByLength(int area) const {
  return NconbyLength[this->areaNum(area)];
}

const BandMatrix& PredStdInfoByLength::BconsumptionByLength(int area) const {
  return BconbyLength[this->areaNum(area)];
}

const BandMatrix& PredStdInfoByLength::MortalityByLength(int area) const {
  return MortbyLength[this->areaNum(area)];
}
