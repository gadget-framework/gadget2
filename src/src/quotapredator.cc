#include "quotapredator.h"
#include "keeper.h"
#include "prey.h"
#include "mathfunc.h"
#include "errorhandler.h"
#include "gadget.h"
#include "global.h"

QuotaPredator::QuotaPredator(CommentStream& infile, const char* givenname,
  const IntVector& Areas, const TimeClass* const TimeInfo, Keeper* const keeper, Formula multscaler)
  : LengthPredator(givenname, Areas, TimeInfo,keeper, multscaler) {

  type = QUOTAPREDATOR;
  keeper->addString("predator");
  keeper->addString(givenname);
  //first read in the suitability parameters
  this->readSuitability(infile, TimeInfo, keeper);

  int i, j;
  double tmp;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);

  functionnumber = 0;
  functionname = new char[MaxStrLength];
  strncpy(functionname, "", MaxStrLength);

  //the next entry in the input file should be the name of the quota function
  infile >> functionname >> ws;
  if (strcasecmp(functionname, "simple") == 0)
    functionnumber = 1;
  else if (strcasecmp(functionname, "simplesum") == 0)
    functionnumber = 2;
  else if (strcasecmp(functionname, "simpleselect") == 0)
    functionnumber = 3;
  else if (strcasecmp(functionname, "annual") == 0)
    functionnumber = 4;
  else if (strcasecmp(functionname, "annualsum") == 0)
    functionnumber = 5;
  else if (strcasecmp(functionname, "annualselect") == 0)
    functionnumber = 6;
  else
    handle.logFileMessage(LOGFAIL, "\nError in quotapredator - unrecognised function", functionname);

  //now read in the stock biomass levels
  infile >> text >> ws;
  if (strcasecmp(text, "biomasslevel") != 0)
    handle.logFileUnexpected(LOGFAIL, "biomasslevel", text);
  while (isdigit(infile.peek()) && !infile.eof()) {
    infile >> tmp >> ws;
    biomasslevel.resize(1, tmp);
  }

  if (biomasslevel.Size() == 0)
    handle.logMessage(LOGFAIL, "Error in quotapredator - missing quota data");

  //check the data to make sure that it is continuous and positive
  if (biomasslevel[0] < 0.0)
    handle.logFileMessage(LOGFAIL, "biomass levels must be positive");
  if (biomasslevel.Size() != 1)
    for (i = 1; i < biomasslevel.Size(); i++)
      if ((biomasslevel[i] - biomasslevel[i - 1]) < verysmall)
        handle.logFileMessage(LOGFAIL, "biomass levels must be strictly increasing");

  //finally read in the quota parameters
  infile >> text >> ws;
  if (strcasecmp(text, "quotalevel") != 0)
    handle.logFileUnexpected(LOGFAIL, "quotalevel", text);

  infile >> ws;
  keeper->addString("quota");
  quotalevel.resize(biomasslevel.Size() + 1, keeper);
  for (i = 0; i < quotalevel.Size(); i++)
    if (!(infile >> quotalevel[i]))
      handle.logFileMessage(LOGFAIL, "invalid format of quotalevel vector");
  quotalevel.Inform(keeper);
  keeper->clearLast();

  calcquota.resize(preference.Size(), 0.0);
  selectprey.resize(preference.Size(), 0);  //default to not using the prey

  //if we need to select the stocks used to calculate the fishing quota
  infile >> text >> ws;
  if ((functionnumber == 3) || (functionnumber == 6)) {
    if (strcasecmp(text, "selectstocks") != 0)
      handle.logFileUnexpected(LOGFAIL, "selectstocks", text);

    i = 0;
    CharPtrVector preynames;
    infile >> text >> ws;
    while (!infile.eof() && (strcasecmp(text, "amount") != 0)) {
      preynames.resize(new char[strlen(text) + 1]);
      strcpy(preynames[i++], text);
      infile >> text >> ws;
    }

    if (preynames.Size() == 0)
      handle.logFileMessage(LOGFAIL, "no stocks selected to calculate quota");

    //check that the prey names are unique
    for (i = 0; i < preynames.Size(); i++)
      for (j = 0; j < preynames.Size(); j++)
        if ((strcasecmp(preynames[i], preynames[j]) == 0) && (i != j))
          handle.logFileMessage(LOGFAIL, "repeated stock", preynames[i]);

    //work out which preys are needed to calculate the fishing quota
    int check = 0;
    for (i = 0; i < preynames.Size(); i++) {
      check = 0;
      for (j = 0; j < preference.Size(); j++) {
        if (strcasecmp(preynames[i], this->getPreyName(j)) == 0) {
          selectprey[j] = 1;
          check = 1;
        }
      }
      if (check == 0)
        handle.logFileMessage(LOGFAIL, "failed to match stock", preynames[i]);
    }

    //finally free the memory since the prey names are no longer needed
    for (i = 0; i < preynames.Size(); i++)
      delete[] preynames[i];
  }

  if (strcasecmp(text, "amount") != 0)
    handle.logFileUnexpected(LOGFAIL, "amount", text);

  keeper->clearLast();
  keeper->clearLast();
}

QuotaPredator::~QuotaPredator() {
  delete[] functionname;
}

void QuotaPredator::Eat(int area, const AreaClass* const Area, const TimeClass* const TimeInfo) {

  int inarea = this->areaNum(area);
  int prey, preyl;
  int predl = 0;  //JMB there is only ever one length group ...
  double tmp, bio;

  totalcons[inarea][predl] = 0.0;
  tmp = prednumber[inarea][predl].N * multi * TimeInfo->getTimeStepSize() / TimeInfo->numSubSteps();
  if (isZero(tmp)) { //JMB no predation takes place on this timestep
    for (prey = 0; prey < this->numPreys(); prey++)
      (*predratio[inarea])[prey][predl] = 0.0;
    return;
  }

  switch (functionnumber) {
    case 1:
      //Calculate the fishing level based on the available biomass of each stock
      for (prey = 0; prey < this->numPreys(); prey++) {
        if (this->getPrey(prey)->isPreyArea(area)) {
          (*predratio[inarea])[prey][predl] = tmp * calcQuota(this->getPrey(prey)->getTotalBiomass(area));
          if ((*predratio[inarea])[prey][predl] > 10.0) //JMB arbitrary value here ...
            handle.logMessage(LOGWARN, "Warning in quotapredator - excessive consumption required");

        } else
          (*predratio[inarea])[prey][predl] = 0.0;
      }
      break;

    case 2:
      //Calculate the fishing level based on the available biomass of all stocks
      bio = 0.0;
      for (prey = 0; prey < this->numPreys(); prey++)
        if (this->getPrey(prey)->isPreyArea(area))
          bio += this->getPrey(prey)->getTotalBiomass(area);

      tmp *= calcQuota(bio);
      if (tmp > 10.0) //JMB arbitrary value here ...
        handle.logMessage(LOGWARN, "Warning in quotapredator - excessive consumption required");
      for (prey = 0; prey < this->numPreys(); prey++) {
        if (this->getPrey(prey)->isPreyArea(area))
          (*predratio[inarea])[prey][predl] = tmp;
        else
          (*predratio[inarea])[prey][predl] = 0.0;
      }
      break;

    case 3:
      //Calculate the fishing level based on the available biomass of the selected stocks
      bio = 0.0;
      for (prey = 0; prey < this->numPreys(); prey++)
        if ((this->getPrey(prey)->isPreyArea(area)) && (selectprey[prey]))
          bio += this->getPrey(prey)->getTotalBiomass(area);

      tmp *= calcQuota(bio);
      if (tmp > 10.0) //JMB arbitrary value here ...
        handle.logMessage(LOGWARN, "Warning in quotapredator - excessive consumption required");
      for (prey = 0; prey < this->numPreys(); prey++) {
        if (this->getPrey(prey)->isPreyArea(area))
          (*predratio[inarea])[prey][predl] = tmp;
        else
          (*predratio[inarea])[prey][predl] = 0.0;
      }
      break;

    case 4:
      //Calculate the annual fishing level based on the available biomass of each stock
      for (prey = 0; prey < this->numPreys(); prey++) {
        if (this->getPrey(prey)->isPreyArea(area)) {
          if (TimeInfo->getStep() == 1)
            calcquota[prey] = calcQuota(this->getPrey(prey)->getTotalBiomass(area));

          //JMB this doesnt work if there is more than one stock since the value of quota will have changed
          (*predratio[inarea])[prey][predl] = calcquota[prey] * tmp;
          if ((*predratio[inarea])[prey][predl] > 10.0) //JMB arbitrary value here ...
            handle.logMessage(LOGWARN, "Warning in quotapredator - excessive consumption required");

        } else
          (*predratio[inarea])[prey][predl] = 0.0;
      }
      break;

    case 5:
      //Calculate the annual fishing level based on the available biomass of all stocks
      if (TimeInfo->getStep() == 1) {
        bio = 0.0;
        for (prey = 0; prey < this->numPreys(); prey++)
          if (this->getPrey(prey)->isPreyArea(area))
            bio += this->getPrey(prey)->getTotalBiomass(area);

        calcquota[0] = calcQuota(bio);  //JMB all quotas are the same
      }

      tmp *= calcquota[0];
      if (tmp > 10.0) //JMB arbitrary value here ...
        handle.logMessage(LOGWARN, "Warning in quotapredator - excessive consumption required");
      for (prey = 0; prey < this->numPreys(); prey++) {
        if (this->getPrey(prey)->isPreyArea(area))
          (*predratio[inarea])[prey][predl] = tmp;
        else
          (*predratio[inarea])[prey][predl] = 0.0;
      }
      break;

    case 6:
      //Calculate the annual fishing level based on the available biomass of the selected stocks
      if (TimeInfo->getStep() == 1) {
        bio = 0.0;
        for (prey = 0; prey < this->numPreys(); prey++)
          if ((this->getPrey(prey)->isPreyArea(area)) && (selectprey[prey]))
            bio += this->getPrey(prey)->getTotalBiomass(area);

        calcquota[0] = calcQuota(bio);  //JMB all quotas are the same
      }

      tmp *= calcquota[0];
      if (tmp > 10.0) //JMB arbitrary value here ...
        handle.logMessage(LOGWARN, "Warning in quotapredator - excessive consumption required");
      for (prey = 0; prey < this->numPreys(); prey++) {
        if (this->getPrey(prey)->isPreyArea(area))
          (*predratio[inarea])[prey][predl] = tmp;
        else
          (*predratio[inarea])[prey][predl] = 0.0;
      }
      break;

    default:
      handle.logMessage(LOGWARN, "Warning in quotapredator - unrecognised function", functionname);
      break;
  }

  for (prey = 0; prey < this->numPreys(); prey++) {
    if (isZero((*predratio[inarea])[prey][predl])) {
      for (preyl = 0; preyl < (*cons[inarea][prey])[predl].Size(); preyl++)
        (*cons[inarea][prey])[predl][preyl] = 0.0;

    } else {
      for (preyl = 0; preyl < (*cons[inarea][prey])[predl].Size(); preyl++) {
        (*cons[inarea][prey])[predl][preyl] = (*predratio[inarea])[prey][predl] *
          this->getSuitability(prey)[predl][preyl] * this->getPrey(prey)->getBiomass(area, preyl);
        totalcons[inarea][predl] += (*cons[inarea][prey])[predl][preyl];
      }
      //inform the preys of the consumption
      this->getPrey(prey)->addBiomassConsumption(area, (*cons[inarea][prey])[predl]);
    }
  }
}

void QuotaPredator::adjustConsumption(int area, const TimeClass* const TimeInfo) {
  int prey, preyl;
  int inarea = this->areaNum(area);
  int predl = 0;  //JMB there is only ever one length group ...
  overcons[inarea][predl] = 0.0;

  if (isZero(totalcons[inarea][predl])) //JMB no predation takes place on this timestep
    return;

  double maxRatio, tmp;
  maxRatio = TimeInfo->getMaxRatioConsumed();

  for (prey = 0; prey < this->numPreys(); prey++) {
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

void QuotaPredator::Print(ofstream& outfile) const {
  outfile << "QuotaPredator\n";
  PopPredator::Print(outfile);
}

double QuotaPredator::calcQuota(double biomass) {
  int i;
  double quota = 0.0;
  if (biomass < biomasslevel[0]) {
    quota = quotalevel[0];
  } else if (biomass > biomasslevel[biomasslevel.Size() - 1]) {
    quota = quotalevel[biomasslevel.Size()];
  } else {
    for (i = 1; i < biomasslevel.Size(); i++)
      if ((biomasslevel[i - 1] < biomass) && (biomass < biomasslevel[i]))
        quota = quotalevel[i];
  }

  if (quota < 0.0)
    handle.logMessage(LOGWARN, "Warning in quotapredator - negative quota", quota);
  return quota;
}

