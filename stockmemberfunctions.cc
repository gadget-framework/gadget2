#include "stock.h"
#include "keeper.h"
#include "areatime.h"
#include "naturalm.h"
#include "grower.h"
#include "stockprey.h"
#include "stockpredator.h"
#include "initialcond.h"
#include "migration.h"
#include "readfunc.h"
#include "mathfunc.h"
#include "maturity.h"
#include "renewal.h"
#include "transition.h"
#include "spawner.h"
#include "errorhandler.h"
#include "gadget.h"

extern ErrorHandler handle;

/* In this file most of the member functions for the class stock are
 * defined. Most of them are rather simple, call member functions of
 * AgeLengthKeys etc. Migration function in stock.
 * Migration->Getmatrix returns a pointer to migrationlist. */

void Stock::Migrate(const TimeClass* const TimeInfo) {
  //age dependent migrations should also be here.
  if (doesmigrate) {
    Alkeys.Migrate(migration->Migrationmatrix(TimeInfo));
    if (tagAlkeys.numTagExperiments() > 0)
      tagAlkeys.Migrate(migration->Migrationmatrix(TimeInfo), Alkeys);
  }
}

//-------------------------------------------------------------------
//Sum up into Growth+Predator and Prey Lengthgroups.  Storage is a
//vector of PopInfo that stores the sum over each lengthgroup.
void Stock::CalcNumbers(int area, const AreaClass* const Area, const TimeClass* const TimeInfo) {
  int inarea = AreaNr[area];
  PopInfo nullpop;
  int i;
  for (i = 0; i < NumberInArea[inarea].Size(); i++)
    NumberInArea[inarea][i] = nullpop;
  Alkeys[inarea].Colsum(NumberInArea[inarea]);
  if (doesgrow)
    grower->Sum(NumberInArea[inarea], area);
  if (iseaten) {
    prey->Sum(Alkeys[inarea], area, TimeInfo->CurrentSubstep());
    for (i = 0; i < allTags.Size(); i++)
      allTags[i]->StoreNumberPriorToEating(area, this->Name());
  }
  if (doeseat)
    ((StockPredator*)predator)->Sum(Alkeys[inarea], area);
}

//-------------------------------------------------------------------
void Stock::calcEat(int area, const AreaClass* const Area, const TimeClass* const TimeInfo) {
  if (doeseat)
    predator->Eat(area, TimeInfo->LengthOfCurrent(), Area->Temperature(area, TimeInfo->CurrentTime()),
      Area->Size(area), TimeInfo->CurrentSubstep(), TimeInfo->NrOfSubsteps());
}

void Stock::checkEat(int area, const AreaClass* const Area, const TimeClass* const TimeInfo) {
  if (iseaten)
    prey->checkConsumption(area, TimeInfo->NrOfSubsteps());
}

void Stock::adjustEat(int area, const AreaClass* const Area, const TimeClass* const TimeInfo) {
  if (doeseat)
    predator->adjustConsumption(area, TimeInfo->NrOfSubsteps(), TimeInfo->CurrentSubstep());
}

//-------------------------------------------------------------------
void Stock::ReducePop(int area, const AreaClass* const Area, const TimeClass* const TimeInfo) {

  int inarea = AreaNr[area];

  //Predation
  if (iseaten)
    prey->Subtract(Alkeys[inarea], area);

  //Natural Mortality changed with more substeps
  DoubleVector* PropSurviving;
  PropSurviving = new DoubleVector(NatM->ProportionSurviving(TimeInfo));
  double timeratio = 1.0 / TimeInfo->NrOfSubsteps();

  int i;
  for (i = 0; i < PropSurviving->Size(); i++)
    (*PropSurviving)[i] = pow((*PropSurviving)[i], timeratio);

  Alkeys[inarea].Multiply(*PropSurviving);
  delete PropSurviving;

  if (tagAlkeys.numTagExperiments() > 0)
    tagAlkeys[inarea].updateAndTagLoss(Alkeys[inarea], tagAlkeys.tagloss());
}

//-----------------------------------------------------------------------
//Function that updates the length distributions and makes part of the stock Mature.
void Stock::Grow(int area, const AreaClass* const Area, const TimeClass* const TimeInfo) {

  if (!doesgrow && doesmature)
    handle.logFailure("Error in stock - maturation without growth is not implemented");

  if (!doesgrow)
    return;
  if (doeseat)
    grower->GrowthCalc(area, Area, TimeInfo, ((StockPredator*)predator)->FPhi(area),
      ((StockPredator*)predator)->MaxConByLength(area));
  else
    grower->GrowthCalc(area, Area, TimeInfo);

  int inarea = AreaNr[area];
  grower->GrowthImplement(area, NumberInArea[inarea], LgrpDiv);

  if (doesmature) {
    if (maturity->IsMaturationStep(area, TimeInfo)) {
      Alkeys[inarea].Grow(grower->LengthIncrease(area), grower->WeightIncrease(area), maturity, TimeInfo, Area, area);
      if (tagAlkeys.numTagExperiments() > 0)
        tagAlkeys[inarea].Grow(grower->LengthIncrease(area), Alkeys[inarea], maturity, TimeInfo, Area, area);
    } else {
      Alkeys[inarea].Grow(grower->LengthIncrease(area), grower->WeightIncrease(area));
      if (tagAlkeys.numTagExperiments() > 0)
        tagAlkeys[inarea].Grow(grower->LengthIncrease(area), Alkeys[inarea]);
    }
  } else {
    Alkeys[inarea].Grow(grower->LengthIncrease(area), grower->WeightIncrease(area));
    if (tagAlkeys.numTagExperiments() > 0)
      tagAlkeys[inarea].Grow(grower->LengthIncrease(area), Alkeys[inarea]);
  }
}

//-----------------------------------------------------------------------
//A number of Special functions, Spawning, Renewal, Maturation and
//Transition to other Stocks, Maturity due to age and increased age.
void Stock::FirstUpdate(int area, const AreaClass* const Area, const TimeClass* const TimeInfo) {
  if (doesmove)
    if (transition->IsTransitionStep(area, TimeInfo))
      transition->keepAgeGroup(area, Alkeys[AreaNr[area]], tagAlkeys[AreaNr[area]], TimeInfo);
}

void Stock::SecondUpdate(int area, const AreaClass* const Area, const TimeClass* const TimeInfo) {
  if (this->Birthday(TimeInfo)) {
    int inarea = AreaNr[area];
    Alkeys[inarea].IncrementAge();
    if (tagAlkeys.numTagExperiments() > 0)
      tagAlkeys[inarea].IncrementAge(Alkeys[inarea]);
  }
}

void Stock::ThirdUpdate(int area, const AreaClass* const Area, const TimeClass* const TimeInfo) {
  if (doesmove) {
    if (transition->IsTransitionStep(area, TimeInfo)) {
      updateTransitionStockWithTags(TimeInfo);
      transition->Move(area, TimeInfo);
    }
  }
  if (doesspawn) {
    spawner->addSpawnStock(area, TimeInfo);
  }
}

void Stock::FirstSpecialTransactions(int area, const AreaClass* const Area, const TimeClass* const TimeInfo) {
  if (doesspawn) {
    int inarea = AreaNr[area];
    spawner->Spawn(Alkeys[inarea], area, Area, TimeInfo);
    if (tagAlkeys.numTagExperiments() > 0)
      tagAlkeys[inarea].updateNumbers(Alkeys[inarea]);
  }
}

void Stock::SecondSpecialTransactions(int area, const AreaClass* const Area, const TimeClass* const TimeInfo) {
  if (doesmature)
    if (maturity->IsMaturationStep(area, TimeInfo)) {
      updateMatureStockWithTags(TimeInfo);
      maturity->Move(area, TimeInfo);
    }
  if (doesrenew)
    this->Renewal(area, TimeInfo);
}

void Stock::updateMatureStockWithTags(const TimeClass* const TimeInfo) {
  int i;
  for (i = 0; i < matureTags.Size(); i++)
    matureTags[i]->updateMatureStock(TimeInfo);
  matureTags.DeleteAll();
}

void Stock::updateTransitionStockWithTags(const TimeClass* const TimeInfo) {
  int i;
  for (i = 0; i < transitionTags.Size(); i++)
    transitionTags[i]->updateTransitionStock(TimeInfo);
  transitionTags.DeleteAll();
}

void Stock::Renewal(int area, const TimeClass* const TimeInfo) {
  if (doesrenew) {
    int inarea = AreaNr[area];
    renewal->addRenewal(Alkeys[inarea], area, TimeInfo);
    if (tagAlkeys.numTagExperiments() > 0)
      tagAlkeys[inarea].updateRatio(Alkeys[inarea]);
  }
}

//Add to a stock.  A function frequently accessed from other stocks.
//Used by Transition and Maturity and Renewal.
void Stock::Add(const AgeBandMatrix& Addition, const ConversionIndex* const CI,
  int area, double ratio, int MinAge, int MaxAge) {

  Alkeys[AreaNr[area]].Add(Addition, *CI, ratio, MinAge, MaxAge);
}

void Stock::Add(const AgeBandMatrixRatioPtrVector& Addition, int AddArea, const ConversionIndex* const CI,
  int area, double ratio, int MinAge, int MaxAge) {

  if ((Addition.numTagExperiments() > 0) && (Addition.numTagExperiments() <= tagAlkeys.numTagExperiments())) {
    int inarea = AreaNr[area];
    AgebandmratioAdd(tagAlkeys, inarea, Addition, AddArea, *CI, ratio, MinAge, MaxAge);
    tagAlkeys[inarea].updateRatio(Alkeys[inarea]);
  }
}

void Stock::RecalcMigration(const TimeClass* const TimeInfo) {
  if (doesmigrate)
    migration->MigrationRecalc(TimeInfo->CurrentYear());
}

void Stock::updateTags(AgeBandMatrixPtrVector* tagbyagelength, Tags* newtag, double tagloss) {

  tagAlkeys.addTag(tagbyagelength, Alkeys, newtag->Name(), tagloss);
  allTags.resize(1, newtag);
  if (doesmature) {
    maturity->addMaturityTag(newtag->Name());
    matureTags.resize(1, newtag);
  }
  if (doesmove) {
    transition->addTransitionTag(newtag->Name());
    transitionTags.resize(1, newtag);
  }
}

void Stock::DeleteTags(const char* tagname) {
  allTags.Delete(tagAlkeys.getID(tagname));
  tagAlkeys.deleteTag(tagname);
  if (doesmature)
    maturity->deleteMaturityTag(tagname);
  if (doesmove)
    transition->deleteTransitionTag(tagname);
}

const CharPtrVector Stock::TaggingExperimentIDs() {
  return tagAlkeys.tagIDs();
}
