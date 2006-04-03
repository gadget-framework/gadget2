#include "totalpredator.h"
#include "keeper.h"
#include "prey.h"
#include "errorhandler.h"
#include "gadget.h"

extern ErrorHandler handle;

TotalPredator::TotalPredator(CommentStream& infile, const char* givenname,
  const IntVector& Areas, const TimeClass* const TimeInfo, Keeper* const keeper, Formula multscaler)
  : LengthPredator(givenname, Areas, keeper, multscaler) {

  type = TOTALPREDATOR;
  keeper->addString("predator");
  keeper->addString(givenname);
  this->readSuitability(infile, TimeInfo, keeper);
  keeper->clearLast();
  keeper->clearLast();
}

void TotalPredator::Eat(int area, const AreaClass* const Area, const TimeClass* const TimeInfo) {

  int inarea = this->areaNum(area);
  int prey, preyl;
  double tmp, wanttoeat;

  int predl = 0;  //JMB there is only ever one length group ...
  totalcons[inarea][predl] = 0.0;

  if (TimeInfo->getSubStep() == 1)
    scaler[inarea] = 0.0;

  //calculate consumption up to a multiplicative constant
  for (prey = 0; prey < this->numPreys(); prey++) {
    if (this->getPrey(prey)->isPreyArea(area)) {
      for (preyl = 0; preyl < (*cons[inarea][prey])[predl].Size(); preyl++) {
        (*cons[inarea][prey])[predl][preyl]
          = this->getSuitability(prey)[predl][preyl] * this->getPrey(prey)->getBiomass(area, preyl);
        totalcons[inarea][predl] += (*cons[inarea][prey])[predl][preyl];
      }

    } else {
      for (preyl = 0; preyl < (*cons[inarea][prey])[predl].Size(); preyl++)
        (*cons[inarea][prey])[predl][preyl] = 0.0;
    }
  }

  //adjust the consumption by the multiplicative factor
  wanttoeat = prednumber[inarea][predl].N * multi / TimeInfo->numSubSteps();
  tmp = wanttoeat / totalcons[inarea][predl];
  for (prey = 0; prey < this->numPreys(); prey++)
    if (this->getPrey(prey)->isPreyArea(area))
      for (preyl = 0; preyl < (*cons[inarea][prey])[predl].Size(); preyl++)
        (*cons[inarea][prey])[predl][preyl] *= tmp;

  //inform the preys of the consumption
  for (prey = 0; prey < this->numPreys(); prey++)
    if (this->getPrey(prey)->isPreyArea(area))
      this->getPrey(prey)->addBiomassConsumption(area, (*cons[inarea][prey])[predl]);

  //set the multiplicative constant
  scaler[inarea] += totalcons[inarea][predl];
  if ((TimeInfo->getSubStep() == TimeInfo->numSubSteps()) && (!(isZero(scaler[inarea]))))
    scaler[inarea] = 1.0 / scaler[inarea];

  //finally set totalcons to the actual consumption
  totalcons[inarea][predl] = wanttoeat;
}

void TotalPredator::adjustConsumption(int area, const TimeClass* const TimeInfo) {
  int check, prey, preyl;
  int inarea = this->areaNum(area);
  double maxRatio, tmp;

  int predl = 0;  //JMB there is only ever one length group ...
  overcons[inarea][predl] = 0.0;
  maxRatio = MaxRatioConsumed;
  if (TimeInfo->numSubSteps() != 1)
    maxRatio = pow(MaxRatioConsumed, TimeInfo->numSubSteps());

  check = 0;
  for (prey = 0; prey < this->numPreys(); prey++) {
    if (this->getPrey(prey)->isPreyArea(area)) {
      check = 1;
      if (this->getPrey(prey)->isOverConsumption(area)) {
        DoubleVector ratio = this->getPrey(prey)->getRatio(inarea);
        for (preyl = 0; preyl < (*cons[inarea][prey])[predl].Size(); preyl++) {
          if (ratio[preyl] > maxRatio) {
            tmp = maxRatio / ratio[preyl];
            overcons[inarea][predl] += (1.0 - tmp) * (*cons[inarea][prey])[predl][preyl];
            (*cons[inarea][prey])[predl][preyl] *= tmp;
          }
        }
      }
    }
  }

  //if no prey found to consume then overcons set to actual consumption
  if (check == 0)
    overcons[inarea][predl] = totalcons[inarea][predl];

  totalcons[inarea][predl] -= overcons[inarea][predl];
  totalconsumption[inarea][predl] += totalcons[inarea][predl];
  overconsumption[inarea][predl] += overcons[inarea][predl];
  for (prey = 0; prey < this->numPreys(); prey++)
    if (this->getPrey(prey)->isPreyArea(area))
      for (preyl = 0; preyl < (*cons[inarea][prey])[predl].Size(); preyl++)
        (*consumption[inarea][prey])[predl][preyl] += (*cons[inarea][prey])[predl][preyl];
}

void TotalPredator::Print(ofstream& outfile) const {
  outfile << "TotalPredator\n";
  PopPredator::Print(outfile);
}
