#include "spawner.h"
#include "errorhandler.h"
#include "keeper.h"
#include "areatime.h"
#include "readfunc.h"
#include "mathfunc.h"
#include "readword.h"
#include "readaggregation.h"
#include "gadget.h"

extern ErrorHandler handle;

SpawnData::SpawnData(CommentStream& infile, int maxage, const LengthGroupDivision* const lgrpdiv,
  const IntVector& Areas, const AreaClass* const Area, const TimeClass* const TimeInfo,
  Keeper* const keeper) : LivesOnAreas(Areas) {

  keeper->addString("spawner");
  int i, tmpint;
  double tmpratio;

  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  ifstream datafile;
  CommentStream subdata(datafile);
  functionname = new char[MaxStrLength];
  strncpy(functionname, "", MaxStrLength);

  spawnLgrpDiv = 0;
  LgrpDiv = new LengthGroupDivision(*lgrpdiv);
  int numlength = LgrpDiv->numLengthGroups();
  spawnNumbers.AddRows(maxage + 1, numlength, 0.0);
  spawnProportion.resize(numlength, 0.0);
  spawnMortality.resize(numlength, 0.0);
  spawnWeightLoss.resize(numlength, 0.0);

  infile >> text >> ws;
  if (!((strcasecmp(text, "spawnstep") == 0) || (strcasecmp(text, "spawnsteps") == 0)))
    handle.logFileUnexpected(LOGFAIL, "spawnsteps", text);

  while (isdigit(infile.peek()) && !infile.eof()) {
    infile >> tmpint >> ws;
    spawnStep.resize(1, tmpint);
  }

  for (i = 0; i < spawnStep.Size(); i++)
    if (spawnStep[i] < 1 || spawnStep[i] > TimeInfo->numSteps())
      handle.logFileMessage(LOGFAIL, "invalid spawning step", spawnStep[i]);

  infile >> text >> ws;
  if (!((strcasecmp(text, "spawnarea") == 0) || (strcasecmp(text, "spawnareas") == 0)))
    handle.logFileUnexpected(LOGFAIL, "spawnareas", text);

  while (isdigit(infile.peek()) && !infile.eof()) {
    infile >> tmpint >> ws;
    spawnArea.resize(1, tmpint);
  }

  for (i = 0; i < spawnArea.Size(); i++)
    spawnArea[i] = Area->InnerArea(spawnArea[i]);

  infile >> text;
  if (strcasecmp(text, "spawnstocksandratios") == 0) {
    onlyParent = 0;
    i = 0;
    infile >> text >> ws;
    while (strcasecmp(text, "proportionfunction") != 0 && !infile.eof()) {
      spawnStockNames.resize(new char[strlen(text) + 1]);
      strcpy(spawnStockNames[i], text);
      infile >> tmpratio >> text >> ws;
      spawnRatio.resize(1, tmpratio);
      i++;
    }
  } else if (strcasecmp(text, "onlyparent") == 0) {
    onlyParent = 1;
    infile >> text >> ws;
  } else
    handle.logFileUnexpected(LOGFAIL, "spawnstocksandratios or onlyparent", text);

  if (infile.eof())
    handle.logFileEOFMessage(LOGFAIL);

  if (strcasecmp(text, "proportionfunction") == 0) {
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
  } else
    handle.logFileUnexpected(LOGFAIL, "proportionfunction", text);

  infile >> text;
  if (strcasecmp(text, "mortalityfunction") == 0) {
    infile >> text >> ws;
    if (strcasecmp(text, "constant") == 0)
      fnMortality = new ConstSelectFunc();
    else if (strcasecmp(text, "straightline") == 0)
      fnMortality = new StraightSelectFunc();
    else if (strcasecmp(text, "exponential") == 0)
      fnMortality = new ExpSelectFunc();
    else
      handle.logFileMessage(LOGFAIL, "unrecognised mortality function", text);

    fnMortality->readConstants(infile, TimeInfo, keeper);
  } else
    handle.logFileUnexpected(LOGFAIL, "mortalityfunction", text);

  infile >> text;
  if (strcasecmp(text, "weightlossfunction") == 0) {
    infile >> text >> ws;
    if (strcasecmp(text, "constant") == 0)
      fnWeightLoss = new ConstSelectFunc();
    else if (strcasecmp(text, "straightline") == 0)
      fnWeightLoss = new StraightSelectFunc();
    else if (strcasecmp(text, "exponential") == 0)
      fnWeightLoss = new ExpSelectFunc();
    else
      handle.logFileMessage(LOGFAIL, "unrecognised weight loss function", text);

    fnWeightLoss->readConstants(infile, TimeInfo, keeper);
  } else
    handle.logFileUnexpected(LOGFAIL, "weightlossfunction", text);

  if (onlyParent == 0) {
    infile >> text >> ws;
    if (!(strcasecmp(text, "recruitment") == 0))
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
    } else
      handle.logFileMessage(LOGFAIL, "unrecognised recruitment function", functionname);

    spawnParameters.read(infile, TimeInfo, keeper);

    //read in the details about the new stock
    stockParameters.resize(4, keeper);
    infile >> text >> ws;
    if (strcasecmp(text, "stockparameters") == 0)
      stockParameters.read(infile, TimeInfo, keeper);
    else
      handle.logFileUnexpected(LOGFAIL, "stockparameters", text);
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
  DoubleVector tmpratio;

  if (onlyParent == 1)
    return;

  for (i = 0; i < stockvec.Size(); i++)
    for (j = 0; j < spawnStockNames.Size(); j++)
      if (strcasecmp(stockvec[i]->getName(), spawnStockNames[j]) == 0) {
        spawnStocks.resize(stockvec[i]);
        tmpratio.resize(1, spawnRatio[j]);
      }

  if (spawnStocks.Size() != spawnStockNames.Size()) {
    handle.logMessage(LOGWARN, "Error in spawner - failed to match spawning stocks");
    for (i = 0; i < stockvec.Size(); i++)
      handle.logMessage(LOGWARN, "Error in spawner - found stock", stockvec[i]->getName());
    for (i = 0; i < spawnStockNames.Size(); i++)
      handle.logMessage(LOGWARN, "Error in spawner - looking for stock", spawnStockNames[i]);
    exit(EXIT_FAILURE);
  }

  spawnRatio.Reset();
  spawnRatio = tmpratio;

  //JMB - check that the spawned stocks are defined on the areas
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
  for (i = 0; i < spawnStocks.Size(); i++)
    CI.resize(new ConversionIndex(spawnLgrpDiv, spawnStocks[i]->getLengthGroupDiv()));

  IntVector minlv(1, 0);
  IntVector sizev(1, spawnLgrpDiv->numLengthGroups());
  Storage.resize(areas.Size(), spawnAge, minlv, sizev);
  for (i = 0; i < Storage.Size(); i++)
    Storage[i].setToZero();
}

void SpawnData::Spawn(AgeBandMatrix& Alkeys, int area, const TimeClass* const TimeInfo) {

  if (onlyParent == 0)
    spawnParameters.Update(TimeInfo);

  int age, len;
  for (age = Alkeys.minAge(); age <= Alkeys.maxAge(); age++) {
    //subtract mortality and reduce the weight of the living ones.
    for (len = Alkeys.minLength(age); len < Alkeys.maxLength(age); len++) {
      PopInfo p = Alkeys[age][len] * spawnProportion[len];

      //calculate the spawning stock biomss if needed
      if (onlyParent == 0)
        spawnNumbers[age][len] = calcSpawnNumber(age, len, p.N, p.W);

      p *= exp(-spawnMortality[len]);
      p.W -= (spawnWeightLoss[len] * p.W);
      Alkeys[age][len] *= (1.0 - spawnProportion[len]);
      Alkeys[age][len] += p;
    }
  }
}

void SpawnData::addSpawnStock(int area, const TimeClass* const TimeInfo) {

  if (onlyParent == 1)
    return;

  int s, age, len;
  int inarea = this->areaNum(area);
  double sum = 0.0;
  double tmpsdev, length, N;

  for (s = 0; s < Storage.Size(); s++)
    Storage[s].setToZero();

  //create a length distribution and mean weight for the new stock
  stockParameters.Update(TimeInfo);
  if (handle.getLogLevel() >= LOGWARN) {
    if (stockParameters[0] > spawnLgrpDiv->maxLength())
      handle.logMessage(LOGWARN, "Warning in spawner - invalid mean length for spawned stock");
    if (isZero(stockParameters[0]))
      handle.logMessage(LOGWARN, "Warning in spawner - invalid standard deviation for spawned stock");
  }

  if (stockParameters[1] > verysmall) {
    tmpsdev = 1.0 / (2 * stockParameters[1] * stockParameters[1]);
    for (len = 0; len < spawnLgrpDiv->numLengthGroups(); len++) {
      length = spawnLgrpDiv->meanLength(len) - stockParameters[0];
      N = exp(-(length * length * tmpsdev));
      Storage[inarea][spawnAge][len].N = N;
      sum += N;
    }
  }

  //calculate the total number of recruits and distribute this through the length groups
  double total = calcRecruitNumber();
  if (sum > verysmall) {
    for (len = 0; len < spawnLgrpDiv->numLengthGroups(); len++) {
      length = spawnLgrpDiv->meanLength(len);
      Storage[inarea][spawnAge][len].N *= total / sum;
      Storage[inarea][spawnAge][len].W = stockParameters[2] * pow(length, stockParameters[3]);
    }
  }

  //add this to the spawned stocks
  for (s = 0; s < spawnStocks.Size(); s++) {
    if (!spawnStocks[s]->isInArea(area))
      handle.logMessage(LOGFAIL, "Error in spawner - spawned stock doesnt live on area", area);

    spawnStocks[s]->Add(Storage[inarea], CI[s], area, spawnRatio[s]);
  }

  for (age = 0; age < spawnNumbers.Nrow(); age++)
    for (len = 0; len < spawnNumbers.Ncol(age); len++)
      spawnNumbers[age][len] = 0.0;
}

int SpawnData::isSpawnStepArea(int area, const TimeClass* const TimeInfo) {
  int i, j;

  for (i = 0; i < spawnStep.Size(); i++)
    for (j = 0; j < spawnArea.Size(); j++)
      if ((spawnStep[i] == TimeInfo->getStep()) && (spawnArea[j] == area))
        return 1;
  return 0;
}

void SpawnData::Reset(const TimeClass* const TimeInfo) {
  int i;

  fnProportion->updateConstants(TimeInfo);
  if (fnProportion->didChange(TimeInfo)) {
    for (i = 0; i < LgrpDiv->numLengthGroups(); i++) {
      spawnProportion[i] = fnProportion->calculate(LgrpDiv->meanLength(i));
      if (spawnProportion[i] < 0.0) {
        handle.logMessage(LOGWARN, "Warning in spawning - function outside bounds", spawnProportion[i]);
        spawnProportion[i] = 0.0;
      }
      if (spawnProportion[i] > 1.0) {
        handle.logMessage(LOGWARN, "Warning in spawning - function outside bounds", spawnProportion[i]);
        spawnProportion[i] = 1.0;
      }
    }
  }

  fnWeightLoss->updateConstants(TimeInfo);
  if (fnWeightLoss->didChange(TimeInfo)) {
    for (i = 0; i < LgrpDiv->numLengthGroups(); i++) {
      spawnWeightLoss[i] = fnWeightLoss->calculate(LgrpDiv->meanLength(i));
      if (spawnWeightLoss[i] < 0.0) {
        handle.logMessage(LOGWARN, "Warning in spawning - function outside bounds", spawnWeightLoss[i]);
        spawnWeightLoss[i] = 0.0;
      }
      if (spawnWeightLoss[i] > 1.0) {
        handle.logMessage(LOGWARN, "Warning in spawning - function outside bounds", spawnWeightLoss[i]);
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

  if (handle.getLogLevel() >= LOGMESSAGE)
    handle.logMessage(LOGMESSAGE, "Reset spawning data");
}

double SpawnData::calcSpawnNumber(int age, int len, double number, double weight) {
  double temp = 0.0;

  switch (functionnumber) {
    case 1:
    case 2:
    case 3:
      temp = number * weight;
      break;
    case 4:
      temp = pow(LgrpDiv->meanLength(len), spawnParameters[1]) * pow(age, spawnParameters[2])
             * pow(number, spawnParameters[3]) * pow(weight, spawnParameters[4]);
      break;
    default:
      handle.logMessage(LOGWARN, "Warning in spawner - unrecognised recruitment function", functionname);
      break;
  }

  return temp;
}

double SpawnData::calcRecruitNumber() {
  int age, len;
  double recruits = 0.0, temp = 0.0;

  for (age = 0; age < spawnNumbers.Nrow(); age++)
    for (len = 0; len < spawnNumbers.Ncol(age); len++)
      temp += spawnNumbers[age][len];

  switch (functionnumber) {
    case 1:
    case 4:
      recruits = temp * spawnParameters[0];
      break;
    case 2:
      recruits = temp * spawnParameters[0] * exp(-spawnParameters[1] * temp);
      break;
    case 3:
      recruits = temp * spawnParameters[0] / (spawnParameters[1] + temp);
      break;
    default:
      handle.logMessage(LOGWARN, "Warning in spawner - unrecognised recruitment function", functionname);
      break;
  }

  return recruits;
}

void SpawnData::Print(ofstream& outfile) const {
  int i;
  outfile << "\nSpawning data\n\tNames of spawned stocks:";
  for (i = 0; i < spawnStockNames.Size(); i++)
    outfile << sep << spawnStockNames[i];
  outfile << "\n\tRatio spawning into each stock:";
  for (i = 0; i < spawnRatio.Size(); i++)
    outfile << sep << spawnRatio[i];
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
