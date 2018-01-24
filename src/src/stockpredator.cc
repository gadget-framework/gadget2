#include "stockpredator.h"
#include "keeper.h"
#include "errorhandler.h"
#include "readfunc.h"
#include "prey.h"
#include "areatime.h"
#include "suits.h"
#include "readword.h"
#include "gadget.h"
#include "global.h"

StockPredator::StockPredator(CommentStream& infile, const char* givenname, const IntVector& Areas,
  const LengthGroupDivision* const OtherLgrpDiv, const LengthGroupDivision* const GivenLgrpDiv,
  int minage, int numage, const TimeClass* const TimeInfo, Keeper* const keeper)
  : PopPredator(givenname, Areas, OtherLgrpDiv, GivenLgrpDiv) {

  type = STOCKPREDATOR;
  functionnumber = 0;
  keeper->addString("predator");
  keeper->addString(givenname);

  //first read in the suitability parameters
  this->readSuitability(infile, TimeInfo, keeper);

  //now we read in the prey preference parameters - should be one for each prey
  int i, check, count = 0;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);

  keeper->addString("preypreference");
  infile >> text >> ws;
  while (!infile.eof() && (((strcasecmp(text, "maxconsumption") != 0)) && (strcasecmp(text, "whaleconsumption") != 0))) {
    check = 0;
    for (i = 0; i < preference.Size(); i++) {
      if (strcasecmp(text, this->getPreyName(i)) == 0) {
        infile >> preference[i] >> ws;
        count++;
        check = 1;
      }
    }
    if (!check)
      handle.logMessage(LOGWARN, "Warning in stockpredator - failed to match prey", text);
    infile >> text >> ws;
  }
  if (count != preference.Size())
    handle.logMessage(LOGFAIL, "Error in stockpredator - missing prey preference data");
  preference.Inform(keeper);
  keeper->clearLast();

  //then read in the maximum consumption parameters
  keeper->addString("consumption");
  if (strcasecmp(text, "maxconsumption") == 0) {
    functionnumber = 1;
    consParam.resize(5, keeper);
    for (i = 0; i < 4; i++)
      if (!(infile >> consParam[i]))
        handle.logFileMessage(LOGFAIL, "invalid format for maxconsumption vector");

    readWordAndVariable(infile, "halffeedingvalue", consParam[4]);

  } else if (strcasecmp(text, "whaleconsumption") == 0) {
    functionnumber = 2;
    consParam.resize(16, keeper);
    for (i = 0; i < 15; i++)
      if (!(infile >> consParam[i]))
        handle.logFileMessage(LOGFAIL, "invalid format for whaleconsumption vector");

    readWordAndVariable(infile, "halffeedingvalue", consParam[15]);

  } else
    handle.logFileUnexpected(LOGFAIL, "maxconsumption", text);

  consParam.Inform(keeper);
  keeper->clearLast();

  //everything has been read from infile ... resize objects
  int numlength = LgrpDiv->numLengthGroups();
  int numarea = areas.Size();
  IntVector lower(numage, 0);
  IntVector agesize(numage, numlength);
  predAlkeys.resize(numarea, minage, lower, agesize);
  for (i = 0; i < predAlkeys.Size(); i++)
    predAlkeys[i].setToZero();
  maxcons.AddRows(numarea, numlength, 0.0);
  Phi.AddRows(numarea, numlength, 0.0);
  fphi.AddRows(numarea, numlength, 0.0);
  subfphi.AddRows(numarea, numlength, 0.0);

  keeper->clearLast();
  keeper->clearLast();
}

void StockPredator::Print(ofstream& outfile) const {
  int i, area;
  outfile << "\nStock predator\n";
  PopPredator::Print(outfile);
  outfile << "\n\tPredator age length keys\n";
  for (area = 0; area < areas.Size(); area++) {
    outfile << "\tInternal area " << areas[area] << "\n\tNumbers\n";
    predAlkeys[area].printNumbers(outfile);
    outfile << "\tMean weights\n";
    predAlkeys[area].printWeights(outfile);
  }
  outfile << "\n\tConsumption information\n";
  for (area = 0; area < areas.Size(); area++) {
    outfile << "\tPhi by length on internal area " << areas[area] << ":\n\t";
    for (i = 0; i < fphi.Ncol(area); i++)
      outfile << setw(smallwidth) << setprecision(smallprecision) << fphi[area][i] << sep;
    outfile << "\n\tMaximum consumption by length on internal area " << areas[area] << ":\n\t";
    for (i = 0; i < maxcons.Ncol(area); i++)
      outfile << setw(smallwidth) << setprecision(smallprecision) << maxcons[area][i] << sep;
    outfile << endl;
  }
  outfile << endl;
}

void StockPredator::Sum(const AgeBandMatrix& stockAlkeys, int area) {
  int inarea = this->areaNum(area);
  predAlkeys[inarea].setToZero();
  predAlkeys[inarea].Add(stockAlkeys, *CI);
  predAlkeys[inarea].sumColumns(prednumber[inarea]);
}

void StockPredator::Reset(const TimeClass* const TimeInfo) {
  PopPredator::Reset(TimeInfo);

  //check that the various parameters that can be estimated are sensible
  if ((handle.getLogLevel() >= LOGWARN) && (TimeInfo->getTime() == 1)) {
    int i, check;
    if (functionnumber == 1)
      for (i = 0; i < consParam.Size(); i++)
        if (consParam[i] < 0.0)
          handle.logMessage(LOGWARN, "Warning in stockpredator - negative consumption parameter", consParam[i]);
    check = 0;
    if (preference.Size() > 1)
      for (i = 1; i < preference.Size(); i++)
        if (!(isEqual(preference[0], preference[i])))
          check++;
    if (check != 0)
      handle.logMessage(LOGWARN, "Warning in stockpredator - preference parameters differ for", this->getName());
  }
}

void StockPredator::Eat(int area, const AreaClass* const Area, const TimeClass* const TimeInfo) {

  int prey, predl, preyl, check;
  int inarea = this->areaNum(area);
  double tmp;

  if (TimeInfo->getSubStep() == 1) {
    //this is the first substep of the timestep so need to reset things
    for (predl = 0; predl < LgrpDiv->numLengthGroups(); predl++) {
      Phi[inarea][predl] = 0.0;
      fphi[inarea][predl] = 0.0;
    }

    if (functionnumber == 1) {
      double temperature = Area->getTemperature(area, TimeInfo->getTime());
      tmp = exp(temperature * (consParam[1] - temperature * temperature * consParam[2]))
           * consParam[0] * TimeInfo->getTimeStepLength() / TimeInfo->numSubSteps();
      for (predl = 0; predl < LgrpDiv->numLengthGroups(); predl++)
        maxcons[inarea][predl] = tmp * pow(LgrpDiv->meanLength(predl), consParam[3]);

    } else if (functionnumber == 2) {
      double max1, max2, max3, l;
      for (predl = 0; predl < LgrpDiv->numLengthGroups(); predl++) {
        l = LgrpDiv->meanLength(predl);
        max1 = max(0.0, consParam[6] * (consParam[7] + consParam[8] * l));
        max2 = max(0.0, consParam[9] * (consParam[10] + consParam[11] * l));
        max3 = max(0.0, consParam[12] * (consParam[13] + consParam[14] * l));
        tmp = consParam[2] * pow(prednumber[inarea][predl].W, consParam[3])
             + consParam[4] * pow(l, consParam[5]) + max1 + max2 + max3;
        maxcons[inarea][predl] = consParam[0] * consParam[1] * tmp;
      }

    } else
      handle.logMessage(LOGWARN, "Warning in stockpredator - unrecognised consumption format");

  } else {
    //this is not the first substep of the timestep so only reset Phi
    for (predl = 0; predl < LgrpDiv->numLengthGroups(); predl++)
      Phi[inarea][predl] = 0.0;
  }

  //Now maxcons contains the maximum consumption by length
  //Calculating Phi(L) and O(l,L,prey) based on energy requirements
  for (prey = 0; prey < this->numPreys(); prey++) {
    check = 0;
    if (isEqual(preference[prey], 1.0))
      check = 1;

    if ((this->getPrey(prey)->isPreyArea(area)) && (!(isZero(this->getPrey(prey)->getEnergy())))) {
      for (predl = 0; predl < LgrpDiv->numLengthGroups(); predl++) {
        for (preyl = 0; preyl < (*cons[inarea][prey])[predl].Size(); preyl++) {
          tmp = this->getSuitability(prey)[predl][preyl] * this->getPrey(prey)->getEnergy()
                  * this->getPrey(prey)->getBiomass(area, preyl);

          //JMB - dont take the power if we dont have to
          if (!check)
            tmp = pow(tmp, preference[prey]);
          (*cons[inarea][prey])[predl][preyl] = tmp;
          Phi[inarea][predl] += tmp;
        }
      }

    } else {
      for (predl = 0; predl < LgrpDiv->numLengthGroups(); predl++)
        for (preyl = 0; preyl < (*cons[inarea][prey])[predl].Size(); preyl++)
          (*cons[inarea][prey])[predl][preyl] = 0.0;
    }
  }

  tmp = TimeInfo->getTimeStepLength() / TimeInfo->numSubSteps();
  if (functionnumber == 1)
    tmp *= consParam[4];
  else if (functionnumber == 2)
    tmp *= consParam[15];
  else
    handle.logMessage(LOGWARN, "Warning in stockpredator - unrecognised consumption format");

  //Calculating fphi(L) and totalcons of predator in area
  for (predl = 0; predl < LgrpDiv->numLengthGroups(); predl++) {
    if (isZero(tmp)) {
      subfphi[inarea][predl] = 1.0;
      totalcons[inarea][predl] = maxcons[inarea][predl] * prednumber[inarea][predl].N;
    } else if (isZero(Phi[inarea][predl]) || isZero(Phi[inarea][predl] + tmp)) {
      subfphi[inarea][predl] = 0.0;
      totalcons[inarea][predl] = 0.0;
    } else {
      subfphi[inarea][predl] = Phi[inarea][predl] / (Phi[inarea][predl] + tmp);
      totalcons[inarea][predl] = subfphi[inarea][predl]
        * maxcons[inarea][predl] * prednumber[inarea][predl].N;
    }
  }

  //Distributing the total consumption on the preys and converting to biomass
  for (prey = 0; prey < this->numPreys(); prey++) {
    if ((this->getPrey(prey)->isPreyArea(area)) && (!(isZero(this->getPrey(prey)->getEnergy())))) {
      for (predl = 0; predl < LgrpDiv->numLengthGroups(); predl++) {
        if (!(isZero(Phi[inarea][predl]))) {
          tmp = totalcons[inarea][predl] / (Phi[inarea][predl] * this->getPrey(prey)->getEnergy());
          for (preyl = 0; preyl < (*cons[inarea][prey])[predl].Size(); preyl++)
            (*cons[inarea][prey])[predl][preyl] *= tmp;

          //set the multiplicative constant
          (*predratio[inarea])[prey][predl] += tmp;
        }
      }
    }
  }

  //Add the calculated consumption to the preys in question
  for (prey = 0; prey < this->numPreys(); prey++)
    if (this->getPrey(prey)->isPreyArea(area))
      for (predl = 0; predl < LgrpDiv->numLengthGroups(); predl++)
        this->getPrey(prey)->addBiomassConsumption(area, (*cons[inarea][prey])[predl]);
}

//Check if any of the preys of the predator are eaten up.
//adjust the consumption according to that.
void StockPredator::adjustConsumption(int area, const TimeClass* const TimeInfo) {
  int inarea = this->areaNum(area);
  int numlen = LgrpDiv->numLengthGroups();
  int preyl, predl, prey;
  double maxRatio, tmp;

  maxRatio = TimeInfo->getMaxRatioConsumed();
  for (predl = 0; predl < numlen; predl++)
    overcons[inarea][predl] = 0.0;

  for (prey = 0; prey < this->numPreys(); prey++) {
    if (this->getPrey(prey)->isOverConsumption(area)) {
      hasoverconsumption[inarea] = 1;
      DoubleVector ratio = this->getPrey(prey)->getRatio(area);
      for (predl = 0; predl < numlen; predl++) {
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

  if (hasoverconsumption[inarea]) {
    for (predl = 0; predl < numlen; predl++) {
      overconsumption[inarea][predl] += overcons[inarea][predl];
      if (totalcons[inarea][predl] > verysmall) {
        tmp = 1.0 - (overcons[inarea][predl] / totalcons[inarea][predl]);
        subfphi[inarea][predl] *= tmp;
        totalcons[inarea][predl] -= overcons[inarea][predl];
      }
    }
  }

  for (predl = 0; predl < numlen; predl++)
    totalconsumption[inarea][predl] += totalcons[inarea][predl];

  if (TimeInfo->numSubSteps() != 1) {
    double ratio1, ratio2;
    ratio2 = 1.0 / TimeInfo->getSubStep();
    ratio1 = 1.0 - ratio2;
    for (predl = 0; predl < numlen; predl++)
      fphi[inarea][predl] = (ratio2 * subfphi[inarea][predl]) + (ratio1 * fphi[inarea][predl]);

  } else
    for (predl = 0; predl < numlen; predl++)
      fphi[inarea][predl] = subfphi[inarea][predl];

  for (prey = 0; prey < this->numPreys(); prey++)
    if (this->getPrey(prey)->isPreyArea(area))
      for (predl = 0; predl < numlen; predl++)
        for (preyl = 0; preyl < (*cons[inarea][prey])[predl].Size(); preyl++)
          (*consumption[inarea][prey])[predl][preyl] += (*cons[inarea][prey])[predl][preyl];
}
