#include "stockpredator.h"
#include "keeper.h"
#include "mathfunc.h"
#include "readfunc.h"
#include "prey.h"
#include "lengthgroup.h"
#include "areatime.h"
#include "gadget.h"

//Function that calculates Maximum consumption.  Eq(l2) in Bogstad et.al.
//Maxconsumption stores the parameters used in the equation.
double StockPredator::MaxConsumption(double Length, const FormulaVector &Maxconsumption, double Temperature) {
  return Maxconsumption[0] * exp(Temperature * (Maxconsumption[1] - Temperature *
    Temperature * Maxconsumption[2])) * pow(Length, Maxconsumption[3]);
}

void StockPredator::Eat(int area, double LengthOfStep, double Temperature,
  double Areasize, int CurrentSubstep, int NrOfSubsteps) {

  int prey, predl, preyl;
  int Iarea = AreaNr[area];
  double tmpcons;

  for (predl = 0; predl < LgrpDiv->NoLengthGroups(); predl++) {
    Phi[Iarea][predl] = 0.0;
    if (CurrentSubstep == 1) {
      fphi[Iarea][predl] = 0;
      totalconsumption[Iarea][predl] = 0.0;
      overconsumption[Iarea][predl] = 0.0;
      for (prey = 0; prey < NoPreys(); prey++)
        if (Preys(prey)->IsInArea(area))
          for (preyl = Suitability(prey)[predl].Mincol();
              preyl < Suitability(prey)[predl].Maxcol(); preyl++)
            consumption[Iarea][prey][predl][preyl] = 0.0;
    }
  }

  this->CalcMaximumConsumption(Temperature, area, CurrentSubstep, NrOfSubsteps, LengthOfStep);

  //Now MaxconByLength contains the maximum consumption by length.
  //Calculating Phi(L) and O(l,L,prey) (stored in consumption)
  for (prey = 0; prey < NoPreys(); prey++) {
    if (Preys(prey)->IsInArea(area)) {
      if (Preys(prey)->Biomass(area) > 0) {
        for (predl = 0; predl < LgrpDiv->NoLengthGroups(); predl++)
          for (preyl = Suitability(prey)[predl].Mincol();
              preyl < Suitability(prey)[predl].Maxcol(); preyl++) {
            cons[Iarea][prey][predl][preyl]
              = Suitability(prey)[predl][preyl] * Preys(prey)->Biomass(area, preyl);
            Phi[Iarea][predl] += cons[Iarea][prey][predl][preyl];
          }
      } else {
        for (predl = 0; predl < LgrpDiv->NoLengthGroups(); predl++)
          for (preyl = Suitability(prey)[predl].Mincol();
              preyl < Suitability(prey)[predl].Maxcol(); preyl++)
            cons[Iarea][prey][predl][preyl] = 0.0;
      }
    }
  }

  //Calculating fphi(L) and Totalconsumption of predator in Area)
  for (predl = 0; predl < LgrpDiv->NoLengthGroups(); predl++) {
    fphI[Iarea][predl] = Phi[Iarea][predl] / (Phi[Iarea][predl] + halfFeedingValue * Areasize);
    totalcons[Iarea][predl] = fphI[Iarea][predl] *
      MaxconByLength[Iarea][predl] * Prednumber[Iarea][predl].N;
  }

  //Distributing the totalconsumption on the Preys().
  for (prey = 0; prey < NoPreys(); prey++)
    if (Preys(prey)->IsInArea(area))
      if (Preys(prey)->Biomass(area) > 0)
        for (predl = 0; predl < LgrpDiv->NoLengthGroups(); predl++)
          if (Phi[Iarea][predl] != 0) {
            tmpcons = totalcons[Iarea][predl] / Phi[Iarea][predl];
            for (preyl = Suitability(prey)[predl].Mincol();
                preyl < Suitability(prey)[predl].Maxcol(); preyl++)

              cons[Iarea][prey][predl][preyl] *= tmpcons;
          }


  //Add the calculated consumption to the preys in question using
  //memberfunctions in prey.
  for (prey = 0; prey < NoPreys(); prey++)
    if (Preys(prey)->IsInArea(area))
      if (Preys(prey)->Biomass(area) > 0)
        for (predl = 0; predl < LgrpDiv->NoLengthGroups(); predl++)
          Preys(prey)->AddConsumption(area, cons[Iarea][prey][predl]);
}

//Check if any of the preys of the predator are eaten up.
//Adjust the consumption according to that.
void StockPredator::AdjustConsumption(int area, int NrOfSubsteps, int CurrentSubstep) {

  double MaxRatioConsumed = pow(MAX_RATIO_CONSUMED, NrOfSubsteps);
  int Iarea = AreaNr[area];
  int AnyPreyEatenUp = 0;
  int preyl, predl, prey;
  double ratio, rat1, rat2, tmp;

  for (predl = 0; predl < LgrpDiv->NoLengthGroups(); predl++)
    overcons[Iarea][predl] = 0;

  for (prey = 0; prey < NoPreys(); prey++)
    if (Preys(prey)->IsInArea(area))
      if (Preys(prey)->Biomass(area) > 0)
        if (Preys(prey)->TooMuchConsumption(area) == 1) {
          AnyPreyEatenUp = 1;
          for (predl = 0; predl < LgrpDiv->NoLengthGroups(); predl++)
            for (preyl = Suitability(prey)[predl].Mincol();
                preyl < Suitability(prey)[predl].Maxcol(); preyl++) {
              ratio = Preys(prey)->Ratio(area, preyl);
              if (ratio > MaxRatioConsumed) {
                tmp = MaxRatioConsumed / ratio;
                overcons[Iarea][predl] += (1 - tmp) * cons[Iarea][prey][predl][preyl];
                cons[Iarea][prey][predl][preyl] *= tmp;
              }
            }
        }

  if (AnyPreyEatenUp == 1) {
    for (predl = 0; predl < LgrpDiv->NoLengthGroups(); predl++) {
      if (totalcons[Iarea][predl] > 0) {
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
  for (predl = 0; predl < LgrpDiv->NoLengthGroups(); predl++) {
    totalconsumption[Iarea][predl] += totalcons[Iarea][predl];
    overconsumption[Iarea][predl] += overcons[Iarea][predl];
    fphi[Iarea][predl] = rat2 * fphI[Iarea][predl] + rat1 * fphi[Iarea][predl];
  }
  for (prey = 0; prey < NoPreys(); prey++)
    if (Preys(prey)->IsInArea(area))
      if (Preys(prey)->Biomass(area) > 0)
        for (predl = 0; predl < LgrpDiv->NoLengthGroups(); predl++)
          for (preyl = Suitability(prey)[predl].Mincol();
              preyl < Suitability(prey)[predl].Maxcol(); preyl++)
            consumption[Iarea][prey][predl][preyl] +=
              cons[Iarea][prey][predl][preyl];
}

/* This function calculates the maximum consumption by length and puts in
 * MaxconByLength. It also sets Alproportion. Breytt HB 30.3  With more than
 * one substep Alprop is only set on the last step.  This should possibly be
 * changed in later versions but that change would mean storage of Alkeys as
 * well as Alprop.  This change should though not have to be made.*/
void StockPredator::CalcMaximumConsumption(double Temperature, int area,
  int CurrentSubstep, int NrOfSubsteps, double LengthOfStep) {

  const int inarea = AreaNr[area];
  int length, age;
  double tmp, timeratio = LengthOfStep / NrOfSubsteps;

  if (CurrentSubstep == 1) {
    for (length = 0; length < MaxconByLength.Ncol(); length++) {
      tmp = MaxConsumption(LgrpDiv->Meanlength(length), maxConsumption, Temperature);
      MaxconByLength[inarea][length] = timeratio * tmp;
    }
  }

  if (CurrentSubstep == NrOfSubsteps) {
    for (age = Alprop[inarea].Minage(); age <= Alprop[inarea].Maxage(); age++)
      for (length = Alprop[inarea].Minlength(age);
          length < Alprop[inarea].Maxlength(age); length++)
        if (Prednumber[inarea][length].N > 0)
          Alprop[inarea][age][length] = Alkeys[inarea][age][length].N /
            Prednumber[inarea][length].N;
        else
          Alprop[inarea][age][length] = 0.0;
  }
}
