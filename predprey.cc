#include "stockpredator.h"
#include "keeper.h"
#include "mathfunc.h"
#include "readfunc.h"
#include "prey.h"
#include "lengthgroup.h"
#include "areatime.h"
#include "gadget.h"

//Function that calculates Maximum consumption.  Eq(l2) in Bogstad et.al.
double StockPredator::maxConsumption(double Length, const FormulaVector &maxcons, double Temperature) {
  return maxcons[0] * exp(Temperature * (maxcons[1] - Temperature *
    Temperature * maxcons[2])) * pow(Length, maxcons[3]);
}

void StockPredator::Eat(int area, double LengthOfStep, double Temperature,
  double Areasize, int CurrentSubstep, int numsubsteps) {

  int prey, predl, preyl;
  int Iarea = AreaNr[area];
  double tmpcons;

  for (predl = 0; predl < LgrpDiv->numLengthGroups(); predl++) {
    Phi[Iarea][predl] = 0.0;
    if (CurrentSubstep == 1) {
      fphi[Iarea][predl] = 0.0;
      totalconsumption[Iarea][predl] = 0.0;
      overconsumption[Iarea][predl] = 0.0;
      for (prey = 0; prey < numPreys(); prey++)
        if (Preys(prey)->IsInArea(area))
          for (preyl = Suitability(prey)[predl].minCol();
              preyl < Suitability(prey)[predl].maxCol(); preyl++)
            consumption[Iarea][prey][predl][preyl] = 0.0;
    }
  }

  this->calcMaxConsumption(Temperature, area, CurrentSubstep, numsubsteps, LengthOfStep);

  //Now maxconbylength contains the maximum consumption by length
  //Calculating Phi(L) and O(l,L,prey) (stored in consumption)
  for (prey = 0; prey < numPreys(); prey++) {
    if (Preys(prey)->IsInArea(area)) {
      if (Preys(prey)->Biomass(area) > verysmall) {
        for (predl = 0; predl < LgrpDiv->numLengthGroups(); predl++)
          for (preyl = Suitability(prey)[predl].minCol();
              preyl < Suitability(prey)[predl].maxCol(); preyl++) {
            cons[Iarea][prey][predl][preyl]
              = Suitability(prey)[predl][preyl] * Preys(prey)->Biomass(area, preyl);
            Phi[Iarea][predl] += cons[Iarea][prey][predl][preyl];
          }
      } else {
        for (predl = 0; predl < LgrpDiv->numLengthGroups(); predl++)
          for (preyl = Suitability(prey)[predl].minCol();
              preyl < Suitability(prey)[predl].maxCol(); preyl++)
            cons[Iarea][prey][predl][preyl] = 0.0;
      }
    }
  }

  //Calculating fphi(L) and Totalconsumption of predator in area
  for (predl = 0; predl < LgrpDiv->numLengthGroups(); predl++) {
    if (isZero(Phi[Iarea][predl] + halfFeedingValue * Areasize))
      fphI[Iarea][predl] = 0.0;
    else
      fphI[Iarea][predl] = Phi[Iarea][predl] / (Phi[Iarea][predl] + halfFeedingValue * Areasize);

    totalcons[Iarea][predl] = fphI[Iarea][predl] *
      maxconbylength[Iarea][predl] * Prednumber[Iarea][predl].N;
  }

  //Distributing the total consumption on the Preys()
  for (prey = 0; prey < numPreys(); prey++)
    if (Preys(prey)->IsInArea(area))
      if (Preys(prey)->Biomass(area) > verysmall)
        for (predl = 0; predl < LgrpDiv->numLengthGroups(); predl++)
          if (!(isZero(Phi[Iarea][predl]))) {
            tmpcons = totalcons[Iarea][predl] / Phi[Iarea][predl];
            for (preyl = Suitability(prey)[predl].minCol();
                preyl < Suitability(prey)[predl].maxCol(); preyl++)

              cons[Iarea][prey][predl][preyl] *= tmpcons;
          }

  //Add the calculated consumption to the preys in question
  for (prey = 0; prey < numPreys(); prey++)
    if (Preys(prey)->IsInArea(area))
      if (Preys(prey)->Biomass(area) > verysmall)
        for (predl = 0; predl < LgrpDiv->numLengthGroups(); predl++)
          Preys(prey)->addConsumption(area, cons[Iarea][prey][predl]);
}

//Check if any of the preys of the predator are eaten up.
//adjust the consumption according to that.
void StockPredator::adjustConsumption(int area, int numsubsteps, int CurrentSubstep) {
  double maxRatio = pow(MaxRatioConsumed, numsubsteps);
  int Iarea = AreaNr[area];
  int AnyPreyEatenUp = 0;
  int preyl, predl, prey;
  double ratio, rat1, rat2, tmp;

  for (predl = 0; predl < LgrpDiv->numLengthGroups(); predl++)
    overcons[Iarea][predl] = 0.0;

  for (prey = 0; prey < numPreys(); prey++)
    if (Preys(prey)->IsInArea(area))
      if (Preys(prey)->Biomass(area) > verysmall)
        if (Preys(prey)->TooMuchConsumption(area) == 1) {
          AnyPreyEatenUp = 1;
          for (predl = 0; predl < LgrpDiv->numLengthGroups(); predl++)
            for (preyl = Suitability(prey)[predl].minCol();
                preyl < Suitability(prey)[predl].maxCol(); preyl++) {
              ratio = Preys(prey)->Ratio(area, preyl);
              if (ratio > maxRatio) {
                tmp = maxRatio / ratio;
                overcons[Iarea][predl] += (1 - tmp) * cons[Iarea][prey][predl][preyl];
                cons[Iarea][prey][predl][preyl] *= tmp;
              }
            }
        }

  if (AnyPreyEatenUp == 1) {
    for (predl = 0; predl < LgrpDiv->numLengthGroups(); predl++) {
      if (totalcons[Iarea][predl] > verysmall) {
        ratio = 1.0 - overcons[Iarea][predl] / totalcons[Iarea][predl];
        fphI[Iarea][predl] *= ratio;
        totalcons[Iarea][predl] -= overcons[Iarea][predl];
      }
    }
  }

  //Add to consumption by predator change made after it was possible
  //to divide each timestep in number of parts.
  rat2 = 1.0 / CurrentSubstep;
  rat1 = 1.0 - rat2;
  for (predl = 0; predl < LgrpDiv->numLengthGroups(); predl++) {
    totalconsumption[Iarea][predl] += totalcons[Iarea][predl];
    overconsumption[Iarea][predl] += overcons[Iarea][predl];
    fphi[Iarea][predl] = (rat2 * fphI[Iarea][predl]) + (rat1 * fphi[Iarea][predl]);
  }
  for (prey = 0; prey < numPreys(); prey++)
    if (Preys(prey)->IsInArea(area))
      if (Preys(prey)->Biomass(area) > verysmall)
        for (predl = 0; predl < LgrpDiv->numLengthGroups(); predl++)
          for (preyl = Suitability(prey)[predl].minCol();
              preyl < Suitability(prey)[predl].maxCol(); preyl++)
            consumption[Iarea][prey][predl][preyl] +=
              cons[Iarea][prey][predl][preyl];
}

/* This function calculates the maximum consumption by length and puts in
 * maxconbylength. It also sets Alproportion. Breytt HB 30.3  With more than
 * one substep Alprop is only set on the last step.  This should possibly be
 * changed in later versions but that change would mean storage of Alkeys as
 * well as Alprop.  This change should though not have to be made.*/
void StockPredator::calcMaxConsumption(double Temperature, int area,
  int CurrentSubstep, int numsubsteps, double LengthOfStep) {

  const int inarea = AreaNr[area];
  int length, age;
  double tmp, timeratio = LengthOfStep / numsubsteps;

  if (CurrentSubstep == 1) {
    for (length = 0; length < maxconbylength.Ncol(); length++) {
      tmp = maxConsumption(LgrpDiv->meanLength(length), maxconsumption, Temperature);
      maxconbylength[inarea][length] = timeratio * tmp;
    }
  }

  if (CurrentSubstep == numsubsteps) {
    for (age = Alprop[inarea].minAge(); age <= Alprop[inarea].maxAge(); age++)
      for (length = Alprop[inarea].minLength(age);
          length < Alprop[inarea].maxLength(age); length++)
        if (!(isZero(Prednumber[inarea][length].N)))
          Alprop[inarea][age][length] = Alkeys[inarea][age][length].N /
            Prednumber[inarea][length].N;
        else
          Alprop[inarea][age][length] = 0.0;
  }
}
