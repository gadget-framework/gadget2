#include "spawner.h"
#include "errorhandler.h"
#include "keeper.h"
#include "areatime.h"
#include "readfunc.h"
#include "mathfunc.h"
#include "readword.h"
#include "readaggregation.h"
#include "gadget.h"
#include "global.h"

SpawnData::SpawnData(CommentStream& infile, int maxage, const LengthGroupDivision* const lgrpdiv,
  const IntVector& Areas, const AreaClass* const Area, const char* givenname,
  const TimeClass* const TimeInfo, Keeper* const keeper) : HasName(givenname), LivesOnAreas(Areas) {

  keeper->addString("spawner");
  int i, tmpint = 0;
  ratioscale = 1.0; //JMB used to scale the ratios to ensure that they sum to 1

  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  ifstream datafile;
  CommentStream subdata(datafile);
  functionname = new char[MaxStrLength];
  strncpy(functionname, "", MaxStrLength);

  spawnLgrpDiv = 0;
  LgrpDiv = new LengthGroupDivision(*lgrpdiv);
  if (LgrpDiv->Error())
    handle.logMessage(LOGFAIL, "Error in spawner - failed to create length group");
  int numlength = LgrpDiv->numLengthGroups();
  spawnFirstYear = TimeInfo->getFirstYear();
  spawnLastYear = TimeInfo->getLastYear();
  spawnProportion.resize(numlength, 0.0);
  spawnMortality.resize(numlength, 0.0);
  spawnWeightLoss.resize(numlength, 0.0);

  infile >> text >> ws;
  if ((strcasecmp(text, "spawnstep") != 0) && (strcasecmp(text, "spawnsteps") != 0))
    handle.logFileUnexpected(LOGFAIL, "spawnsteps", text);

  while (isdigit(infile.peek()) && !infile.eof()) {
    infile >> tmpint >> ws;
    spawnStep.resize(1, tmpint);
  }

  for (i = 0; i < spawnStep.Size(); i++)
    if (spawnStep[i] < 1 || spawnStep[i] > TimeInfo->numSteps())
      handle.logFileMessage(LOGFAIL, "invalid spawning step", spawnStep[i]);

  infile >> text >> ws;
  if ((strcasecmp(text, "spawnarea") != 0) && (strcasecmp(text, "spawnareas") != 0))
    handle.logFileUnexpected(LOGFAIL, "spawnareas", text);

  while (isdigit(infile.peek()) && !infile.eof()) {
    infile >> tmpint >> ws;
    spawnArea.resize(1, tmpint);
  }

  for (i = 0; i < spawnArea.Size(); i++)
    spawnArea[i] = Area->getInnerArea(spawnArea[i]);

  infile >> text >> ws;
  //JMB check for optional firstspwanyear and lastspawnyear values
  if (strcasecmp(text, "firstspawnyear") == 0) {
    infile >> spawnFirstYear >> text >> ws;
    if (spawnFirstYear < TimeInfo->getFirstYear())
      handle.logFileMessage(LOGFAIL, "invalid first spawning year", spawnFirstYear);
  }
  if (strcasecmp(text, "lastspawnyear") == 0) {
    infile >> spawnLastYear >> text >> ws;
    if (spawnLastYear > TimeInfo->getLastYear())
      handle.logFileMessage(LOGFAIL, "invalid last spawning year", spawnLastYear);
  }

  if (strcasecmp(text, "spawnstocksandratios") == 0) {
    onlyParent = 0;
    i = 0;
    infile >> text >> ws;
    while (strcasecmp(text, "proportionfunction") != 0 && !infile.eof()) {
      spawnStockNames.resize(new char[strlen(text) + 1]);
      strcpy(spawnStockNames[i], text);
      spawnRatio.resize(1, keeper);
      if (!(infile >> spawnRatio[i]))
        handle.logFileMessage(LOGFAIL, "invalid format for spawn ratio");
      spawnRatio[i].Inform(keeper);

      infile >> text >> ws;
      i++;
    }
  } else if (strcasecmp(text, "onlyparent") == 0) {
    onlyParent = 1;
    infile >> text >> ws;
  } else
    handle.logFileUnexpected(LOGFAIL, "spawnstocksandratios or onlyparent", text);

  if (infile.eof())
    handle.logFileEOFMessage(LOGFAIL);

  if (strcasecmp(text, "proportionfunction") != 0)
    handle.logFileUnexpected(LOGFAIL, "proportionfunction", text);

  infile >> text >> ws;
  if (strcasecmp(text, "constant") == 0)
    fnProportion = new ConstSelectFunc();
  else if (strcasecmp(text, "straightline") == 0)
    fnProportion = new StraightSelectFunc();
  else if (strcasecmp(text, "exponential") == 0)
    fnProportion = new ExpSelectFunc();
  else
    handle.logFileMessage(LOGFAIL, "unrecognised proportion function", text);
  fnProportion->readConstants(infile, TimeInfo, keeper);

  readWordAndValue(infile, "mortalityfunction", text);
  if (strcasecmp(text, "constant") == 0)
    fnMortality = new ConstSelectFunc();
  else if (strcasecmp(text, "straightline") == 0)
    fnMortality = new StraightSelectFunc();
  else if (strcasecmp(text, "exponential") == 0)
    fnMortality = new ExpSelectFunc();
  else
    handle.logFileMessage(LOGFAIL, "unrecognised mortality function", text);
  fnMortality->readConstants(infile, TimeInfo, keeper);

  readWordAndValue(infile, "weightlossfunction", text);
  if (strcasecmp(text, "constant") == 0)
    fnWeightLoss = new ConstSelectFunc();
  else if (strcasecmp(text, "straightline") == 0)
    fnWeightLoss = new StraightSelectFunc();
  else if (strcasecmp(text, "exponential") == 0)
    fnWeightLoss = new ExpSelectFunc();
  else
    handle.logFileMessage(LOGFAIL, "unrecognised weight loss function", text);
  fnWeightLoss->readConstants(infile, TimeInfo, keeper);

  if (!onlyParent) {
    infile >> text >> ws;
    if (strcasecmp(text, "recruitment") != 0)
      handle.logFileUnexpected(LOGFAIL, "recruitment", text);

    //read in the recruitment function details
    functionnumber = 0;
    infile >> functionname >> ws;

    if (strcasecmp(functionname, "simplessb") == 0) {
      functionnumber = 1;
      spawnParameters.resize(1, keeper);
    } else if (strcasecmp(functionname, "ricker") == 0) {
      functionnumber = 2;
      spawnParameters.resize(2, keeper);
    } else if (strcasecmp(functionname, "bevertonholt") == 0) {
      functionnumber = 3;
      spawnParameters.resize(2, keeper);
    } else if (strcasecmp(functionname, "fecundity") == 0) {
      functionnumber = 4;
      spawnParameters.resize(5, keeper);
    } else if (strcasecmp(functionname, "baleen") == 0) {
      functionnumber = 5;
      spawnParameters.resize(4, keeper);
    } else if (strcasecmp(functionname, "hockeystick") == 0) {
      functionnumber = 6;
      spawnParameters.resize(2, keeper);
    } else {
      handle.logFileMessage(LOGFAIL, "unrecognised recruitment function", functionname);
    }
    spawnParameters.read(infile, TimeInfo, keeper);

    //read in the details about the new stock
    stockParameters.resize(4, keeper);
    infile >> text >> ws;
    if (strcasecmp(text, "stockparameters") != 0)
      handle.logFileUnexpected(LOGFAIL, "stockparameters", text);
    stockParameters.read(infile, TimeInfo, keeper);

    //resize spawnNumbers to store details of the spawning stock biomass
    for (i = 0; i < areas.Size(); i++)
      spawnNumbers.resize(new DoubleMatrix(maxage + 1, numlength, 0.0));
  }

  infile >> ws;
  if (!infile.eof()) {
    infile >> text >> ws;
    handle.logFileUnexpected(LOGFAIL, "<end of file>", text);
  }
  handle.logMessage(LOGMESSAGE, "Read spawning data file");
  keeper->clearLast();
}

SpawnData::~SpawnData() {
  int i;
  for (i = 0; i < spawnStockNames.Size(); i++)
    delete[] spawnStockNames[i];
  for (i = 0; i < spawnNumbers.Size(); i++)
    delete spawnNumbers[i];
  for (i = 0; i < CI.Size(); i++)
    delete CI[i];
  delete LgrpDiv;
  delete spawnLgrpDiv;
  delete fnProportion;
  delete fnMortality;
  delete fnWeightLoss;
  delete[] functionname;
}

void SpawnData::setStock(StockPtrVector& stockvec) {
  int i, j, index;

  if (onlyParent)
    return;

  for (i = 0; i < stockvec.Size(); i++)
    for (j = 0; j < spawnStockNames.Size(); j++)
      if (strcasecmp(stockvec[i]->getName(), spawnStockNames[j]) == 0)
        spawnStocks.resize(stockvec[i]);

  if (spawnStocks.Size() != spawnStockNames.Size()) {
    handle.logMessage(LOGWARN, "Error in spawner - failed to match spawning stocks");
    for (i = 0; i < stockvec.Size(); i++)
      handle.logMessage(LOGWARN, "Error in spawner - found stock", stockvec[i]->getName());
    for (i = 0; i < spawnStockNames.Size(); i++)
      handle.logMessage(LOGWARN, "Error in spawner - looking for stock", spawnStockNames[i]);
    handle.logMessage(LOGFAIL, ""); //JMB this will exit gadget
  }

  //JMB ensure that the ratio vector is indexed in the right order
  ratioindex.resize(spawnStocks.Size(), 0);
  for (i = 0; i < spawnStocks.Size(); i++)
    for (j = 0; j < spawnStockNames.Size(); j++)
      if (strcasecmp(spawnStocks[i]->getName(), spawnStockNames[j]) == 0)
        ratioindex[i] = j;

  //JMB check that the spawned stocks are defined on all the areas
  spawnAge = 9999;
  double minlength = 9999.0;
  double maxlength = 0.0;
  double dl = 9999.0;
  for (i = 0; i < spawnStocks.Size(); i++) {
    index = 0;
    for (j = 0; j < spawnArea.Size(); j++)
      if (!spawnStocks[i]->isInArea(spawnArea[j]))
        index++;

    if (index != 0)
      handle.logMessage(LOGWARN, "Warning in spawner - spawned stock isnt defined on all areas");

    spawnAge = min(spawnStocks[i]->minAge(), spawnAge);
    minlength = min(spawnStocks[i]->getLengthGroupDiv()->minLength(), minlength);
    maxlength = max(spawnStocks[i]->getLengthGroupDiv()->maxLength(), maxlength);
    dl = min(spawnStocks[i]->getLengthGroupDiv()->dl(), dl);
  }

  spawnLgrpDiv = new LengthGroupDivision(minlength, maxlength, dl);
  if (spawnLgrpDiv->Error())
    handle.logMessage(LOGFAIL, "Error in spawner - failed to create length group");
  for (i = 0; i < spawnStocks.Size(); i++) {
    CI.resize(new ConversionIndex(spawnLgrpDiv, spawnStocks[i]->getLengthGroupDiv()));
    if (CI[i]->Error())
      handle.logMessage(LOGFAIL, "Error in spawner - error when checking length structure");
  }

  IntVector minlv(1, 0);
  IntVector sizev(1, spawnLgrpDiv->numLengthGroups());
  Storage.resize(areas.Size(), spawnAge, minlv, sizev);
  for (i = 0; i < Storage.Size(); i++)
    Storage[i].setToZero();
}

void SpawnData::Spawn(AgeBandMatrix& Alkeys, int area, const TimeClass* const TimeInfo) {

  if (!onlyParent)
    spawnParameters.Update(TimeInfo);

  int age, len;
  int inarea = this->areaNum(area);
  PopInfo pop;
  for (age = Alkeys.minAge(); age <= Alkeys.maxAge(); age++) {
    //subtract mortality and reduce the weight of the living ones.
    for (len = Alkeys.minLength(age); len < Alkeys.maxLength(age); len++) {
      if (!isZero(spawnProportion[len])) {
        pop = Alkeys[age][len] * spawnProportion[len];

        //calculate the spawning stock biomass if needed
        if (!onlyParent)
          (*spawnNumbers[inarea])[age][len] = calcSpawnNumber(age, len, pop.N, pop.W);

        pop *= exp(-spawnMortality[len]);
        pop.W -= (spawnWeightLoss[len] * pop.W);
        Alkeys[age][len] *= (1.0 - spawnProportion[len]);
        Alkeys[age][len] += pop;
      }
    }
  }
}

void SpawnData::addSpawnStock(int area, const TimeClass* const TimeInfo) {

  if (onlyParent)
    return;

  int s, age, len;
  int inarea = this->areaNum(area);
  double tmp, length, N, total, sum;

  //create a length distribution and mean weight for the new stock
  stockParameters.Update(TimeInfo);
 
  if (handle.getLogLevel() >= LOGWARN) {
    if (isZero(stockParameters[1]))
      handle.logMessage(LOGWARN, "Warning in spawner - invalid standard deviation for spawned stock", this->getName());
    if (stockParameters[0] < spawnLgrpDiv->minLength())
      handle.logMessage(LOGWARN, "Warning in spawner - mean length is less than minimum length for stock", this->getName());
    if (stockParameters[0] > spawnLgrpDiv->maxLength())
      handle.logMessage(LOGWARN, "Warning in spawner - mean length is greater than maximum length for stock", this->getName());
  }

  sum = 0.0;
  Storage[inarea].setToZero();
  if (stockParameters[1] > verysmall) {
    tmp = 1.0 / (2.0 * stockParameters[1] * stockParameters[1]);
    for (len = 0; len < spawnLgrpDiv->numLengthGroups(); len++) {
      length = spawnLgrpDiv->meanLength(len) - stockParameters[0];
      N = exp(-(length * length * tmp));
      Storage[inarea][spawnAge][len].N = N;
      sum += N;
    }
  }

  //calculate the total number of recruits and distribute this through the length groups
  if (!isZero(sum)) {
    tmp = 0.0;  //JMB dummy temperature of zero
    total = calcRecruitNumber(tmp, inarea) / sum;
    for (len = 0; len < spawnLgrpDiv->numLengthGroups(); len++) {
      length = spawnLgrpDiv->meanLength(len);
      Storage[inarea][spawnAge][len].N *= total;
      Storage[inarea][spawnAge][len].W = stockParameters[2] * pow(length, stockParameters[3]);
    }

    //add this to the spawned stocks
    for (s = 0; s < spawnStocks.Size(); s++) {
      if (!spawnStocks[s]->isInArea(area))
        handle.logMessage(LOGFAIL, "Error in spawner - spawned stock doesnt live on area", area);

      tmp = spawnRatio[ratioindex[s]] * ratioscale;
      spawnStocks[s]->Add(Storage[inarea], CI[s], area, tmp);
    }
  }
}

int SpawnData::isSpawnStepArea(int area, const TimeClass* const TimeInfo) {
  int i, j;

  if ((TimeInfo->getYear() < spawnFirstYear) || (TimeInfo->getYear() > spawnLastYear))
    return 0;

  for (i = 0; i < spawnStep.Size(); i++)
    for (j = 0; j < spawnArea.Size(); j++)
      if ((spawnStep[i] == TimeInfo->getStep()) && (spawnArea[j] == area))
        return 1;
  return 0;
}

void SpawnData::Reset(const TimeClass* const TimeInfo) {
  int i;

   spawnParameters.Update(TimeInfo);

  fnProportion->updateConstants(TimeInfo);
  if (fnProportion->didChange(TimeInfo)) {
    for (i = 0; i < LgrpDiv->numLengthGroups(); i++) {
      spawnProportion[i] = fnProportion->calculate(LgrpDiv->meanLength(i));
      if (spawnProportion[i] < 0.0) {
        handle.logMessage(LOGWARN, "Warning in spawner - function outside bounds", spawnProportion[i]);
        spawnProportion[i] = 0.0;
      }
      if (spawnProportion[i] > 1.0) {
        handle.logMessage(LOGWARN, "Warning in spawner - function outside bounds", spawnProportion[i]);
        spawnProportion[i] = 1.0;
      }
    }
  }

  fnWeightLoss->updateConstants(TimeInfo);
  if (fnWeightLoss->didChange(TimeInfo)) {
    for (i = 0; i < LgrpDiv->numLengthGroups(); i++) {
      spawnWeightLoss[i] = fnWeightLoss->calculate(LgrpDiv->meanLength(i));
      if (spawnWeightLoss[i] < 0.0) {
        handle.logMessage(LOGWARN, "Warning in spawner - function outside bounds", spawnWeightLoss[i]);
        spawnWeightLoss[i] = 0.0;
      }
      if (spawnWeightLoss[i] > 1.0) {
        handle.logMessage(LOGWARN, "Warning in spawner - function outside bounds", spawnWeightLoss[i]);
        spawnWeightLoss[i] = 1.0;
      }
    }
  }

  fnMortality->updateConstants(TimeInfo);
  if (fnMortality->didChange(TimeInfo)) {
    for (i = 0; i < LgrpDiv->numLengthGroups(); i++) {
      spawnMortality[i] = fnMortality->calculate(LgrpDiv->meanLength(i));
    }
  }

  //JMB check that the sum of the ratios is 1
  if ((!onlyParent) && (TimeInfo->getTime() == 1)) {
    ratioscale = 0.0;
    for (i = 0; i < spawnRatio.Size(); i++ )
      ratioscale += spawnRatio[i];

    if (isZero(ratioscale)) {
      handle.logMessage(LOGWARN, "Warning in spawner - specified ratios are zero");
      ratioscale = 1.0;
    } else if (isEqual(ratioscale, 1.0)) {
      // do nothing
    } else {
      handle.logMessage(LOGWARN, "Warning in spawner - scaling ratios using", ratioscale);
      ratioscale = 1.0 / ratioscale;
    }
  }

  if (handle.getLogLevel() >= LOGMESSAGE)
    handle.logMessage(LOGMESSAGE, "Reset spawning data for stock", this->getName());
}

double SpawnData::calcSpawnNumber(int age, int len, double number, double weight) {
  double temp = 0.0;
  switch (functionnumber) {
    case 1:
    case 2:
    case 3:
    case 6:
      temp = number * weight;
      break;
    case 4:
      temp = pow(LgrpDiv->meanLength(len), spawnParameters[1]) * pow(age, spawnParameters[2])
        * pow(number, spawnParameters[3]) * pow(weight, spawnParameters[4]);
      break;
    case 5:
      temp = number;
      break;
    default:
      handle.logMessage(LOGWARN, "Warning in spawner data - unrecognised recruitment function", functionname);
      break;
  }

  return temp;
}

double SpawnData::calcRecruitNumber(double temp, int inarea) {
  int age, len;
  double total = 0.0, ssb = 0.0;

  for (age = 0; age < (*spawnNumbers[inarea]).Nrow(); age++)
    for (len = 0; len < (*spawnNumbers[inarea]).Ncol(age); len++)
      ssb += (*spawnNumbers[inarea])[age][len];

  if (isZero(ssb))
    return total;

  switch (functionnumber) {
    case 1:
    case 4:
      total = ssb * spawnParameters[0];
      break;
    case 2:
      total = ssb * spawnParameters[0] * exp(-spawnParameters[1] * ssb);
      break;
    case 3:
      total = ssb * spawnParameters[0] / (spawnParameters[1] + ssb);
      break;
    case 5:
      if (isZero(spawnParameters[2])) {
        handle.logMessage(LOGWARN, "Warning in spawner - spawn parameter is zero");
        total = ssb * spawnParameters[0];
      } else {
        total = ssb * spawnParameters[0]
          * max(1.0 + spawnParameters[1] * (1.0 - pow(ssb / spawnParameters[2], spawnParameters[3])), 0.0);
      }
      break;
    case 6:
      if (ssb > spawnParameters[1]) {
        total = 1.0e4 * spawnParameters[0];
      } else if (isZero(spawnParameters[1])) {
        handle.logMessage(LOGWARN, "Warning in spawner - spawn parameter is zero");
        total = 1.0e4 * spawnParameters[0];
      } else {
        total = 1.0e4 * spawnParameters[0] * (ssb / spawnParameters[1]);
      }
      break;
    default:
      handle.logMessage(LOGWARN, "Warning in spawner calcrecruit- unrecognised recruitment function", functionname);
      break;
  }

  return total;
}

void SpawnData::Print(ofstream& outfile) const {

  if (onlyParent) {
    outfile << "\nSpawning data\n\t** Only modelling the affect on the parent stock **"
     << "\n\t** No recruits are created during the spawning process **\n";
    return;
  }

  int i;
  outfile << "\nSpawning data\n\tNames of spawned stocks:";
  for (i = 0; i < spawnStockNames.Size(); i++)
    outfile << sep << spawnStockNames[i];
  outfile << "\n\tRatio spawning into each stock:";
  for (i = 0; i < spawnRatio.Size(); i++)
    outfile << sep << (spawnRatio[ratioindex[i]] * ratioscale);
  outfile << "\n\t";
  spawnLgrpDiv->Print(outfile);
  outfile << "\tStored numbers:\n";
  for (i = 0; i < areas.Size(); i++) {
    outfile << "\tInternal area " << areas[i] << "\n\tNumbers\n";
    Storage[i].printNumbers(outfile);
    outfile << "\tMean weights\n";
    Storage[i].printWeights(outfile);
  }
}
