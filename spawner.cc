#include "spawner.h"
#include "errorhandler.h"
#include "keeper.h"
#include "areatime.h"
#include "readfunc.h"
#include "readword.h"
#include "readaggregation.h"
#include "gadget.h"

extern ErrorHandler handle;

Spawner::Spawner(CommentStream& infile, int maxage, const LengthGroupDivision* const lgrpdiv,
  const AreaClass* const Area, const TimeClass* const TimeInfo, Keeper* const keeper) {

  keeper->addString("spawner");
  int i;

  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  ifstream datafile;
  CommentStream subdata(datafile);

  spawnLgrpDiv = 0;
  LgrpDiv = new LengthGroupDivision(*lgrpdiv);
  int numlength = LgrpDiv->NoLengthGroups();
  ssb.AddRows(maxage + 1, numlength, 0.0);
  spawnProportion.resize(numlength, 0.0);
  spawnMortality.resize(numlength, 0.0);
  spawnWeightLoss.resize(numlength, 0.0);

  infile >> text >> ws;
  if (strcasecmp(text, "spawnstep") != 0)
    handle.Unexpected("spawnstep", text);

  i = 0;
  while (isdigit(infile.peek()) && !infile.eof()) {
    spawnstep.resize(1);
    infile >> spawnstep[i] >> ws;
    i++;
  }

  for (i = 0; i < spawnstep.Size(); i++)
    if (spawnstep[i] < 1 || spawnstep[i] > TimeInfo->StepsInYear())
      handle.Message("Illegal spawn step in spawning function");

  infile >> text >> ws;
  if (strcasecmp(text, "spawnarea") != 0)
    handle.Unexpected("spawnarea", text);

  i = 0;
  while (isdigit(infile.peek()) && !infile.eof()) {
    spawnarea.resize(1);
    infile >> spawnarea[i] >> ws;
    i++;
  }

  for (i = 0; i < spawnarea.Size(); i++)
    if ((spawnarea[i] = Area->InnerArea(spawnarea[i])) == -1)
      handle.UndefinedArea(spawnarea[i]);

  infile >> text;
  if (strcasecmp(text, "spawnstocksandratios") == 0) {
    onlyParent = 0;
    infile >> text >> ws;
    i = 0;
    while (strcasecmp(text, "proportionfunction") != 0 && infile.good()) {
      SpawnStockNames.resize(1);
      SpawnStockNames[i] = new char[strlen(text) + 1];
      strcpy(SpawnStockNames[i], text);
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
      handle.Message("Unrecognised spawning function", text);

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
      handle.Message("Unrecognised spawning function", text);

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
      handle.Message("Unrecognised spawning function", text);

    fnWeightLoss->readConstants(infile, TimeInfo, keeper);
  } else
    handle.Unexpected("weightlossfunction", text);

  if (onlyParent == 0) {
    spawnParameters.resize(5, keeper);
    infile >> text >> ws;
    if (strcasecmp(text, "spawnparameters") == 0)
      spawnParameters.read(infile, TimeInfo, keeper);
    else
      handle.Unexpected("spawnparameters", text);

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

Spawner::~Spawner() {
  int i;
  for (i = 0; i < SpawnStockNames.Size(); i++)
    delete[] SpawnStockNames[i];
  for (i = 0; i < CI.Size(); i++)
    delete CI[i];
  delete LgrpDiv;
  delete spawnLgrpDiv;
  delete fnProportion;
  delete fnMortality;
  delete fnWeightLoss;
}

void Spawner::setStock(StockPtrVector& stockvec) {
  int index = 0;
  int i, j;
  DoubleVector tmpratio;

  if (onlyParent == 1)
    return;

  for (i = 0; i < stockvec.Size(); i++)
    for (j = 0; j < SpawnStockNames.Size(); j++)
      if (strcasecmp(stockvec[i]->Name(), SpawnStockNames[j]) == 0) {
        SpawnStocks.resize(1);
        tmpratio.resize(1);
        SpawnStocks[index] = stockvec[i];
        tmpratio[index] = Ratio[j];
        index++;
      }

  if (index != SpawnStockNames.Size()) {
    handle.logWarning("Error in spawner - failed to match spawning stocks");
    for (i = 0; i < stockvec.Size(); i++)
      handle.logWarning("Error in spawner - found stock", stockvec[i]->Name());
    for (i = 0; i < SpawnStockNames.Size(); i++)
      handle.logWarning("Error in spawner - looking for stock", SpawnStockNames[i]);
    exit(EXIT_FAILURE);
  }

  for (i = Ratio.Size(); i > 0; i--)
    Ratio.Delete(0);
  Ratio.resize(tmpratio.Size());
  for (i = 0; i < tmpratio.Size(); i++)
    Ratio[i] = tmpratio[i];

  //JMB - check that the spawned stocks are defined on the areas
  spawnage = 9999;
  double minlength = 9999.0;
  double maxlength = 0.0;
  double dl = 9999.0;
  for (i = 0; i < SpawnStocks.Size(); i++) {
    index = 0;
    for (j = 0; j < spawnarea.Size(); j++)
      if (!SpawnStocks[i]->IsInArea(spawnarea[j]))
        index++;

    if (index != 0)
      handle.logWarning("Warning in spawner - spawn stock isnt defined on all areas");

    if (SpawnStocks[i]->minAge() < spawnage)
      spawnage = SpawnStocks[i]->minAge();
    if (SpawnStocks[i]->returnLengthGroupDiv()->minLength() < minlength)
      minlength = SpawnStocks[i]->returnLengthGroupDiv()->minLength();
    if (SpawnStocks[i]->returnLengthGroupDiv()->maxLength() > maxlength)
      maxlength = SpawnStocks[i]->returnLengthGroupDiv()->maxLength();
    if (SpawnStocks[i]->returnLengthGroupDiv()->dl() < dl)
      dl = SpawnStocks[i]->returnLengthGroupDiv()->dl();
  }

  spawnLgrpDiv = new LengthGroupDivision(minlength, maxlength, dl);

  IntVector minlv(1, 0);
  IntVector sizev(1, spawnLgrpDiv->NoLengthGroups());
  Storage.resize(spawnarea.Size(), spawnage, minlv, sizev);
  for (i = 0; i < Storage.Size(); i++)
    Storage[i].setToZero();

  CI.resize(SpawnStocks.Size(), 0);
  for (i = 0; i < SpawnStocks.Size(); i++)
    CI[i] = new ConversionIndex(spawnLgrpDiv, SpawnStocks[i]->returnLengthGroupDiv());

}

void Spawner::Spawn(AgeBandMatrix& Alkeys, int area,
  const AreaClass* const Area, const TimeClass* const TimeInfo) {

  int age, len;
  if (this->IsSpawnStepArea(area, TimeInfo) == 1) {
    spawnParameters.Update(TimeInfo);
    for (age = Alkeys.minAge(); age <= Alkeys.maxAge(); age++) {
      //subtract mortality and reduce the weight of the living ones.
      for (len = Alkeys.minLength(age); len < Alkeys.maxLength(age); len++) {
        PopInfo p = Alkeys[age][len] * spawnProportion[len];

        //calculate the spawning stock biomss if needed
        if (onlyParent == 0)
          ssb[age][len] = ssbFunc(age, len, p.N, p.W);

        p *= exp(-spawnMortality[len]);
        p.W -= (spawnWeightLoss[len] * p.W);
        Alkeys[age][len] *= (1 - spawnProportion[len]);
        Alkeys[age][len] += p;
      }
    }
  }
}

//area in the call to this routine is not in the local area numbering of the stock.
void Spawner::addSpawnStock(int area, const TimeClass* const TimeInfo) {

  if (onlyParent == 1)
    return;

  if (this->IsSpawnStepArea(area, TimeInfo) == 0)
    return;

  int s, age, len;
  double TEP = 0.0;
  double sum = 0.0;
  double tmpsdev, length, N;

  for (s = 0; s < Storage.Size(); s++)
    Storage[s].setToZero();

  //calculate the number of eggs produced
  for (age = 0; age < ssb.Nrow(); age++)
    for (len = 0; len < ssb.Ncol(age); len++)
      TEP += ssb[age][len];

  //create a length distribution and mean weight for the new stock
  stockParameters.Update(TimeInfo);
  if (stockParameters[1] > verysmall) {
    tmpsdev = 1.0 / (2 * stockParameters[1] * stockParameters[1]);
    for (len = 0; len < spawnLgrpDiv->NoLengthGroups(); len++) {
      length = spawnLgrpDiv->meanLength(len) - stockParameters[0];
      N = exp(-(length * length * tmpsdev));
      Storage[area][spawnage][len].N = N;
      sum += N;
    }
  }

  if (sum > verysmall) {
    for (len = 0; len < spawnLgrpDiv->NoLengthGroups(); len++) {
      length = spawnLgrpDiv->meanLength(len);
      Storage[area][spawnage][len].N *= TEP / sum;
      Storage[area][spawnage][len].W = stockParameters[2] * pow(length, stockParameters[3]);
    }
  }

  //add this to the spawned stocks
  for (s = 0; s < SpawnStocks.Size(); s++) {
    if (!SpawnStocks[s]->IsInArea(area))
      handle.logFailure("Error in spawner - spawn stock doesnt live on area", area);

    SpawnStocks[s]->Add(Storage[area], CI[s], area, Ratio[s], SpawnStocks[s]->minAge(), SpawnStocks[s]->minAge());
  }

  for (age = 0; age < ssb.Nrow(); age++)
    for (len = 0; len < ssb.Ncol(age); len++)
      ssb[age][len] = 0.0;
}

int Spawner::IsSpawnStepArea(int area, const TimeClass* const TimeInfo) {
  int i, j;
  for (i = 0; i < spawnstep.Size(); i++)
    for (j = 0; j < spawnarea.Size(); j++)
      if ((spawnstep[i] == TimeInfo->CurrentStep()) && (spawnarea[j] == area))
        return 1;
  return 0;
}

void Spawner::Reset(const TimeClass* const TimeInfo) {
  int i;

  fnProportion->updateConstants(TimeInfo);
  if (fnProportion->constantsHaveChanged(TimeInfo)) {
    for (i = 0; i < LgrpDiv->NoLengthGroups(); i++) {
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
    for (i = 0; i < LgrpDiv->NoLengthGroups(); i++) {
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
    for (i = 0; i < LgrpDiv->NoLengthGroups(); i++) {
      spawnMortality[i] = fnMortality->calculate(LgrpDiv->meanLength(i));
    }
  }

  handle.logMessage("Reset spawning data");
}

double Spawner::ssbFunc(int age, int len, double number, double weight) {
  double temp = 0.0;
  temp = spawnParameters[0] * pow(LgrpDiv->meanLength(len), spawnParameters[1])
           * pow(age, spawnParameters[2]) * pow(number, spawnParameters[3]) * pow(weight, spawnParameters[4]);

  return temp;
}

void Spawner::Print(ofstream& outfile) const {
  int i;
  outfile << "\nSpawning data\n\tNames of spawned stocks:";
  for (i = 0; i < SpawnStocks.Size(); i++)
    outfile << sep << (const char*)(SpawnStocks[i]->Name());
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
