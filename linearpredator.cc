#include "linearpredator.h"
#include "keeper.h"
#include "prey.h"
#include "mathfunc.h"
#include "errorhandler.h"
#include "gadget.h"

extern ErrorHandler handle;

LinearPredator::LinearPredator(CommentStream& infile, const char* givenname,
  const IntVector& Areas, const LengthGroupDivision* const OtherLgrpDiv,
  const LengthGroupDivision* const GivenLgrpDiv, const TimeClass* const TimeInfo,
  Keeper* const keeper, double multi)
  : LengthPredator(givenname, Areas, OtherLgrpDiv, GivenLgrpDiv, multi) {

  keeper->addString("predator");
  keeper->addString(givenname);

  readSuitabilityMatrix(infile, "amount", TimeInfo, keeper);

  keeper->clearLast();
  keeper->clearLast();
  //Predator::setPrey will call resizeObjects.
}

void LinearPredator::Eat(int area, double LengthOfStep, double Temperature,
  double Areasize, int CurrentSubstep, int numsubsteps) {

  //The parameters Temperature and Areasize will not be used.
  const int inarea = AreaNr[area];
  int prey, predl, preyl;
  double tmp;

  for (predl = 0; predl < LgrpDiv->numLengthGroups(); predl++) {
    if (CurrentSubstep == 1) {
      totalconsumption[inarea][predl] = 0.0;
      overconsumption[inarea][predl] = 0.0;
      for (prey = 0; prey < numPreys(); prey++) {
        if (Preys(prey)->IsInArea(area)) {
          for (preyl = Suitability(prey)[predl].minCol();
              preyl < Suitability(prey)[predl].maxCol(); preyl++) {
            consumption[inarea][prey][predl][preyl] = 0.0;
          }
        }
      }
    }
  }

  for (predl = 0; predl < LgrpDiv->numLengthGroups(); predl++)
    totalcons[inarea][predl] = 0.0;
  scaler[inarea] = Multiplicative;
  tmp = Multiplicative * LengthOfStep / numsubsteps;

  for (prey = 0; prey < numPreys(); prey++) {
    if (Preys(prey)->IsInArea(area)) {
      if (Preys(prey)->Biomass(area) > verysmall) {
        for (predl = 0; predl < LgrpDiv->numLengthGroups(); predl++) {
          for (preyl = Suitability(prey)[predl].minCol();
              preyl < Suitability(prey)[predl].maxCol(); preyl++) {

            cons[inarea][prey][predl][preyl] = tmp *
              Suitability(prey)[predl][preyl] * Preys(prey)->Biomass(area, preyl) *
              Prednumber[inarea][predl].N * Prednumber[inarea][predl].W;

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
  }

  //Inform the preys of the consumption.
  for (prey = 0; prey < numPreys(); prey++) {
    if (Preys(prey)->IsInArea(area)) {
      if (Preys(prey)->Biomass(area) > verysmall) {
        for (predl = 0; predl < LgrpDiv->numLengthGroups(); predl++)
          Preys(prey)->addConsumption(area, cons[inarea][prey][predl]);
      }
    }
  }
}

void LinearPredator::adjustConsumption(int area, int numsubsteps, int CurrentSubstep) {
  double maxRatio = pow(MaxRatioConsumed, numsubsteps);
  int prey, predl, preyl;
  int AnyPreyEatenUp = 0;
  double ratio, tmp;
  const int inarea = AreaNr[area];

  for (predl = 0; predl < LgrpDiv->numLengthGroups(); predl++)
    overcons[inarea][predl] = 0.0;

  for (prey = 0; prey < numPreys(); prey++) {
    if (Preys(prey)->IsInArea(area)) {
      if (Preys(prey)->Biomass(area) > verysmall) {
        if (Preys(prey)->TooMuchConsumption(area) == 1) {
          AnyPreyEatenUp = 1;
          for (predl = 0; predl < LgrpDiv->numLengthGroups(); predl++) {
            for (preyl = Suitability(prey)[predl].minCol();
                preyl < Suitability(prey)[predl].maxCol(); preyl++) {
              ratio = Preys(prey)->Ratio(area, preyl);
              if (ratio > maxRatio) {
                tmp = maxRatio / ratio;
                overcons[inarea][predl] += (1 - tmp) * cons[inarea][prey][predl][preyl];
                cons[inarea][prey][predl][preyl] *= tmp;
              }
            }
          }
        }
      }
    }
  }

  //JMB - this was inside the for loop ...
  if (AnyPreyEatenUp == 1)
    for (predl = 0; predl < LgrpDiv->numLengthGroups(); predl++)
      totalcons[inarea][predl] -= overcons[inarea][predl];

  //Changes after division of timestep in substeps was possible.
  for (predl = 0; predl < LgrpDiv->numLengthGroups(); predl++)
    totalconsumption[inarea][predl] += totalcons[inarea][predl];

  for (prey = 0; prey < numPreys(); prey++) {
    if (Preys(prey)->IsInArea(area)) {
      if (Preys(prey)->Biomass(area) > verysmall) {
        for (predl = 0; predl < LgrpDiv->numLengthGroups(); predl++) {
          for (preyl = Suitability(prey)[predl].minCol();
              preyl < Suitability(prey)[predl].maxCol(); preyl++) {
            consumption[inarea][prey][predl][preyl] +=
              cons[inarea][prey][predl][preyl];
          }
        }
      }
    }
  }
}

void LinearPredator::Print(ofstream& outfile) const {
  outfile << "LinearPredator\n";
  PopPredator::Print(outfile);
}

const PopInfoVector& LinearPredator::NumberPriortoEating(int area, const char* preyname) const {
  int prey;
  for (prey = 0; prey < numPreys(); prey++)
    if (strcasecmp(Preyname(prey), preyname) == 0)
      return Preys(prey)->NumberPriortoEating(area);

  handle.logFailure("Error in linearpredator - failed to match prey", preyname);
  exit(EXIT_FAILURE);
}
