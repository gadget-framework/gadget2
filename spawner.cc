#include "spawner.h"
#include "errorhandler.h"
#include "keeper.h"
#include "areatime.h"
#include "readfunc.h"
#include "readword.h"
#include "readaggregation.h"
#include "gadget.h"

Spawner::Spawner(CommentStream& infile, int maxage, const LengthGroupDivision* const lgrpdiv,
  const AreaClass* const Area, const TimeClass* const TimeInfo, Keeper* const keeper) {

  ErrorHandler handle;
  keeper->AddString("spawner");
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

  if (strcasecmp(text, "proportionfunction") == 0) {
    infile >> text >> ws;
    if (strcasecmp(text, "constant") == 0)
      fnProportion = new ConstSpawnFunc();
    else if (strcasecmp(text, "straightline") == 0)
      fnProportion = new StraightSpawnFunc();
    else if (strcasecmp(text, "exponential") == 0)
      fnProportion = new ExpSpawnFunc();
    else
      handle.Message("Unrecognised spawning function", text);

    fnProportion->readConstants(infile, TimeInfo, keeper);
  } else
    handle.Unexpected("proportionfunction", text);

  infile >> text;
  if (strcasecmp(text, "mortalityfunction") == 0) {
    infile >> text >> ws;
    if (strcasecmp(text, "constant") == 0)
      fnMortality = new ConstSpawnFunc();
    else if (strcasecmp(text, "straightline") == 0)
      fnMortality = new StraightSpawnFunc();
    else if (strcasecmp(text, "exponential") == 0)
      fnMortality = new ExpSpawnFunc();
    else
      handle.Message("Unrecognised spawning function", text);

    fnMortality->readConstants(infile, TimeInfo, keeper);
  } else
    handle.Unexpected("mortalityfunction", text);

  infile >> text;
  if (strcasecmp(text, "weightlossfunction") == 0) {
    infile >> text >> ws;
    if (strcasecmp(text, "constant") == 0)
      fnWeightLoss = new ConstSpawnFunc();
    else if (strcasecmp(text, "straightline") == 0)
      fnWeightLoss = new StraightSpawnFunc();
    else if (strcasecmp(text, "exponential") == 0)
      fnWeightLoss = new ExpSpawnFunc();
    else
      handle.Message("Unrecognised spawning function", text);

    fnWeightLoss->readConstants(infile, TimeInfo, keeper);
  } else
    handle.Unexpected("weightlossfunction", text);

  if (onlyParent == 0) {
    spawnParameters.resize(5, keeper);
    infile >> text;
    if (strcasecmp(text, "spawnparameters") == 0) {
      for (i = 0; i < spawnParameters.Size(); i++) {
        if (!(infile >> spawnParameters[i]))
          handle.Message("Wrong format for spawning parameters");
        spawnParameters[i].Inform(keeper);
      }
    } else
      handle.Unexpected("spawnparameters", text);
    readWordAndFormula(infile, "meanlength", meanlength);
    meanlength.Inform(keeper);
    readWordAndFormula(infile, "sdev", sdev);
    sdev.Inform(keeper);
    readWordAndFormula(infile, "alpha", alpha);
    alpha.Inform(keeper);
    readWordAndFormula(infile, "beta", beta);
    beta.Inform(keeper);
  }

  infile >> ws;
  if (!infile.eof()) {
    infile >> text >> ws;
    handle.Unexpected("<end of file>", text);
  }
  keeper->ClearLast();
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

void Spawner::SetStock(StockPtrVector& stockvec) {
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
    cerr << "Error - did not find all the stock(s) matching:\n";
    for (i = 0; i < SpawnStockNames.Size(); i++)
      cerr << (const char*)SpawnStockNames[i] << sep;
    cerr << "\nwhen searching for spawning stock(s) - found only:\n";
    for (i = 0; i < stockvec.Size(); i++)
      cerr << stockvec[i]->Name() << sep;
    cerr << endl;
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
      cerr << "Warning - spawning requested to stock " << (const char*)SpawnStocks[i]->Name()
        << "\nwhich might not be defined on " << index << " areas\n";

    if (SpawnStocks[i]->Minage() < spawnage)
      spawnage = SpawnStocks[i]->Minage();
    if (SpawnStocks[i]->ReturnLengthGroupDiv()->minLength() < minlength)
      minlength = SpawnStocks[i]->ReturnLengthGroupDiv()->minLength();
    if (SpawnStocks[i]->ReturnLengthGroupDiv()->maxLength() > maxlength)
      maxlength = SpawnStocks[i]->ReturnLengthGroupDiv()->maxLength();
    if (SpawnStocks[i]->ReturnLengthGroupDiv()->dl() < dl)
      dl = SpawnStocks[i]->ReturnLengthGroupDiv()->dl();
  }

  spawnLgrpDiv = new LengthGroupDivision(minlength, maxlength, dl);

  IntVector minlv(1, 0);
  IntVector sizev(1, spawnLgrpDiv->NoLengthGroups());
  Storage.resize(spawnarea.Size(), spawnage, minlv, sizev);

  CI.resize(SpawnStocks.Size(), 0);
  for (i = 0; i < SpawnStocks.Size(); i++)
    CI[i] = new ConversionIndex(spawnLgrpDiv, SpawnStocks[i]->ReturnLengthGroupDiv());

}

void Spawner::Spawn(AgeBandMatrix& Alkeys, int area,
  const AreaClass* const Area, const TimeClass* const TimeInfo) {

  int age, len;
  if (this->IsSpawnStepArea(area, TimeInfo) == 1) {
    for (age = Alkeys.Minage(); age <= Alkeys.Maxage(); age++) {
      //subtract mortality and reduce the weight of the living ones.
      for (len = Alkeys.Minlength(age); len < Alkeys.Maxlength(age); len++) {
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
  if (sdev > verysmall) {
    tmpsdev = 1.0 / (2 * sdev * sdev);
    for (len = 0; len < spawnLgrpDiv->NoLengthGroups(); len++) {
      length = spawnLgrpDiv->Meanlength(len) - meanlength;
      N = exp(-(length * length * tmpsdev));
      Storage[area][spawnage][len].N = N;
      sum += N;
    }
  }

  if (sum > verysmall) {
    for (len = 0; len < spawnLgrpDiv->NoLengthGroups(); len++) {
      length = spawnLgrpDiv->Meanlength(len);
      Storage[area][spawnage][len].N *= TEP / sum;
      Storage[area][spawnage][len].W = alpha * pow(length, beta);
    }
  }

  //add this to the spawned stocks
  for (s = 0; s < SpawnStocks.Size(); s++) {
    if (!SpawnStocks[s]->IsInArea(area)) {
      cerr << "Error - spawning stock " << (const char*)(SpawnStocks[s]->Name())
        << " cannot happen on area " << area << " since it doesnt live there!\n";
      exit(EXIT_FAILURE);
    }
    SpawnStocks[s]->Add(Storage[area], CI[s], area, Ratio[s], SpawnStocks[s]->Minage(), SpawnStocks[s]->Minage());
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

void Spawner::Precalc(const TimeClass* const TimeInfo) {
  int i, j;
  double len;

  fnProportion->updateConstants(TimeInfo);
  fnWeightLoss->updateConstants(TimeInfo);
  fnMortality->updateConstants(TimeInfo);
  for (i = 0; i < LgrpDiv->NoLengthGroups(); i++) {
    len = LgrpDiv->Meanlength(i);
    spawnProportion[i] = fnProportion->calculate(len);
    if (spawnProportion[i] < 0.0) {
      cerr << "Warning in spawning - function outside bounds " << spawnProportion[i] << endl;
      spawnProportion[i] = 0.0;
    }
    if (spawnProportion[i] > 1.0) {
      cerr << "Warning in spawning - function outside bounds " << spawnProportion[i] << endl;
      spawnProportion[i] = 1.0;
    }
    spawnWeightLoss[i] = fnWeightLoss->calculate(len);
    if (spawnWeightLoss[i] < 0.0) {
      cerr << "Warning in spawning - function outside bounds " << spawnWeightLoss[i] << endl;
      spawnWeightLoss[i] = 0.0;
    }
    if (spawnWeightLoss[i] > 1.0) {
      cerr << "Warning in spawning - function outside bounds " << spawnWeightLoss[i] << endl;
      spawnWeightLoss[i] = 1.0;
    }
    spawnMortality[i] = fnMortality->calculate(len);
  }

  if (onlyParent == 1)
    return;

  for (i = 0; i < Storage.Size(); i++)
    Storage[i].setToZero();
  for (i = 0; i < ssb.Nrow(); i++)
    for (j = 0; j < ssb.Ncol(i); j++)
      ssb[i][j] = 0.0;
}

double Spawner::ssbFunc(int age, int len, double number, double weight) {
  double temp = 0.0;
  temp = spawnParameters[0] * pow(LgrpDiv->Meanlength(len), spawnParameters[1])
           * pow(age, spawnParameters[2]) * pow(number, spawnParameters[3]) * pow(weight, spawnParameters[4]);

  return temp;
}

void Spawner::Print(ofstream& outfile) const {
  outfile << "\tSpawning information:\n";
}
