#include "stockpredator.h"
#include "keeper.h"
#include "errorhandler.h"
#include "readfunc.h"
#include "prey.h"
#include "areatime.h"
#include "suits.h"
#include "readword.h"
#include "gadget.h"

extern ErrorHandler handle;

StockPredator::StockPredator(CommentStream& infile, const char* givenname, const IntVector& Areas,
  const LengthGroupDivision* const OtherLgrpDiv, const LengthGroupDivision* const GivenLgrpDiv,
  int minage, int maxage, const TimeClass* const TimeInfo, Keeper* const keeper)
  : PopPredator(givenname, Areas, OtherLgrpDiv, GivenLgrpDiv),
    maxconbylength(areas.Size(), GivenLgrpDiv->numLengthGroups(), 0.0) {

  keeper->addString("predator");
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);

  infile >> text >> ws;
  if (!(strcasecmp(text, "suitability") == 0))
    handle.Unexpected("suitability", text);

  this->readSuitability(infile, "maxconsumption", TimeInfo, keeper);

  keeper->addString("maxconsumption");
  maxconsumption.resize(4, keeper);
  if (!(infile >> maxconsumption))
    handle.Message("Error in stock file - incorrect format of maxconsumption vector");
  maxconsumption.Inform(keeper);

  keeper->clearLast();
  keeper->addString("halffeedingvalue");
  readWordAndFormula(infile, "halffeedingvalue", halfFeedingValue);
  halfFeedingValue.Inform(keeper);
  keeper->clearLast();
  keeper->clearLast();

  //Everything read from infile.
  IntVector size(maxage - minage + 1, GivenLgrpDiv->numLengthGroups());
  IntVector minlength(maxage - minage + 1, 0);

  int numlength = LgrpDiv->numLengthGroups();
  int numarea = areas.Size();
  Alkeys.resize(numarea, minage, minlength, size);
  BandMatrix bm(minlength, size, minage); //default initialization to 0.
  Alprop.resize(numarea, bm);
  Phi.AddRows(numarea, numlength, 0.0);
  fphi.AddRows(numarea, numlength, 0.0);
  fphI.AddRows(numarea, numlength, 0.0);
}

void StockPredator::Print(ofstream& outfile) const {
  int i, area;
  outfile << "\nStock predator\n";
  PopPredator::Print(outfile);
  for (area = 0; area < areas.Size(); area++) {
    outfile << "\tPhi on internal area " << areas[area] << ":\n\t";
    for (i = 0; i < Phi.Ncol(area); i++) {
      outfile.precision(smallprecision);
      outfile.width(smallwidth);
      outfile << fphI[area][i] << sep;
    }
    outfile << endl;
  }
  for (area = 0; area < areas.Size(); area++) {
    outfile << "\tAlkeys (numbers) on internal area " << areas[area] << ":\n";
    Alkeys[area].printNumbers(outfile);
  }
  for (area = 0; area < areas.Size(); area++) {
    outfile << "\tAlkeys (mean weights) on internal area " << areas[area] << ":\n";
    Alkeys[area].printWeights(outfile);
  }
  for (area = 0; area < areas.Size(); area++) {
    outfile << "\tAge-length proportion on internal area " << areas[area] << ":\n";
    Alprop[area].Print(outfile);
  }
  for (area = 0; area < areas.Size(); area++) {
    outfile << "\tMaximum consumption by length on internal area " << areas[area] << ":\n\t";
    for (i = 0; i < maxconbylength.Ncol(); i++) {
      outfile.precision(smallprecision);
      outfile.width(smallwidth);
      outfile << sep << maxconbylength[area][i];
    }
    outfile << endl;
  }
  outfile << endl;
}

void StockPredator::Sum(const AgeBandMatrix& stock, int area) {
  int inarea = this->areaNum(area);
  Alkeys[inarea].setToZero();
  Alkeys[inarea].Add(stock, *CI);
  Alkeys[inarea].sumColumns(prednumber[inarea]);
}

void StockPredator::Reset(const TimeClass* const TimeInfo) {
  PopPredator::Reset(TimeInfo);
  int a, l, age;
  if (TimeInfo->CurrentTime() == 1) {
    for (a = 0; a < areas.Size(); a++) {
      for (l = 0; l < LgrpDiv->numLengthGroups(); l++) {
        Phi[a][l] = 0.0;
        fphi[a][l] = 0.0;
        maxconbylength[a][l] = 0.0;
      }
    }
    for (a = 0; a < areas.Size(); a++) {
      for (age = Alkeys[a].minAge(); age <= Alkeys[a].maxAge(); age++) {
        for (l = Alkeys[a].minLength(age); l < Alkeys[a].maxLength(age); l++) {
          Alkeys[a][age][l].N = 0.0;
          Alkeys[a][age][l].W = 0.0;
          Alprop[a][age][l] = 0.0;
        }
      }
    }
  }
}

const PopInfoVector& StockPredator::getNumberPriorToEating(int area, const char* preyname) const {
  int prey;
  for (prey = 0; prey < this->numPreys(); prey++)
    if (strcasecmp(Preyname(prey), preyname) == 0)
      return Preys(prey)->getNumberPriorToEating(area);

  handle.logFailure("Error in stockpredator - failed to match prey", preyname);
  exit(EXIT_FAILURE);
}

double StockPredator::maxConsumption(double Length, const FormulaVector &maxcons, double Temperature) {
  return maxcons[0] * exp(Temperature * (maxcons[1] - Temperature *
    Temperature * maxcons[2])) * pow(Length, maxcons[3]);
}

void StockPredator::Eat(int area, double LengthOfStep, double Temperature,
  double Areasize, int CurrentSubstep, int numsubsteps) {

  int prey, predl, preyl;
  int inarea = this->areaNum(area);
  double tmpcons;

  for (predl = 0; predl < LgrpDiv->numLengthGroups(); predl++) {
    Phi[inarea][predl] = 0.0;
    if (CurrentSubstep == 1)
      fphi[inarea][predl] = 0.0;
  }

  this->calcMaxConsumption(Temperature, inarea, CurrentSubstep, numsubsteps, LengthOfStep);

  //Now maxconbylength contains the maximum consumption by length
  //Calculating Phi(L) and O(l,L,prey) (stored in consumption)
  for (prey = 0; prey < this->numPreys(); prey++) {
    if (Preys(prey)->isPreyArea(area)) {
      for (predl = 0; predl < LgrpDiv->numLengthGroups(); predl++) {
        for (preyl = Suitability(prey)[predl].minCol();
            preyl < Suitability(prey)[predl].maxCol(); preyl++) {

          cons[inarea][prey][predl][preyl]
            = Suitability(prey)[predl][preyl] * Preys(prey)->getBiomass(area, preyl);
          Phi[inarea][predl] += cons[inarea][prey][predl][preyl];
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

  //Calculating fphi(L) and Totalconsumption of predator in area
  for (predl = 0; predl < LgrpDiv->numLengthGroups(); predl++) {
    if (isZero(Phi[inarea][predl] + halfFeedingValue * Areasize))
      fphI[inarea][predl] = 0.0;
    else
      fphI[inarea][predl] = Phi[inarea][predl] / (Phi[inarea][predl] + halfFeedingValue * Areasize);

    totalcons[inarea][predl] = fphI[inarea][predl] *
      maxconbylength[inarea][predl] * prednumber[inarea][predl].N;
  }

  //Distributing the total consumption on the Preys()
  for (prey = 0; prey < this->numPreys(); prey++) {
    if (Preys(prey)->isPreyArea(area)) {
      for (predl = 0; predl < LgrpDiv->numLengthGroups(); predl++) {
        if (!(isZero(Phi[inarea][predl]))) {
          tmpcons = totalcons[inarea][predl] / Phi[inarea][predl];
          for (preyl = Suitability(prey)[predl].minCol();
              preyl < Suitability(prey)[predl].maxCol(); preyl++) {

              cons[inarea][prey][predl][preyl] *= tmpcons;
          }
        }
      }
    }
  }

  //Add the calculated consumption to the preys in question
  for (prey = 0; prey < this->numPreys(); prey++)
    if (Preys(prey)->isPreyArea(area))
      for (predl = 0; predl < LgrpDiv->numLengthGroups(); predl++)
        Preys(prey)->addBiomassConsumption(area, cons[inarea][prey][predl]);
}

//Check if any of the preys of the predator are eaten up.
//adjust the consumption according to that.
void StockPredator::adjustConsumption(int area, int numsubsteps, int CurrentSubstep) {
  double maxRatio = pow(MaxRatioConsumed, numsubsteps);
  int inarea = this->areaNum(area);
  int AnyPreyEatenUp = 0;
  int preyl, predl, prey;
  double ratio, rat1, rat2, tmp;

  for (predl = 0; predl < LgrpDiv->numLengthGroups(); predl++)
    overcons[inarea][predl] = 0.0;

  for (prey = 0; prey < this->numPreys(); prey++) {
    if (Preys(prey)->isPreyArea(area)) {
      if (Preys(prey)->TooMuchConsumption(area) == 1) {
        AnyPreyEatenUp = 1;
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

  if (AnyPreyEatenUp == 1) {
    for (predl = 0; predl < LgrpDiv->numLengthGroups(); predl++) {
      if (totalcons[inarea][predl] > verysmall) {
        ratio = 1.0 - overcons[inarea][predl] / totalcons[inarea][predl];
        fphI[inarea][predl] *= ratio;
        totalcons[inarea][predl] -= overcons[inarea][predl];
      }
    }
  }

  //Add to consumption by predator change made after it was possible
  //to divide each timestep in number of parts.
  rat2 = 1.0 / CurrentSubstep;
  rat1 = 1.0 - rat2;
  for (predl = 0; predl < LgrpDiv->numLengthGroups(); predl++) {
    totalconsumption[inarea][predl] += totalcons[inarea][predl];
    overconsumption[inarea][predl] += overcons[inarea][predl];
    fphi[inarea][predl] = (rat2 * fphI[inarea][predl]) + (rat1 * fphi[inarea][predl]);
  }
  for (prey = 0; prey < this->numPreys(); prey++)
    if (Preys(prey)->isPreyArea(area))
      for (predl = 0; predl < LgrpDiv->numLengthGroups(); predl++)
        for (preyl = Suitability(prey)[predl].minCol();
            preyl < Suitability(prey)[predl].maxCol(); preyl++)
          consumption[inarea][prey][predl][preyl] += cons[inarea][prey][predl][preyl];
}

/* This function calculates the maximum consumption by length and puts in
 * maxconbylength. It also sets Alproportion. Breytt HB 30.3  With more than
 * one substep Alprop is only set on the last step.  This should possibly be
 * changed in later versions but that change would mean storage of Alkeys as
 * well as Alprop.  This change should though not have to be made.*/
void StockPredator::calcMaxConsumption(double Temperature, int inarea,
  int CurrentSubstep, int numsubsteps, double LengthOfStep) {

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
        if (!(isZero(prednumber[inarea][length].N)))
          Alprop[inarea][age][length] = Alkeys[inarea][age][length].N /
            prednumber[inarea][length].N;
        else
          Alprop[inarea][age][length] = 0.0;
  }
}
