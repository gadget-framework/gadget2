#include "totalpredator.h"
#include "keeper.h"
#include "prey.h"
#include "errorhandler.h"
#include "gadget.h"

extern ErrorHandler handle;

TotalPredator::TotalPredator(CommentStream& infile, const char* givenname,
  const IntVector& Areas, const TimeClass* const TimeInfo, Keeper* const keeper, Formula multscaler)
  : LengthPredator(givenname, Areas, keeper, multscaler) {

  keeper->addString("predator");
  keeper->addString(givenname);
  this->readSuitability(infile, "amount", TimeInfo, keeper);
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

  //Calculate consumption up to a multiplicative constant.
  for (prey = 0; prey < this->numPreys(); prey++) {
    if (this->getPrey(prey)->isPreyArea(area)) {
      for (preyl = 0; preyl < this->getPrey(prey)->numLengthGroups(); preyl++) {
        cons[inarea][prey][predl][preyl]
          = Suitability(prey)[predl][preyl] * this->getPrey(prey)->getBiomass(area, preyl);
        totalcons[inarea][predl] += cons[inarea][prey][predl][preyl];
      }

    } else {
      for (preyl = 0; preyl < this->getPrey(prey)->numLengthGroups(); preyl++)
        cons[inarea][prey][predl][preyl] = 0.0;
    }
  }

  //adjust the consumption by the multiplicative factor.
  tmp = multi / TimeInfo->numSubSteps();  //use the multiplicative factor
  for (prey = 0; prey < this->numPreys(); prey++) {
    if (this->getPrey(prey)->isPreyArea(area)) {
      wanttoeat = tmp * prednumber[inarea][predl].N;
      for (preyl = 0; preyl < this->getPrey(prey)->numLengthGroups(); preyl++)
        cons[inarea][prey][predl][preyl] *= wanttoeat / totalcons[inarea][predl];
    }
  }

  //set the multiplicative constant
  scaler[inarea] += totalcons[inarea][predl];
  if ((TimeInfo->getSubStep() == TimeInfo->numSubSteps()) && (!(isZero(scaler[inarea]))))
    scaler[inarea] = 1.0 / scaler[inarea];

  //Inform the preys of the consumption.
  for (prey = 0; prey < this->numPreys(); prey++)
    if (this->getPrey(prey)->isPreyArea(area))
      this->getPrey(prey)->addBiomassConsumption(area, cons[inarea][prey][predl]);

  //set totalconsumption to the actual total consumption
  for (prey = 0; prey < this->numPreys(); prey++)
    if (this->getPrey(prey)->isPreyArea(area))
      totalcons[inarea][predl] = tmp * prednumber[inarea][predl].N;
}

void TotalPredator::adjustConsumption(int area, const TimeClass* const TimeInfo) {
  double maxRatio = pow(MaxRatioConsumed, TimeInfo->numSubSteps());
  int check, over, prey, preyl;
  int inarea = this->areaNum(area);
  double ratio, tmp;

  int predl = 0;  //JMB there is only ever one length group ...
  overcons[inarea][predl] = 0.0;

  over = 0;
  check = 0;
  for (prey = 0; prey < this->numPreys(); prey++) {
    if (this->getPrey(prey)->isPreyArea(area)) {
      check = 1;
      if (this->getPrey(prey)->checkOverConsumption(area)) {
        over = 1;
        for (preyl = 0; preyl < this->getPrey(prey)->numLengthGroups(); preyl++) {
          ratio = this->getPrey(prey)->getRatio(area, preyl);
          if (ratio > maxRatio) {
            tmp = maxRatio / ratio;
            overcons[inarea][predl] += (1.0 - tmp) * cons[inarea][prey][predl][preyl];
            cons[inarea][prey][predl][preyl] *= tmp;
          }
        }
      }
    }
  }

  if (over == 1)
    totalcons[inarea][predl] -= overcons[inarea][predl];

  //if no prey found to consume then overcons set to actual consumption
  if (check == 0)
    overcons[inarea][predl] = prednumber[inarea][predl].N * multi / TimeInfo->numSubSteps();

  totalconsumption[inarea][predl] += totalcons[inarea][predl];
  overconsumption[inarea][predl] += overcons[inarea][predl];

  for (prey = 0; prey < this->numPreys(); prey++)
    if (this->getPrey(prey)->isPreyArea(area))
      for (preyl = 0; preyl < this->getPrey(prey)->numLengthGroups(); preyl++)
        consumption[inarea][prey][predl][preyl] += cons[inarea][prey][predl][preyl];
}

void TotalPredator::Print(ofstream& outfile) const {
  outfile << "TotalPredator\n";
  PopPredator::Print(outfile);
}

const PopInfoVector& TotalPredator::getNumberPriorToEating(int area, const char* preyname) const {
  int prey;
  for (prey = 0; prey < this->numPreys(); prey++)
    if (strcasecmp(getPreyName(prey), preyname) == 0)
      return this->getPrey(prey)->getNumberPriorToEating(area);

  handle.logMessage(LOGFAIL, "Error in totalpredator - failed to match prey", preyname);
  exit(EXIT_FAILURE);
}
