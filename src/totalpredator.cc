#include "totalpredator.h"
#include "keeper.h"
#include "prey.h"
#include "errorhandler.h"
#include "gadget.h"
#include "global.h"

TotalPredator::TotalPredator(CommentStream& infile, const char* givenname,
  const IntVector& Areas, const TimeClass* const TimeInfo, Keeper* const keeper, Formula multscaler)
  : LengthPredator(givenname, Areas, TimeInfo,keeper, multscaler) {

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

  wanttoeat = prednumber[inarea][predl].N * multi*timeMultiplier[TimeInfo->getTime()] / TimeInfo->numSubSteps();
  totalcons[inarea][predl] = 0.0;

  if (isZero(wanttoeat)) //JMB no predation takes place on this timestep
    return;

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
  if (!(isZero(totalcons[inarea][predl]))) {
    tmp = wanttoeat / totalcons[inarea][predl];
    for (prey = 0; prey < this->numPreys(); prey++) {
      if (this->getPrey(prey)->isPreyArea(area)) {
        for (preyl = 0; preyl < (*cons[inarea][prey])[predl].Size(); preyl++)
          (*cons[inarea][prey])[predl][preyl] *= tmp;

        //inform the preys of the consumption
        this->getPrey(prey)->addBiomassConsumption(area, (*cons[inarea][prey])[predl]);
        //set the multiplicative constant
        (*predratio[inarea])[prey][predl] += totalcons[inarea][predl];
        if ((TimeInfo->getSubStep() == TimeInfo->numSubSteps()) && (!(isZero((*predratio[inarea])[prey][predl]))))
          (*predratio[inarea])[prey][predl] = wanttoeat / (*predratio[inarea])[prey][predl];
      }
    }
  }

  //finally set totalcons to the actual consumption
  totalcons[inarea][predl] = wanttoeat;
}

void TotalPredator::adjustConsumption(int area, const TimeClass* const TimeInfo) {
  int check, prey, preyl;
  int inarea = this->areaNum(area);
  int predl = 0;  //JMB there is only ever one length group ...
  overcons[inarea][predl] = 0.0;

  if (isZero(totalcons[inarea][predl])) //JMB no predation takes place on this timestep
    return;

  double maxRatio, tmp;
  maxRatio = TimeInfo->getMaxRatioConsumed();

  check = 0;
  for (prey = 0; prey < this->numPreys(); prey++) {
    if (this->getPrey(prey)->isPreyArea(area)) {
      check = 1;
      if (this->getPrey(prey)->isOverConsumption(area)) {
        hasoverconsumption[inarea] = 1;
        DoubleVector ratio = this->getPrey(prey)->getRatio(area);
        for (preyl = 0; preyl < (*cons[inarea][prey])[predl].Size(); preyl++) {
          if (ratio[preyl] > maxRatio) {
            tmp = maxRatio / ratio[preyl];
            overcons[inarea][predl] += (1.0 - tmp) * (*cons[inarea][prey])[predl][preyl];
            (*cons[inarea][prey])[predl][preyl] *= tmp;
            (*usesuit[inarea][prey])[predl][preyl] *= tmp;
          }
        }
      }
    }
  }

  if (!check) {  //if no prey found to consume then overcons set to actual consumption
    hasoverconsumption[inarea] = 1;
    overcons[inarea][predl] = totalcons[inarea][predl];
  }

  if (hasoverconsumption[inarea]) {
    totalcons[inarea][predl] -= overcons[inarea][predl];
    overconsumption[inarea][predl] += overcons[inarea][predl];
  }

  totalconsumption[inarea][predl] += totalcons[inarea][predl];
  for (prey = 0; prey < this->numPreys(); prey++)
    if (this->getPrey(prey)->isPreyArea(area))
      for (preyl = 0; preyl < (*cons[inarea][prey])[predl].Size(); preyl++)
        (*consumption[inarea][prey])[predl][preyl] += (*cons[inarea][prey])[predl][preyl];
}

void TotalPredator::Print(ofstream& outfile) const {
  outfile << "TotalPredator\n";
  PopPredator::Print(outfile);
}
