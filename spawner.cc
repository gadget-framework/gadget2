#include "spawner.h"
#include "errorhandler.h"
#include "keeper.h"
#include "areatime.h"
#include "readfunc.h"
#include "readword.h"
#include "readaggregation.h"
#include "gadget.h"

Spawner::Spawner(CommentStream& infile, int maxstockage, int numlength,
  const AreaClass* const Area, const TimeClass* const TimeInfo, Keeper* const keeper) {

  ErrorHandler handle;
  keeper->AddString("spawner");
  int i, j;
  int numarea = 0, numage = 0;

  char datafilename[MaxStrLength];
  strncpy(datafilename, "", MaxStrLength);
  ifstream datafile;
  CommentStream subdata(datafile);

  ssb.AddRows(maxstockage, numlength, 0.0);

  //Read in area aggregation from file
  readWordAndValue(infile, "areaaggfile", datafilename);
  datafile.open(datafilename, ios::in);
  checkIfFailure(datafile, datafilename);
  handle.Open(datafilename);
  numarea = readAggregation(subdata, spawnareas, areaindex);
  handle.Close();
  datafile.close();
  datafile.clear();

  //Read in age aggregation from file
  readWordAndValue(infile, "ageaggfile", datafilename);
  datafile.open(datafilename, ios::in);
  checkIfFailure(datafile, datafilename);
  handle.Open(datafilename);
  numage = readAggregation(subdata, spawnages, ageindex);
  handle.Close();
  datafile.close();
  datafile.clear();

  //Must change from outer areas to inner areas.
  for (i = 0; i < spawnareas.Nrow(); i++)
    for (j = 0; j < spawnareas.Ncol(i); j++)
      if ((spawnareas[i][j] = Area->InnerArea(spawnareas[i][j])) == -1)
        handle.UndefinedArea(spawnareas[i][j]);

  //Finally read in spawning data from file
  readWordAndValue(infile, "spawnfile", datafilename);
  datafile.open(datafilename, ios::in);
  checkIfFailure(datafile, datafilename);
  handle.Open(datafilename);
  ReadSpawnerData(subdata, TimeInfo, numarea, numage);
  handle.Close();
  datafile.close();
  datafile.clear();
  keeper->ClearLast();
}

void Spawner::ReadSpawnerData(CommentStream& infile,
  const TimeClass* TimeInfo, int numarea, int numage) {

  int i;
  int year, step;
  char tmparea[MaxStrLength], tmpage[MaxStrLength];
  strncpy(tmparea, "", MaxStrLength);
  strncpy(tmpage, "", MaxStrLength);
  int keepdata, timeid, ageid, areaid;
  ErrorHandler handle;

  Formula tmpF;  //initialised to zero
  char junknumber[MaxStrLength];
  strncpy(junknumber, "", MaxStrLength);

  //Find start of spawning data in datafile
  infile >> ws;
  char c;
  c = infile.peek();
  if (!isdigit(c)) {
    infile.get(c);
    while (c != '\n' && !infile.eof())
      infile.get(c);
  }

  //Check the number of columns in the inputfile
  if (countColumns(infile) != 7)
    handle.Message("Wrong number of columns in inputfile - should be 7");

  //Found the start of the spawning data in the following format
  //year - step - age - area - ratio - mortality - weight loss

  while (!infile.eof()) {
    keepdata = 0;
    infile >> year >> step >> tmparea >> tmpage >> ws;

    //check if the year and step are in the simulation
    timeid = -1;
    if (TimeInfo->IsWithinPeriod(year, step)) {
      //if this is a new timestep, resize to store the data
      for (i = 0; i < Years.Size(); i++)
        if ((Years[i] == year) && (Steps[i] == step))
          timeid = i;

      if (timeid == -1) {
        Years.resize(1, year);
        Steps.resize(1, step);
        timeid = (Years.Size() - 1);

        //Resize the matrices to hold the data
        spawnRatio.resize(1, new FormulaMatrix(numarea, numage, tmpF));
        spawnMortality.resize(1, new FormulaMatrix(numarea, numage, tmpF));
        spawnWeightLoss.resize(1, new FormulaMatrix(numarea, numage, tmpF));
      }

    } else {
      //dont keep the data
      keepdata = 1;
    }

    //if tmparea is in areaindex find areaid, else dont keep the data
    areaid = -1;
    for (i = 0; i < areaindex.Size(); i++)
      if (strcasecmp(areaindex[i], tmparea) == 0)
        areaid = i;

    if (areaid == -1)
      keepdata = 1;

    //if tmpage is in ageindex find ageid, else dont keep the data
    ageid = -1;
    for (i = 0; i < ageindex.Size(); i++)
      if (strcasecmp(ageindex[i], tmpage) == 0)
        ageid = i;

    if (ageid == -1)
      keepdata = 1;

    if (keepdata == 0) {
      //spawning data is required, so store it
      infile >> (*spawnRatio[timeid])[areaid][ageid] >> ws;
      infile >> (*spawnMortality[timeid])[areaid][ageid] >> ws;
      infile >> (*spawnWeightLoss[timeid])[areaid][ageid] >> ws;
    } else {
      //spawning data is not required, so read but ignore it
      infile >> junknumber >> ws;
      infile >> junknumber >> ws;
      infile >> junknumber >> ws;
    }
  }
  AAT.AddActions(Years, Steps, TimeInfo);
}

Spawner::~Spawner() {
  int i;
  for (i = 0; i < areaindex.Size(); i++)
    delete[] areaindex[i];
  for (i = 0; i < ageindex.Size(); i++)
    delete[] ageindex[i];
  for (i = 0; i < spawnRatio.Size(); i++) {
    delete spawnRatio[i];
    delete spawnMortality[i];
    delete spawnWeightLoss[i];
  }
}

void Spawner::Spawn(AgeBandMatrix& Alkeys, int area,
  const AreaClass* const Area, const TimeClass* const TimeInfo) {

  int i, j, age, len;
  int areaid = -1;
  int timeid = -1;
  int ageid = -1;
  double ratio, mort, prop, weight;

  //check to find out if spawning takes place on current timestep
  if (!(AAT.AtCurrentTime(TimeInfo)))
    return;

  for (i = 0; i < Years.Size(); i++)
    if ((Years[i] == TimeInfo->CurrentYear()) && (Steps[i] == TimeInfo->CurrentStep()))
      timeid = i;

  if (timeid == -1)
    return;

  //spawning takes place one area at a time - find areaid
  for (i = 0; i < spawnareas.Nrow(); i++)
    for (j = 0; j < spawnareas.Ncol(i); j++)
      if (spawnareas[i][j] == area)
        areaid = i;

  if (areaid == -1)
    return;

  for (age = Alkeys.Minage(); age <= Alkeys.Maxage(); age++) {
    ageid = -1;
    for (i = 0; i < spawnages.Nrow(); i++)
      for (j = 0; j < spawnages.Ncol(i); j++)
        if (spawnages[i][j] == age)
          ageid = i;

    if (ageid == -1) {
      cerr << "Error in spawner - failed to calculate ageid for age " << age << endl;
      break;
    }

    ratio = (*spawnRatio[timeid])[areaid][ageid];
    mort = (*spawnMortality[timeid])[areaid][ageid];
    weight = (*spawnWeightLoss[timeid])[areaid][ageid];
    prop = 1.0;

    //some error checking
    if (ratio < 0) {
      cerr << "Warning - spawning ratio for age " << age << " is " << ratio << " which is less than 0\n";
      ratio = 0.0;
    }
    if (ratio > 1) {
      cerr << "Warning - spawning ratio for age " << age << " is " << ratio << " which is greater than 1\n";
      ratio = 1.0;
    }
    if (weight < 0) {
      cerr << "Warning - spawning weight loss for age " << age << " is " << weight << " which is less than 0\n";
      weight = 0.0;
    }
    if (weight > 1) {
      cerr << "Warning - spawning weight loss for age " << age << " is " << weight << " which is greater than 1\n";
      weight = 1.0;
    }

    if (mort >= 0)
      prop = exp(-mort);
    else
      cerr << "Warning - spawning mortality for age " << age << " is " << mort << " which is less than 0\n";

    //Subtract mortality and reduce the weight of the living ones.
    for (len = Alkeys.Minlength(age); len < Alkeys.Maxlength(age); len++) {
      PopInfo p = Alkeys[age][len] * ratio;
      ssb[ageid][len] = p.N * (p.W * weight);
      //cout << "for age " << age << " and length " << len << " ssb is " << ssb[ageid][len] << endl;

      p *= prop;
      p.W -= weight * p.W;
      Alkeys[age][len] *= (1 - ratio);
      Alkeys[age][len] += p;
    }
  }

  DoubleVector tmpSpawn;
  tmpSpawn.resize(ssb.Ncol(), 0.0);
  for (age = 0; age < ssb.Nrow(); age++)
    for (len = 0; len < ssb.Ncol(age); len++)
      tmpSpawn[len] += ssb[age][len];

  //for (len = 0; len < tmpSpawn.Size(); len++)
    //cout << "for length " << len << " ssb is " << tmpSpawn[len] << endl;

}
