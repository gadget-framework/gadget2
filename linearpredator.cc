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
  int prey, preyl;
  double tmp;
  
  int predl = 0;  //JMB there is only ever one length group ...
  totalcons[inarea][predl] = 0.0;

  scaler[inarea] = Multiplicative;
  tmp = prednumber[inarea][predl].N * Multiplicative * LengthOfStep / numsubsteps;

  for (prey = 0; prey < this->numPreys(); prey++) {
    if (Preys(prey)->isPreyArea(area)) {
      if ((handle.getLogLevel() >= LOGWARN) && (tmp > 10.0))
        handle.logMessage(LOGWARN, "Warning in linearpredator - excessive consumption required");

      for (preyl = Suitability(prey)[predl].minCol();
          preyl < Suitability(prey)[predl].maxCol(); preyl++) {
        cons[inarea][prey][predl][preyl] = tmp * 
          Suitability(prey)[predl][preyl] * Preys(prey)->getBiomass(area, preyl);
        totalcons[inarea][predl] += cons[inarea][prey][predl][preyl];
      }

    } else {
      for (preyl = Suitability(prey)[predl].minCol();
          preyl < Suitability(prey)[predl].maxCol(); preyl++) {
        cons[inarea][prey][predl][preyl] = 0.0;
      }
    }
  }

  //Inform the preys of the consumption.
  for (prey = 0; prey < this->numPreys(); prey++)
    if (Preys(prey)->isPreyArea(area))
      Preys(prey)->addBiomassConsumption(area, cons[inarea][prey][predl]);
}

void LinearPredator::adjustConsumption(int area, int numsubsteps, int CurrentSubstep) {
  double maxRatio = pow(MaxRatioConsumed, numsubsteps);
  int over, prey, preyl;
  double ratio, tmp;
  int inarea = this->areaNum(area);

  int predl = 0;  //JMB there is only ever one length group ...
  overcons[inarea][predl] = 0.0;

  over = 0;
  for (prey = 0; prey < this->numPreys(); prey++) {
    if (Preys(prey)->isPreyArea(area)) {
      if (Preys(prey)->checkOverConsumption(area)) {
        over = 1;
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

  if (over == 1)
    totalcons[inarea][predl] -= overcons[inarea][predl];

  totalconsumption[inarea][predl] += totalcons[inarea][predl];
  overconsumption[inarea][predl] += overcons[inarea][predl];

  for (prey = 0; prey < this->numPreys(); prey++)
    if (Preys(prey)->isPreyArea(area))
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

  handle.logMessage(LOGFAIL, "Error in linearpredator - failed to match prey", preyname);
  exit(EXIT_FAILURE);
}
