#include "spawner.h"
#include "errorhandler.h"
#include "keeper.h"
#include "areatime.h"
#include "readfunc.h"
#include "readword.h"
#include "readaggregation.h"
#include "gadget.h"

extern ErrorHandler handle;

SpawnData::SpawnData(CommentStream& infile, int maxage, const LengthGroupDivision* const lgrpdiv,
  const IntVector& Areas, const AreaClass* const Area, const TimeClass* const TimeInfo,
  Keeper* const keeper) : LivesOnAreas(Areas) {

  keeper->addString("spawner");
  int i;

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
    handle.Unexpected("spawnsteps", text);

  i = 0;
  while (isdigit(infile.peek()) && !infile.eof()) {
    spawnStep.resize(1);
    infile >> spawnStep[i] >> ws;
    i++;
  }

  for (i = 0; i < spawnStep.Size(); i++)
    if (spawnStep[i] < 1 || spawnStep[i] > TimeInfo->StepsInYear())
      handle.Message("Error in spawner - invalid spawning step");

  infile >> text >> ws;
  if (!((strcasecmp(text, "spawnarea") == 0) || (strcasecmp(text, "spawnareas") == 0)))
    handle.Unexpected("spawnareas", text);

  i = 0;
  while (isdigit(infile.peek()) && !infile.eof()) {
    spawnArea.resize(1);
    infile >> spawnArea[i] >> ws;
    i++;
  }

  for (i = 0; i < spawnArea.Size(); i++)
    if ((spawnArea[i] = Area->InnerArea(spawnArea[i])) == -1)
      handle.UndefinedArea(spawnArea[i]);

  infile >> text;
  if (strcasecmp(text, "spawnstocksandratios") == 0) {
    onlyParent = 0;
    infile >> text >> ws;
    i = 0;
    while (strcasecmp(text, "proportionfunction") != 0 && infile.good()) {
      spawnStockNames.resize(1);
      spawnStockNames[i] = new char[strlen(text) + 1];
      strcpy(spawnStockNames[i], text);
      Ratio.resize(1);
      infile >> Ratio[i];
      i++;
      infile >> text >> ws;
    }
  } else if (strcasecmp(text, "onlyparent") == 0) {
    onlyParent = 1;
    infile >> text >> ws;
  } else
    handle.Unexpected("spawnstocksandratios or onlyparent", text);

  if (!infile.good())
    handle.Failure();

  if (strcasecmp(text, "proportionfunction") == 0) {
    infile >> text >> ws;
    if (strcasecmp(text, "constant") == 0)
      fnProportion = new ConstSelectFunc();
    else if (strcasecmp(text, "straightline") == 0)
      fnProportion = new StraightSelectFunc();
    else if (strcasecmp(text, "exponential") == 0)
      fnProportion = new ExpSelectFunc();
    else
      handle.Message("Error in spawner - unrecognised proportion function", text);

    fnProportion->readConstants(infile, TimeInfo, keeper);
  } else
    handle.Unexpected("proportionfunction", text);

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
      handle.Message("Error in spawner - unrecognised mortality function", text);

    fnMortality->readConstants(infile, TimeInfo, keeper);
  } else
    handle.Unexpected("mortalityfunction", text);

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
      handle.Message("Error in spawner - unrecognised weight loss function", text);

    fnWeightLoss->readConstants(infile, TimeInfo, keeper);
  } else
    handle.Unexpected("weightlossfunction", text);

  if (onlyParent == 0) {
    infile >> text >> ws;
    if (!(strcasecmp(text, "recruitment") == 0))
      handle.Unexpected("recruitment", text);

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
      handle.Message("Error in spawner - unrecognised recruitment function", functionname);

    spawnParameters.read(infile, TimeInfo, keeper);

    //read in the details about the new stock
    stockParameters.resize(4, keeper);
    infile >> text >> ws;
    if (strcasecmp(text, "stockparameters") == 0)
      stockParameters.read(infile, TimeInfo, keeper);
    else
      handle.Unexpected("stockparameters", text);
  }

  infile >> ws;
  if (!infile.eof()) {
    infile >> text >> ws;
    handle.Unexpected("<end of file>", text);
  }
  handle.logMessage("Read spawning data file");
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
  int index = 0;
  int i, j;
  DoubleVector tmpratio;

  if (onlyParent == 1)
    return;

  for (i = 0; i < stockvec.Size(); i++)
    for (j = 0; j < spawnStockNames.Size(); j++)
      if (strcasecmp(stockvec[i]->Name(), spawnStockNames[j]) == 0) {
        spawnStocks.resize(1);
        tmpratio.resize(1);
        spawnStocks[index] = stockvec[i];
        tmpratio[index] = Ratio[j];
        index++;
      }

  if (index != spawnStockNames.Size()) {
    handle.logWarning("Error in spawner - failed to match spawning stocks");
    for (i = 0; i < stockvec.Size(); i++)
      handle.logWarning("Error in spawner - found stock", stockvec[i]->Name());
    for (i = 0; i < spawnStockNames.Size(); i++)
      handle.logWarning("Error in spawner - looking for stock", spawnStockNames[i]);
    exit(EXIT_FAILURE);
  }

  for (i = Ratio.Size(); i > 0; i--)
    Ratio.Delete(0);
  Ratio.resize(tmpratio.Size());
  for (i = 0; i < tmpratio.Size(); i++)
    Ratio[i] = tmpratio[i];

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
      handle.logWarning("Warning in spawner - spawned stock isnt defined on all areas");

    if (spawnStocks[i]->minAge() < spawnAge)
      spawnAge = spawnStocks[i]->minAge();
    if (spawnStocks[i]->returnLengthGroupDiv()->minLength() < minlength)
      minlength = spawnStocks[i]->returnLengthGroupDiv()->minLength();
    if (spawnStocks[i]->returnLengthGroupDiv()->maxLength() > maxlength)
      maxlength = spawnStocks[i]->returnLengthGroupDiv()->maxLength();
    if (spawnStocks[i]->returnLengthGroupDiv()->dl() < dl)
      dl = spawnStocks[i]->returnLengthGroupDiv()->dl();
  }

  spawnLgrpDiv = new LengthGroupDivision(minlength, maxlength, dl);

  IntVector minlv(1, 0);
  IntVector sizev(1, spawnLgrpDiv->numLengthGroups());
  Storage.resize(areas.Size(), spawnAge, minlv, sizev);
  for (i = 0; i < Storage.Size(); i++)
    Storage[i].setToZero();

  CI.resize(spawnStocks.Size(), 0);
  for (i = 0; i < spawnStocks.Size(); i++)
    CI[i] = new ConversionIndex(spawnLgrpDiv, spawnStocks[i]->returnLengthGroupDiv());

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
      Alkeys[age][len] *= (1 - spawnProportion[len]);
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
  if (stockParameters[0] > spawnLgrpDiv->maxLength())
    handle.logWarning("Warning in spawner - invalid mean length for spawned stock");
  if (isZero(stockParameters[0]))
    handle.logWarning("Warning in spawner - invalid standard deviation for spawned stock");

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
      handle.logFailure("Error in spawner - spawned stock doesnt live on area", area);

    spawnStocks[s]->Add(Storage[inarea], CI[s], area, Ratio[s], spawnStocks[s]->minAge(), spawnStocks[s]->minAge());
  }

  for (age = 0; age < spawnNumbers.Nrow(); age++)
    for (len = 0; len < spawnNumbers.Ncol(age); len++)
      spawnNumbers[age][len] = 0.0;
}

int SpawnData::isSpawnStepArea(int area, const TimeClass* const TimeInfo) {
  int i, j;

  for (i = 0; i < spawnStep.Size(); i++)
    for (j = 0; j < spawnArea.Size(); j++)
      if ((spawnStep[i] == TimeInfo->CurrentStep()) && (spawnArea[j] == area))
        return 1;
  return 0;
}

void SpawnData::Reset(const TimeClass* const TimeInfo) {
  int i;

  fnProportion->updateConstants(TimeInfo);
  if (fnProportion->constantsHaveChanged(TimeInfo)) {
    for (i = 0; i < LgrpDiv->numLengthGroups(); i++) {
      spawnProportion[i] = fnProportion->calculate(LgrpDiv->meanLength(i));
      if (spawnProportion[i] < 0.0) {
        handle.logWarning("Warning in spawning - function outside bounds", spawnProportion[i]);
        spawnProportion[i] = 0.0;
      }
      if (spawnProportion[i] > 1.0) {
        handle.logWarning("Warning in spawning - function outside bounds", spawnProportion[i]);
        spawnProportion[i] = 1.0;
      }
    }
  }

  fnWeightLoss->updateConstants(TimeInfo);
  if (fnWeightLoss->constantsHaveChanged(TimeInfo)) {
    for (i = 0; i < LgrpDiv->numLengthGroups(); i++) {
      spawnWeightLoss[i] = fnWeightLoss->calculate(LgrpDiv->meanLength(i));
      if (spawnWeightLoss[i] < 0.0) {
        handle.logWarning("Warning in spawning - function outside bounds", spawnWeightLoss[i]);
        spawnWeightLoss[i] = 0.0;
      }
      if (spawnWeightLoss[i] > 1.0) {
        handle.logWarning("Warning in spawning - function outside bounds", spawnWeightLoss[i]);
        spawnWeightLoss[i] = 1.0;
      }
    }
  }

  fnMortality->updateConstants(TimeInfo);
  if (fnMortality->constantsHaveChanged(TimeInfo)) {
    for (i = 0; i < LgrpDiv->numLengthGroups(); i++) {
      spawnMortality[i] = fnMortality->calculate(LgrpDiv->meanLength(i));
    }
  }

  handle.logMessage("Reset spawning data");
}

double SpawnData::calcSpawnNumber(int age, int len, double number, double weight) {
  double temp = 0.0;

  switch(functionnumber) {
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
      handle.logWarning("Warning in spawner - unrecognised recruitment function", functionname);
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

  switch(functionnumber) {
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
      handle.logWarning("Warning in spawner - unrecognised recruitment function", functionname);
      break;
  }

  return recruits;
}

void SpawnData::Print(ofstream& outfile) const {
  int i;
  outfile << "\nSpawning data\n\tNames of spawned stocks:";
  for (i = 0; i < spawnStocks.Size(); i++)
    outfile << sep << (const char*)(spawnStocks[i]->Name());
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
