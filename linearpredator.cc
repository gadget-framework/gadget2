#include "linearpredator.h"
#include "keeper.h"
#include "prey.h"
#include "mathfunc.h"
#include "errorhandler.h"
#include "gadget.h"

extern ErrorHandler handle;

LinearPredator::LinearPredator(CommentStream& infile, const char* givenname,
  const IntVector& Areas, const TimeClass* const TimeInfo, Keeper* const keeper, Formula multi)
  : LengthPredator(givenname, Areas, keeper, multi) {

  keeper->addString("predator");
  keeper->addString(givenname);

  this->readSuitability(infile, "amount", TimeInfo, keeper);

  keeper->clearLast();
  keeper->clearLast();
}

void LinearPredator::Eat(int area, double LengthOfStep, double Temperature,
  double Areasize, int CurrentSubstep, int numsubsteps) {

  int inarea = this->areaNum(area);
  int prey, predl, preyl;
  double tmp;

  for (predl = 0; predl < LgrpDiv->numLengthGroups(); predl++)
    totalcons[inarea][predl] = 0.0;

  scaler[inarea] = Multiplicative;
  tmp = Multiplicative * LengthOfStep / numsubsteps;

  for (prey = 0; prey < this->numPreys(); prey++) {
    if (Preys(prey)->isPreyArea(area)) {
      for (predl = 0; predl < LgrpDiv->numLengthGroups(); predl++) {
        if (tmp * prednumber[inarea][predl].N > 10.0)
          handle.logWarning("Warning in linearpredator - excessive consumption required");

        for (preyl = Suitability(prey)[predl].minCol();
            preyl < Suitability(prey)[predl].maxCol(); preyl++) {

          cons[inarea][prey][predl][preyl] = tmp * prednumber[inarea][predl].N *
            Suitability(prey)[predl][preyl] * Preys(prey)->getBiomass(area, preyl);

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

  //Inform the preys of the consumption.
  for (prey = 0; prey < this->numPreys(); prey++)
    if (Preys(prey)->isPreyArea(area))
      for (predl = 0; predl < LgrpDiv->numLengthGroups(); predl++)
        Preys(prey)->addBiomassConsumption(area, cons[inarea][prey][predl]);
}

void LinearPredator::adjustConsumption(int area, int numsubsteps, int CurrentSubstep) {
  double maxRatio = pow(MaxRatioConsumed, numsubsteps);
  int over, prey, predl, preyl;
  double ratio, tmp;
  int inarea = this->areaNum(area);

  for (predl = 0; predl < LgrpDiv->numLengthGroups(); predl++)
    overcons[inarea][predl] = 0.0;

  over = 0;
  for (prey = 0; prey < this->numPreys(); prey++) {
    if (Preys(prey)->isPreyArea(area)) {
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

void LinearPredator::Print(ofstream& outfile) const {
  outfile << "LinearPredator\n";
  PopPredator::Print(outfile);
}

const PopInfoVector& LinearPredator::getNumberPriorToEating(int area, const char* preyname) const {
  int prey;
  for (prey = 0; prey < this->numPreys(); prey++)
    if (strcasecmp(getPreyName(prey), preyname) == 0)
      return Preys(prey)->getNumberPriorToEating(area);

  handle.logFailure("Error in linearpredator - failed to match prey", preyname);
  exit(EXIT_FAILURE);
}
