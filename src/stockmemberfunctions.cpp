#include "stock.h"
#include "keeper.h"
#include "areatime.h"
#include "naturalm.h"
#include "grower.h"
#include "stockprey.h"
#include "stockpredator.h"
#include "migration.h"
#include "maturity.h"
#include "renewal.h"
#include "transition.h"
#include "spawner.h"
#include "stray.h"
#include "errorhandler.h"
#include "gadget.h"
#include "global.h"

void Stock::Migrate(const TimeClass* const TimeInfo) {
  if (doesmigrate && migration->isMigrationStep(TimeInfo)) {
    Alkeys.Migrate(migration->getMigrationMatrix(TimeInfo), tmpMigrate);
    if (istagged && tagAlkeys.numTagExperiments() > 0)
      tagAlkeys.Migrate(migration->getMigrationMatrix(TimeInfo), Alkeys);
  }
}

//-------------------------------------------------------------------
//Sum up into Growth+Predator and Prey Lengthgroups.  Storage is a
//vector of PopInfo that stores the sum over each lengthgroup.
void Stock::calcNumbers(int area, const TimeClass* const TimeInfo) {
  int inarea = this->areaNum(area);
  Alkeys[inarea].sumColumns(tmpPopulation[inarea]);
  if (doesgrow)
    grower->Sum(tmpPopulation[inarea], area);
  if (iseaten) {
    prey->Sum(Alkeys[inarea], area);
    if (istagged) {
      int i;
      for (i = 0; i < allTags.Size(); i++)
        allTags[i]->storeConsumptionALK(area, this->getName());
    }
  }
  if (doeseat)
    ((StockPredator*)predator)->Sum(Alkeys[inarea], area);
}

//-------------------------------------------------------------------
void Stock::calcEat(int area, const AreaClass* const Area, const TimeClass* const TimeInfo) {
  if (doeseat)
    predator->Eat(area, Area, TimeInfo);
}

void Stock::checkEat(int area, const TimeClass* const TimeInfo) {
  if (iseaten)
    prey->checkConsumption(area, TimeInfo);
}

void Stock::adjustEat(int area, const TimeClass* const TimeInfo) {
  if (doeseat)
    predator->adjustConsumption(area, TimeInfo);
}

//-------------------------------------------------------------------
void Stock::reducePop(int area, const TimeClass* const TimeInfo) {
  int inarea = this->areaNum(area);

  //Predation
  if (iseaten)
    prey->Subtract(Alkeys[inarea], area);

  //Natural mortality
  if (TimeInfo->numSubSteps() == 1) {
    Alkeys[inarea].Multiply(naturalm->getProportion(area));
  } else {
    //changed to include the possibility of substeps
    DoubleVector* PropSurviving;
    PropSurviving = new DoubleVector(naturalm->getProportion(area));
    double timeratio = 1.0 / TimeInfo->numSubSteps();

    int i;
    for (i = 0; i < PropSurviving->Size(); i++)
      (*PropSurviving)[i] = pow((*PropSurviving)[i], timeratio);

    Alkeys[inarea].Multiply(*PropSurviving);
    delete PropSurviving;
  }

  if (istagged && tagAlkeys.numTagExperiments() > 0)
    tagAlkeys[inarea].updateAndTagLoss(Alkeys[inarea], tagAlkeys.getTagLoss());
}

//-----------------------------------------------------------------------
//Function that updates the length distributions and makes part of the stock Mature.
void Stock::Grow(int area, const AreaClass* const Area, const TimeClass* const TimeInfo) {

  if (!doesgrow)
    return;

  if (doeseat)
    grower->calcGrowth(area, Area, TimeInfo, ((StockPredator*)predator)->getFPhi(area),
      ((StockPredator*)predator)->getMaxConsumption(area));
  else
    grower->calcGrowth(area, Area, TimeInfo);

  int inarea = this->areaNum(area);
  if (grower->getFixedWeights()) {
    //Weights at length are fixed to the value in the input file
    grower->implementGrowth(area, LgrpDiv);
    if (doesmature && maturity->isMaturationStep(TimeInfo))
      Alkeys[inarea].Grow(grower->getLengthIncrease(area), grower->getWeight(area), maturity, area);
    else
      Alkeys[inarea].Grow(grower->getLengthIncrease(area), grower->getWeight(area));

  } else {
    //New weights at length are calculated
    grower->implementGrowth(area, tmpPopulation[inarea], LgrpDiv);
    if (doesmature && maturity->isMaturationStep(TimeInfo))
      Alkeys[inarea].Grow(grower->getLengthIncrease(area), grower->getWeightIncrease(area), maturity, area);
    else
      Alkeys[inarea].Grow(grower->getLengthIncrease(area), grower->getWeightIncrease(area));
  }

  if (istagged && tagAlkeys.numTagExperiments() > 0) {
    if (doesmature && maturity->isMaturationStep(TimeInfo))
      tagAlkeys[inarea].Grow(grower->getLengthIncrease(area), Alkeys[inarea], maturity, area);
    else
      tagAlkeys[inarea].Grow(grower->getLengthIncrease(area), Alkeys[inarea]);
  }
}

//-----------------------------------------------------------------------
//A number of Special functions, Spawning, Renewal, Maturation and
//Transition to other Stocks, Maturity due to age and increased age.
void Stock::updateAgePart1(int area, const TimeClass* const TimeInfo) {
  if (doesmove && transition->isTransitionStep(TimeInfo)) {
    if (istagged && tagAlkeys.numTagExperiments() > 0)
      transition->storeTransitionStock(area, Alkeys[this->areaNum(area)], tagAlkeys[this->areaNum(area)], TimeInfo);
    else
      transition->storeTransitionStock(area, Alkeys[this->areaNum(area)], TimeInfo);
  }
}

void Stock::updateAgePart2(int area, const TimeClass* const TimeInfo) {
  if (this->isBirthday(TimeInfo)) {
    Alkeys[this->areaNum(area)].IncrementAge();
    if (istagged && tagAlkeys.numTagExperiments() > 0)
      tagAlkeys[this->areaNum(area)].IncrementAge(Alkeys[this->areaNum(area)]);
  }
}

void Stock::updateAgePart3(int area, const TimeClass* const TimeInfo) {
  if (doesmove && transition->isTransitionStep(TimeInfo)) {
    if (istagged && transitionTags.Size() > 0) {
      int i;
      for (i = 0; i < transitionTags.Size(); i++)
        transitionTags[i]->updateTransitionStock(TimeInfo);
      transitionTags.deleteAll();
    }
    transition->Move(area, TimeInfo);
  }

  //JMB - only update the ratio for the tagged stock after all other age updates
  if (istagged && tagAlkeys.numTagExperiments() > 0)
    tagAlkeys[this->areaNum(area)].updateRatio(Alkeys[this->areaNum(area)]);
}

void Stock::updatePopulationPart1(int area, const TimeClass* const TimeInfo) {
  if (doesspawn && spawner->isSpawnStepArea(area, TimeInfo)) {
    spawner->Spawn(Alkeys[this->areaNum(area)], area, TimeInfo);
    if (istagged && tagAlkeys.numTagExperiments() > 0)
      tagAlkeys[this->areaNum(area)].updateNumbers(Alkeys[this->areaNum(area)]);
  }
}

void Stock::updatePopulationPart2(int area, const TimeClass* const TimeInfo) {
  if (doesmature && maturity->isMaturationStep(TimeInfo)) {
    if (istagged && matureTags.Size() > 0) {
      int i;
      for (i = 0; i < matureTags.Size(); i++)
        matureTags[i]->updateMatureStock(TimeInfo);
      matureTags.deleteAll();
    }
    maturity->Move(area, TimeInfo);
  }
}

void Stock::updatePopulationPart3(int area, const TimeClass* const TimeInfo) {
  if (doesrenew && renewal->isRenewalStepArea(area, TimeInfo))
    renewal->addRenewal(Alkeys[this->areaNum(area)], area, TimeInfo);

  if (doesspawn && spawner->isSpawnStepArea(area, TimeInfo))
    spawner->addSpawnStock(area, TimeInfo);
}

void Stock::updatePopulationPart4(int area, const TimeClass* const TimeInfo) {
  if (doesstray && stray->isStrayStepArea(area, TimeInfo)) {
    if (istagged && tagAlkeys.numTagExperiments() > 0)
      stray->storeStrayingStock(area, Alkeys[this->areaNum(area)], tagAlkeys[this->areaNum(area)], TimeInfo);
    else
      stray->storeStrayingStock(area, Alkeys[this->areaNum(area)], TimeInfo);
  }
}

void Stock::updatePopulationPart5(int area, const TimeClass* const TimeInfo) {
  if (doesstray && stray->isStrayStepArea(area, TimeInfo)) {
    if (istagged && strayTags.Size() > 0) {
      int i;
      for (i = 0; i < strayTags.Size(); i++)
        strayTags[i]->updateStrayStock(TimeInfo);
      strayTags.deleteAll();
    }
    stray->addStrayStock(area, TimeInfo);
  }

  //JMB - only update the ratio for the tagged stock after all other population updates
  if (istagged && tagAlkeys.numTagExperiments() > 0)
    tagAlkeys[this->areaNum(area)].updateRatio(Alkeys[this->areaNum(area)]);
}

void Stock::Add(const AgeBandMatrix& Addition,
  const ConversionIndex* const CI, int area, double ratio) {

  Alkeys[this->areaNum(area)].Add(Addition, *CI, ratio);
}

void Stock::Add(const AgeBandMatrixRatioPtrVector& Addition,
  const ConversionIndex* const CI, int area, double ratio) {

  if (!istagged)
    return;

  if ((Addition.numTagExperiments() > 0) && (Addition.numTagExperiments() <= tagAlkeys.numTagExperiments())) {
    tagAlkeys.Add(Addition, this->areaNum(area), *CI, ratio);
    tagAlkeys[this->areaNum(area)].updateRatio(Alkeys[this->areaNum(area)]);
  }
}

void Stock::addTags(AgeBandMatrixPtrVector* tagbyagelength, Tags* newtag, double tagloss) {

  if (!istagged)
    return;

  tagAlkeys.addTag(tagbyagelength, Alkeys, newtag->getName(), tagloss);
  allTags.resize(newtag);
  if (doesmature) {
    maturity->addMaturityTag(newtag->getName());
    matureTags.resize(newtag);
  }
  if (doesmove) {
    transition->addTransitionTag(newtag->getName());
    transitionTags.resize(newtag);
  }
  if (doesstray) {
    stray->addStrayTag(newtag->getName());
    strayTags.resize(newtag);
  }
}

void Stock::deleteTags(const char* tagname) {

  if (!istagged)
    return;

  allTags.Delete(tagAlkeys.getTagID(tagname));
  tagAlkeys.deleteTag(tagname);
  if (doesmature)
    maturity->deleteMaturityTag(tagname);
  if (doesmove)
    transition->deleteTransitionTag(tagname);
  if (doesstray)
    stray->deleteStrayTag(tagname);
}

double Stock::getTotalStockNumber(int area) const {
  int inarea = this->areaNum(area);
  if (inarea == -1)
    return 0.0;

  int age, len;
  double num = 0.0;
  for (age = Alkeys[inarea].minAge(); age <= Alkeys[inarea].maxAge(); age++)
    for (len = Alkeys[inarea].minLength(age); len < Alkeys[inarea].maxLength(age); len++)
      num += (Alkeys[inarea])[age][len].N;

  return num;
}

double Stock::getTotalStockNumberAllAreas() const {
  int a;
  double sum = 0.0;
  for (a = 0; a <= Alkeys.Size(); a++)
    sum += this->getTotalStockNumber(a);

  return sum;
}

double Stock::getTotalStockBiomass(int area) const {
  int inarea = this->areaNum(area);
  if (inarea == -1)
    return 0.0;

  int age, len;
  double kilos = 0.0;
  for (age = Alkeys[inarea].minAge(); age <= Alkeys[inarea].maxAge(); age++)
    for (len = Alkeys[inarea].minLength(age); len < Alkeys[inarea].maxLength(age); len++)
      kilos += ((Alkeys[inarea])[age][len].N * (Alkeys[inarea])[age][len].W);

  return kilos;
}

double Stock::getWeightedStockBiomass(int area,const FormulaVector & parameters) const {
  int inarea = this->areaNum(area);
  if (inarea == -1)
    return 0.0;

  int age, len;
  double kilos = 0.0;
  for (age = Alkeys[inarea].minAge(); age <= Alkeys[inarea].maxAge(); age++)
    for (len = Alkeys[inarea].minLength(age); len < Alkeys[inarea].maxLength(age); len++)
      kilos += parameters[2]*((Alkeys[inarea])[age][len].N * (Alkeys[inarea])[age][len].W)/(1.0+exp(-parameters[0]*(parameters[1]-LgrpDiv->meanLength(len))));

  return kilos;
}


double Stock::getTotalStockBiomassAllAreas() const {
  int a;
  double sum = 0.0;
  for (a = 0; a <= Alkeys.Size(); a++)
    sum += this->getTotalStockBiomass(a);

  return sum;
}

double Stock::getWeightedStockBiomassAllAreas(const FormulaVector & parameters) const {
  int a;
  double sum = 0.0;
  for (a = 0; a <= Alkeys.Size(); a++)
    sum += this->getWeightedStockBiomass(a,parameters);

  return sum;
}
