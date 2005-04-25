#include "totalpredator.h"
#include "keeper.h"
#include "conversionindex.h"
#include "prey.h"
#include "errorhandler.h"
#include "gadget.h"

extern ErrorHandler handle;

TotalPredator::TotalPredator(CommentStream& infile, const char* givenname,
  const IntVector& Areas, const TimeClass* const TimeInfo, Keeper* const keeper, Formula multi)
  : LengthPredator(givenname, Areas, keeper, multi) {

  keeper->addString("predator");
  keeper->addString(givenname);

  this->readSuitability(infile, "amount", TimeInfo, keeper);

  keeper->clearLast();
  keeper->clearLast();
}

void TotalPredator::Eat(int area, double LengthOfStep, double Temperature,
  double Areasize, int CurrentSubstep, int numsubsteps) {

  int inarea = this->areaNum(area);
  double tmp, wanttoeat;
  int prey, predl, preyl;

  if (CurrentSubstep == 1)
    scaler[inarea] = 0.0;

  for (predl = 0; predl < LgrpDiv->numLengthGroups(); predl++)
    totalcons[inarea][predl] = 0.0;

  //Calculate consumption up to a multiplicative constant.
  for (prey = 0; prey < this->numPreys(); prey++) {
    if (Preys(prey)->isPreyArea(area)) {
      for (predl = 0; predl < LgrpDiv->numLengthGroups(); predl++) {
        for (preyl = Suitability(prey)[predl].minCol();
            preyl < Suitability(prey)[predl].maxCol(); preyl++) {

          cons[inarea][prey][predl][preyl]
            = Suitability(prey)[predl][preyl] * Preys(prey)->getBiomass(area, preyl);
          totalcons[inarea][predl] += cons[inarea][prey][predl][preyl];
        }
      }

    } else {
      for (predl = 0; predl < LgrpDiv->numLengthGroups(); predl++) {
        for (preyl = Suitability(prey)[predl].minCol();
            preyl < Suitability(prey)[predl].maxCol(); preyl++) {

          cons[inarea][prey][predl][preyl] = 0.0;
        }
      }
    }
  }

  //adjust the consumption by the multiplicative factor.
  tmp = Multiplicative / numsubsteps;  //use the multiplicative factor
  for (prey = 0; prey < this->numPreys(); prey++) {
    if (Preys(prey)->isPreyArea(area)) {
      for (predl = 0; predl < LgrpDiv->numLengthGroups(); predl++) {
        wanttoeat = tmp * prednumber[inarea][predl].N;
        for (preyl = Suitability(prey)[predl].minCol();
            preyl < Suitability(prey)[predl].maxCol(); preyl++) {
          cons[inarea][prey][predl][preyl] *= wanttoeat / totalcons[inarea][predl];
        }
      }
    }
  }

  //set the multiplicative constant
  for (predl = 0; predl < LgrpDiv->numLengthGroups(); predl++)
    scaler[inarea] += totalcons[inarea][predl];
  if (CurrentSubstep == numsubsteps)
    if (scaler[inarea] > verysmall)
      scaler[inarea] = 1.0 / scaler[inarea];

  //Inform the preys of the consumption.
  for (prey = 0; prey < this->numPreys(); prey++)
    if (Preys(prey)->isPreyArea(area))
      for (predl = 0; predl < LgrpDiv->numLengthGroups(); predl++)
        Preys(prey)->addBiomassConsumption(area, cons[inarea][prey][predl]);

  //set totalconsumption to the actual total consumption
  for (prey = 0; prey < this->numPreys(); prey++)
    if (Preys(prey)->isPreyArea(area))
      for (predl = 0; predl < LgrpDiv->numLengthGroups(); predl++)
        totalcons[inarea][predl] = tmp * prednumber[inarea][predl].N;
}

void TotalPredator::adjustConsumption(int area, int numsubsteps, int CurrentSubstep) {
  double maxRatio = pow(MaxRatioConsumed, numsubsteps);
  int check, over, prey, predl, preyl;
  int inarea = this->areaNum(area);
  double ratio, tmp;

  for (predl = 0; predl < LgrpDiv->numLengthGroups(); predl++)
    overcons[inarea][predl] = 0.0;

  over = 0;
  check = 0;
  for (prey = 0; prey < this->numPreys(); prey++) {
    if (Preys(prey)->isPreyArea(area)) {
      check = 1;
      if (Preys(prey)->checkOverConsumption(area)) {
        over = 1;
        for (predl = 0; predl < LgrpDiv->numLengthGroups(); predl++) {
          for (preyl = Suitability(prey)[predl].minCol();
              preyl < Suitability(prey)[predl].maxCol(); preyl++) {

            ratio = Preys(prey)->Ratio(area, preyl);
            if (ratio > maxRatio) {
              tmp = maxRatio / ratio;
              overcons[inarea][predl] += (1.0 - tmp) * cons[inarea][prey][predl][preyl];
              cons[inarea][prey][predl][preyl] *= tmp;
            }
          }
        }
      }
    }
  }

  if (over == 1)
    for (predl = 0; predl < LgrpDiv->numLengthGroups(); predl++)
      totalcons[inarea][predl] -= overcons[inarea][predl];

  if (check == 0) {
    //no prey found to consume so overcons set to actual consumption
    tmp = Multiplicative / numsubsteps;
    for (predl = 0; predl < LgrpDiv->numLengthGroups(); predl++)
      overcons[inarea][predl] = tmp * prednumber[inarea][predl].N;
  }

  for (predl = 0; predl < LgrpDiv->numLengthGroups(); predl++) {
    totalconsumption[inarea][predl] += totalcons[inarea][predl];
    overconsumption[inarea][predl] += overcons[inarea][predl];
  }

  for (prey = 0; prey < this->numPreys(); prey++)
    if (Preys(prey)->isPreyArea(area))
      for (predl = 0; predl < LgrpDiv->numLengthGroups(); predl++)
        for (preyl = Suitability(prey)[predl].minCol();
            preyl < Suitability(prey)[predl].maxCol(); preyl++)
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
      return Preys(prey)->getNumberPriorToEating(area);

  handle.logFailure("Error in totalpredator - failed to match prey", preyname);
  exit(EXIT_FAILURE);
}
