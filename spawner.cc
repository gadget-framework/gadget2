#include "spawner.h"
#include "errorhandler.h"
#include "keeper.h"
#include "areatime.h"
#include "readfunc.h"
#include "readword.h"
#include "gadget.h"

Spawner::Spawner(CommentStream& infile, int minage, int maxage,
  const AreaClass* const Area, const TimeClass* const TimeInfo, Keeper* const keeper)
  : Spawningstep(maxage - minage + 1, minage,  0),
    Spawningratio(maxage - minage + 1, minage, 0),
    SpawningmortalityPattern(maxage - minage + 1, minage, 0),
    SpawningweightlossPattern(maxage - minage + 1, minage, 0) {

  ErrorHandler handle;
  keeper->AddString("spawner");
  int i, tmpint;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  infile >> text >> ws;

  intvector tmpareas;
  if (strcasecmp(text, "spawnsinareas") == 0) {
    i = 0;
    while (isdigit(infile.peek()) && !infile.eof() && (i < Area->NoAreas())) {
      tmpareas.resize(1);
      infile >> tmpint >> ws;
      if ((tmpareas[i] = Area->InnerArea(tmpint)) == -1)
        handle.UndefinedArea(tmpint);
      i++;
    }
  } else
    handle.Unexpected("spawnsinareas", text);
  this->LetLiveOnAreas(tmpareas);

  infile >> text;
  if (strcasecmp(text, "spawningstep") == 0)
    ReadIndexVector(infile, Spawningstep);
  else
    handle.Unexpected("spawningstep", text);

  infile >> text;
  if (strcasecmp(text, "spawningratio") == 0)
    ReadIndexVector(infile, Spawningratio);
  else
    handle.Unexpected("spawningratio", text);

  infile >> text;
  if (strcasecmp(text, "spawningmortality") == 0) {
    if (!(infile >> spawningMortality))
      handle.Message("Not able to read Spawningmortality");
    spawningMortality.Inform(keeper);
  } else
    handle.Unexpected("spawningmortality", text);

  infile >> text;
  if (strcasecmp(text, "spawningmortalitypattern") == 0)
    ReadIndexVector(infile, SpawningmortalityPattern);
  else
    handle.Unexpected("spawningmortalitypattern", text);

  infile >> text;
  if (strcasecmp(text, "spawningweightloss") == 0) {
    if (!(infile >> spawningWeightLoss))
      handle.Message("Not able to read Spawningweightloss from file");
    spawningWeightLoss.Inform(keeper);
  } else
    handle.Unexpected("spawningweightloss", text);

  infile >> text;
  if (strcasecmp(text, "spawningweightlosspattern") == 0)
    ReadIndexVector(infile, SpawningweightlossPattern);
  else
    handle.Unexpected("spawningweightlosspattern", text);

  if (infile.fail())
    handle.Failure("spawning");

  //And now we do some error checks
  if (spawningMortality < 0)
    handle.Message("Spawning mortality should be greater than zero");

  if (spawningWeightLoss < 0)
    handle.Message("Spawning weightloss should be greater than zero");

  for (i = Spawningstep.Mincol(); i < Spawningstep.Maxcol(); i++) {
    if (Spawningstep[i] <= 0 || Spawningstep[i] >= TimeInfo->StepsInYear())
      handle.Message("Error in spawning step - illegal timestep");

    if (Spawningratio[i] < 0 || Spawningratio[i] > 1)
      handle.Message("Error in spawning ratio - should be between 0 and 1");

    if (SpawningmortalityPattern[i] < 0 || SpawningmortalityPattern[i] * spawningMortality > 1)
      handle.Message("Error in spawning mortality - should be between 0 and 1");

    if (SpawningweightlossPattern[i] < 0 || SpawningweightlossPattern[i] * spawningWeightLoss > 1)
      handle.Message("Error in spawning weight loss - should be between 0 and 1");
  }

  keeper->ClearLast();
}

void Spawner::Spawn(Agebandmatrix& Alkeys, int area,
  const AreaClass* const Area, const TimeClass* const TimeInfo) {

  if (!this->IsInArea(area))
    return;

  int age, len;
  double ratio, m, prop, weightloss;

  //Subtract mortality and reduce the weight of the living ones.
  for (age = Alkeys.Minage(); age <= Alkeys.Maxage(); age++) {
    if (TimeInfo->CurrentStep() == Spawningstep[age]) {
      ratio = Spawningratio[age];
      m = spawningMortality * SpawningmortalityPattern[age];
      prop = 1.0;
      if (m > 0)
        prop = exp(-m);
      weightloss = spawningWeightLoss * SpawningweightlossPattern[age];
      if (weightloss < 0)
        weightloss = 0.0;
      else
        if (weightloss > 1)
          weightloss = 1.0;

      for (len = Alkeys.Minlength(age); len < Alkeys.Maxlength(age); len++) {
        popinfo p = Alkeys[age][len] * ratio * prop;
        p.W -= weightloss * p.W;
        Alkeys[age][len] *= (1 - ratio);
        Alkeys[age][len] += p;
      }
    }
  }
}
