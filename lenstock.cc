#include "stock.h"
#include "keeper.h"
#include "areatime.h"
#include "naturalm.h"
#include "lennaturalm.h"
#include "cannibalism.h"
#include "grower.h"
#include "stockpredator.h"
#include "initialcond.h"
#include "migration.h"
#include "errorhandler.h"
#include "maturity.h"
#include "renewal.h"
#include "transition.h"
#include "spawner.h"
#include "readword.h"
#include "readaggregation.h"
#include "lenstock.h"
#include "readfunc.h"
#include "gadget.h"

extern ErrorHandler handle;

LenStock::LenStock(CommentStream& infile, const char* givenname,
  const AreaClass* const Area, const TimeClass* const TimeInfo,
  Keeper* const keeper) : Stock(givenname), cann_vec(0) {

  //Written by kgf 16/7 98
  year = -1;
  type = LENSTOCKTYPE;
  int i, tmpint;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  char filename[MaxStrLength];
  strncpy(filename, "", MaxStrLength);
  ifstream datafile;
  CommentStream subdata(datafile);
  keeper->setString(this->Name());

  //read the area data
  infile >> ws;
  if (infile.eof())
    handle.Eof();
  infile >> text;
  IntVector tmpareas;
  i = 0;
  if (strcasecmp(text, "livesonareas") == 0) {
    infile >> ws;
    while (isdigit(infile.peek()) && !infile.eof() && (i < Area->NoAreas())) {
      tmpareas.resize(1);
      infile >> tmpint >> ws;
      if ((tmpareas[i] = Area->InnerArea(tmpint)) == -1)
        handle.UndefinedArea(tmpint);
      i++;
    }
  } else
    handle.Unexpected("livesonareas", text);

  this->LetLiveOnAreas(tmpareas);

  //read the stock age and length data
  int minage, maxage;
  double minlength, maxlength, dl;
  readWordAndVariable(infile, "minage", minage);
  readWordAndVariable(infile, "maxage", maxage);
  readWordAndVariable(infile, "minlength", minlength);
  readWordAndVariable(infile, "maxlength", maxlength);
  readWordAndVariable(infile, "dl", dl);

  //JMB need to read the location of the reference weights file
  char refweight[MaxStrLength];
  strncpy(refweight, "", MaxStrLength);
  readWordAndValue(infile, "refweightfile", refweight);

  LgrpDiv = new LengthGroupDivision(minlength, maxlength, dl);
  if (LgrpDiv->Error())
    handle.Message("Error in stock - failed to create length group");

  //JMB need to set the lowerlgrp and size vectors to a default
  //value to allow the whole range of lengths to be calculated
  IntVector lowerlgrp(maxage - minage + 1, 0);
  IntVector size(maxage - minage + 1, LgrpDiv->NoLengthGroups());

  Alkeys.resize(areas.Size(), minage, lowerlgrp, size);
  tagAlkeys.resize(areas.Size(), minage, lowerlgrp, size);
  for (i = 0; i < Alkeys.Size(); i++)
    Alkeys[i].setToZero();

  //read the length group division used in Grower and in Predator
  DoubleVector grlengths;
  CharPtrVector grlenindex;

  readWordAndValue(infile, "growthandeatlengths", filename);
  datafile.open(filename, ios::in);
  handle.checkIfFailure(datafile, filename);
  handle.Open(filename);
  i = readLengthAggregation(subdata, grlengths, grlenindex);
  handle.Close();
  datafile.close();
  datafile.clear();

  LengthGroupDivision* GrowLgrpDiv = new LengthGroupDivision(grlengths);
  if (GrowLgrpDiv->Error())
    handle.Message("Error in stock - failed to create growth length group");

  //check the growth length groups cover the stock length groups
  checkLengthGroupIsFiner(LgrpDiv, GrowLgrpDiv);
  if (!(isZero(LgrpDiv->minLength() - GrowLgrpDiv->minLength())))
    handle.logWarning("Warning in stock - minimum lengths don't match for growth of", this->Name());
  if (!(isZero(LgrpDiv->maxLength() - GrowLgrpDiv->maxLength())))
    handle.logWarning("Warning in stock - maximum lengths don't match for growth of", this->Name());
  handle.logMessage("Read basic stock data for stock", this->Name());

  //read the growth function data
  readWordAndVariable(infile, "doesgrow", doesgrow);
  if (doesgrow)
    grower = new Grower(infile, LgrpDiv, GrowLgrpDiv, areas, TimeInfo, keeper, refweight, Area, grlenindex);
  else
    grower = 0;
  handle.logMessage("Read growth data for stock", this->Name());

  //read the prey data.
  readWordAndVariable(infile, "iseaten", iseaten);
  if (iseaten)
    prey = new MortPrey(infile, areas, this->Name(), minage, maxage, keeper, LgrpDiv);
  else
    prey = 0;
  handle.logMessage("Read prey data for stock", this->Name());

  if (iseaten) //check to see if cannibalism is included
    readWordAndVariable(infile, "cannibalism", cannibalism);
  else
    cannibalism = 0; //cannibalism is true only for stocks that are
                     //subject to cannibalism, ie immature stocks.
  if (cannibalism) {
    cann = new Cannibalism(infile, prey->returnLengthGroupDiv(), TimeInfo, keeper);
    cann_vec.resize(prey->returnLengthGroupDiv()->NoLengthGroups());
  } else
    cann = 0;
  handle.logMessage("Read cannibalism data for stock", this->Name());

  //read the predator data
  readWordAndVariable(infile, "doeseat", doeseat);
  if (doeseat) { //must be a new predator type for multispecies purpose
    //Predator not allowed in single species case.
    handle.logFailure("Error in stock - predator not allowed for single species model");
  } else
    predator = 0;

  if (iseaten && cann_vec.Size() == 0)
    cann_vec.resize(prey->returnLengthGroupDiv()->NoLengthGroups(), 0);

  //read the length dependant natural mortality data
  infile >> text;
  if (strcasecmp(text, "lennaturalm") == 0) {
    if (iseaten) //len_natm is normally added to total mortality in prey
      len_natm = new LenNaturalM(infile, prey->returnLengthGroupDiv(), keeper);
    else {
      if (predator != 0)
        len_natm = new LenNaturalM(infile, predator->returnLengthGroupDiv(), keeper);
      else
        len_natm = new LenNaturalM(infile, LgrpDiv, keeper);
    }
  } else
    handle.Unexpected("lennaturalm", text);
  handle.logMessage("Read natural mortality data for stock", this->Name());

  //read the initial condition data
  infile >> text;
  if (strcasecmp(text, "initialconditions") == 0)
    initial = new InitialCond(infile, areas, keeper, refweight, Area);
  else
    handle.Unexpected("initialconditions", text);
  handle.logMessage("Read initial conditions data for stock", this->Name());

  //read the migration data
  readWordAndVariable(infile, "doesmigrate", doesmigrate);
  if (doesmigrate) {
    readWordAndVariable(infile, "agedependentmigration", AgeDepMigration);
    readWordAndValue(infile, "migrationfile", filename);
    ifstream subfile;
    subfile.open(filename, ios::in);
    CommentStream subcomment(subfile);
    handle.checkIfFailure(subfile, filename);
    handle.Open(filename);
    migration = new Migration(subcomment, AgeDepMigration, areas, Area, TimeInfo, keeper);
    handle.Close();
    subfile.close();
    subfile.clear();

  } else
    migration = 0;
  handle.logMessage("Read migration data for stock", this->Name());

  //read the maturation data
  readWordAndVariable(infile, "doesmature", doesmature);
  if (doesmature) {
    readWordAndValue(infile, "maturityfunction", text);
    readWordAndValue(infile, "maturityfile", filename);
    ifstream subfile;
    subfile.open(filename, ios::in);
    CommentStream subcomment(subfile);
    handle.checkIfFailure(subfile, filename);
    handle.Open(filename);

    if (strcasecmp(text, "continuous") == 0)
      maturity = new MaturityA(subcomment, TimeInfo, keeper, minage, lowerlgrp, size, areas, LgrpDiv, 4);
    else if (strcasecmp(text, "fixedlength") == 0)
      maturity = new MaturityB(subcomment, TimeInfo, keeper, minage, lowerlgrp, size, areas, LgrpDiv);
    else if (strcasecmp(text, "constant") == 0)
      maturity = new MaturityC(subcomment, TimeInfo, keeper, minage, lowerlgrp, size, areas, LgrpDiv, 4);
    else if (strcasecmp(text, "constantweight") == 0)
      maturity = new MaturityD(subcomment, TimeInfo, keeper, minage, lowerlgrp, size, areas, LgrpDiv, 6, refweight);
    else if (strcasecmp(text, "ageandlength") == 0)
      handle.Message("Error in stock file - the ageandlength maturity function is no longer supported");
    else
      handle.Message("Error in stock file - unrecognised maturity", text);

    handle.Close();
    subfile.close();
    subfile.clear();

    if (!doesgrow)
      handle.Warning("The stock does not grow, so it is unlikely to mature!");

  } else
    maturity = 0;
  handle.logMessage("Read maturity data for stock", this->Name());

  //read the movement data
  readWordAndVariable(infile, "doesmove", doesmove);
  if (doesmove) {
    //transition handles the movements of the age group maxage:
    transition = new Transition(infile, areas, maxage, LgrpDiv, keeper);

  } else
    transition = 0;
  handle.logMessage("Read transition data for stock", this->Name());

  //read the renewal data
  readWordAndVariable(infile, "doesrenew", doesrenew);
  if (doesrenew) {
    readWordAndValue(infile, "renewaldatafile", filename);
    ifstream subfile;
    subfile.open(filename, ios::in);
    CommentStream subcomment(subfile);
    handle.checkIfFailure(subfile, filename);
    handle.Open(filename);
    renewal = new RenewalData(subcomment, areas, Area, TimeInfo, keeper);
    handle.Close();
    subfile.close();
    subfile.clear();

  } else
    renewal = 0;
  handle.logMessage("Read renewal data for stock", this->Name());

  //read the spawning data
  readWordAndVariable(infile, "doesspawn", doesspawn);
  if (doesspawn) {
    readWordAndValue(infile, "spawnfile", filename);
    ifstream subfile;
    subfile.open(filename, ios::in);
    CommentStream subcomment(subfile);
    handle.checkIfFailure(subfile, filename);
    handle.Open(filename);
    spawner = new Spawner(subcomment, maxage, LgrpDiv, Area, TimeInfo, keeper);
    handle.Close();
    subfile.close();
    subfile.clear();

  } else
    spawner = 0;
  handle.logMessage("Read spawning data for stock", this->Name());

  //read the filter data
  readWordAndVariable(infile, "filter", filter);

  infile >> ws;
  if (!infile.eof()) {
    infile >> text >> ws;
    handle.Unexpected("<end of file>", text);
  }

  //set the birthday for the stock
  birthdate = TimeInfo->StepsInYear();

  //Finished reading from infile - resize objects and clean up
  NumberInArea.AddRows(areas.Size(), LgrpDiv->NoLengthGroups());
  int nrofyears = TimeInfo->LastYear() - TimeInfo->FirstYear() + 1;
  for (i = 0; i < areas.Size(); i++) {
    F.resize(1, new DoubleMatrix(maxage - minage + 1, nrofyears, 0));
    M1.resize(1, new DoubleMatrix(maxage - minage + 1, nrofyears, 0));
    M2.resize(1, new DoubleMatrix(maxage - minage + 1, nrofyears, 0));
    Nbar.resize(1, new DoubleMatrix(maxage - minage + 1, nrofyears, 0));
    Nsum.resize(1, new DoubleMatrix(maxage - minage + 1, nrofyears, 0));
    bio.resize(1, new DoubleMatrix(maxage - minage + 1, nrofyears, 0));
  }
  C.resize(maxage - minage + 1);
  D1.resize(maxage - minage + 1);
  D2.resize(maxage - minage + 1);
  N.resize(maxage - minage + 1);
  E.resize(maxage - minage + 1);
  Z.resize(maxage - minage + 1);

  delete GrowLgrpDiv;
  for (i = 0; i < grlenindex.Size(); i++)
    delete[] grlenindex[i];
  keeper->clearAll();
}

LenStock::~LenStock() {
  int i;
  for (i = 0; i < F.Size(); i++) {
    delete F[i];
    delete M1[i];
    delete M2[i];
    delete Nbar[i];
    delete Nsum[i];
    delete bio[i];
  }

  if (iseaten && cannibalism) {
    for (i = 0; i < cann->nrOfPredators(); i++)
      delete (((MortPrey*)prey)->ageGroupMatrix(i));
  }

  if (len_natm != 0)
    delete len_natm;
  if (cann != 0)
    delete cann;

}

void LenStock::Reset(const TimeClass* const TimeInfo) {

  int i;
  if (TimeInfo->CurrentTime() == 1) {
    this->Clear();
    initial->Initialise(Alkeys);
    if (iseaten)
      prey->Reset();
    if (doesmature)
      maturity->Reset(TimeInfo);
    if (doesrenew)
      renewal->Reset();
    if (doesgrow)
      grower->Reset();
    if (doesmigrate)
      migration->Reset();
    if (doesmove)
      transition->Reset();
    if (doesspawn)
      spawner->Reset(TimeInfo);
    len_natm->NatCalc();
    year = -1;
    for (i = 0; i < Nsum.Size(); i++)
      Nsum[i]->setElementsTo(0.0);

    handle.logMessage("Reset stock data for stock", this->Name());
  }

  if (doeseat)
    predator->Reset(TimeInfo);
}

Prey* LenStock::returnPrey() const {
  return prey;
}

Predator* LenStock::returnPredator() const {
  return predator;
}

void LenStock::Print(ofstream& outfile) const {
  int i;

  outfile << "\nStock\nName" << sep << this->Name() << "\nLives on internal areas";

  for (i = 0; i < areas.Size(); i++)
    outfile << sep << areas[i];
  outfile << endl;

  outfile << "\nis eaten" << sep << iseaten << "\ndoes eat" << sep << doeseat
    << "\ndoes move" << sep << doesmove << "\ndoes spawn" << sep << doesspawn
    << "\ndoes mature" << sep << doesmature << "\ndoes renew" << sep << doesrenew
    << "\ndoes grow" << sep << doesgrow << "\ndoes migrate" << sep << doesmigrate << endl;

  LgrpDiv->Print(outfile);
  initial->Print(outfile);
  len_natm->Print(outfile);
  if (cannibalism)
    cann->Print(outfile);
  if (doesmature)
    maturity->Print(outfile);
  if (iseaten)
    prey->Print(outfile);
  if (doeseat)
    predator->Print(outfile);
  if (doesmove)
    transition->Print(outfile);
  if (doesrenew)
    renewal->Print(outfile);
  if (doesgrow)
    grower->Print(outfile);
  if (doesmigrate)
    migration->Print(outfile);
  if (doesspawn)
    spawner->Print(outfile);

  outfile << "\nAge length keys\n";
  for (i = 0; i < areas.Size(); i++) {
    outfile << "\tInternal area " << areas[i] << "\n\tNumbers\n";
    Alkeys[i].printNumbers(outfile);
    outfile << "\tMean weights\n";
    Alkeys[i].printWeights(outfile);
  }
}

void LenStock::calcEat(int area,
  const AreaClass* const Area, const TimeClass* const TimeInfo) {

  int i, nrofpredators;

  if (doeseat)
    predator->Eat(area, TimeInfo->LengthOfCurrent(),
      Area->Temperature(area, TimeInfo->CurrentTime()), Area->Size(area),
      TimeInfo->CurrentSubstep(), TimeInfo->NrOfSubsteps());

  if (cannibalism) {
    nrofpredators = cann->nrOfPredators();
    cann_vec.setElementsTo(0.0);
    for (i = 0; i < nrofpredators; i++) {
      // add cannibalism mortality on this prey from each predator in turn
      cann_vec += cann->Mortality(Alkeys[AreaNr[area]],
        cannPredators[i]->Agelengthkeys(area), LgrpDiv,
        cannPredators[i]->returnLengthGroupDiv(), TimeInfo, i, len_natm->NatMortality());
    }

    for (i = 0; i < nrofpredators; i++) {
      // calculate consumption of this prey by each predator in turn
      prey->setConsumption(area, i, cann->getCons(i));
      prey->setAgeMatrix(i, area, cann->getAgeGroups(i));
    }
    prey->setCannibalism(area, cann_vec);
  }
}

void LenStock::checkEat(int area,
  const AreaClass* const Area, const TimeClass* const TimeInfo) {

  if (iseaten) {
    prey->calcZ(area, len_natm->NatMortality()); //calculate total mortality
    prey->calcMeanN(area);
  }
}

void LenStock::adjustEat(int area,
  const AreaClass* const Area, const TimeClass* const TimeInfo) {
}

void LenStock::ReducePop(int area,
  const AreaClass* const Area, const TimeClass* const TimeInfo) {

  //written by kgf 31/7 98
  //Apply total mortality over present substep
  int inarea = AreaNr[area];
  if (!iseaten && (predator == 0)) //no mortality applied
    return;
  if (iseaten) //propSurv is meaningsfull only for mortality models
    prey->Multiply(Alkeys[inarea], ((MortPrey*)prey)->propSurv(area));
  else //apply only natural mortality on stock
    predator->Multiply(Alkeys[inarea], len_natm->NatMortality());

  if (tagAlkeys.numTagExperiments() > 0)
    tagAlkeys[inarea].updateNumbers(Alkeys[AreaNr[area]]);
}

void LenStock::CalcNumbers(int area,
  const AreaClass* const Area, const TimeClass* const TimeInfo) {

  calcDone = 0;
  int inarea = AreaNr[area];
  PopInfo nullpop;

  int i;
  for (i = 0; i < NumberInArea[inarea].Size(); i++)
    NumberInArea[inarea][i] = nullpop;

  if (doesrenew)
    this->Renewal(area, TimeInfo);

  Alkeys[inarea].Colsum(NumberInArea[inarea]);

  if (doesgrow)
    grower->Sum(NumberInArea[inarea], area);
  if (iseaten)
    prey->Sum(Alkeys[inarea], area, TimeInfo->CurrentSubstep());
  if (doeseat)
    ((MortPredator*)predator)->Sum(NumberInArea[inarea], area);

  N.setElementsTo(0.0);
  int row, col;
  for (row = Alkeys[area].minAge(); row <= Alkeys[area].maxAge(); row++)
    for (col = Alkeys[area].minLength(row); col < Alkeys[area].maxLength(row); col++)
      N[row-Alkeys[area].minAge()] += Alkeys[area][row][col].N;

  if (TimeInfo->CurrentStep() == 1) {
    for (row = Alkeys[area].minAge(); row <= Alkeys[area].maxAge(); row++)
      for (col = Alkeys[area].minLength(row); col < Alkeys[area].maxLength(row); col++)
        (*Nsum[inarea])[row - Alkeys[area].minAge()][year + 1] += Alkeys[area][row][col].N;
    calcBiomass(year + 1, area);
  }
}

void LenStock::calcBiomass(int yr, int area) {
  int inarea = AreaNr[area];
  int row, col;
  for (row = Alkeys[area].minAge(); row <= Alkeys[area].maxAge(); row++)
    for (col = Alkeys[area].minLength(row); col < Alkeys[area].maxLength(row); col++)
      (*bio[inarea])[row - Alkeys[area].minAge()][yr] +=
        Alkeys[area][row][col].N * Alkeys[area][row][col].W;
}

void LenStock::Grow(int area,
  const AreaClass* const Area, const TimeClass* const TimeInfo) {

  if (!doesgrow && doesmature)
    handle.logFailure("Error in stock - maturation without growth is not implemented");

  if (!doesgrow)
    return;

  int inarea = AreaNr[area];
  grower->GrowthCalc(area, Area, TimeInfo);

  if (grower->getGrowthType() != 6 && grower->getGrowthType() != 7) {
    //New weights at length are calculated
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

  } else { //GrowthCalcF || GrowthCalcG
    grower->GrowthImplement(area, LgrpDiv);
    if (doesmature) {
      if (maturity->IsMaturationStep(area, TimeInfo)) {
        Alkeys[inarea].Grow(grower->LengthIncrease(area), grower->getWeight(area), maturity, TimeInfo, Area, area);
        if (tagAlkeys.numTagExperiments() > 0)
          tagAlkeys[inarea].Grow(grower->LengthIncrease(area), Alkeys[inarea], maturity, TimeInfo, Area, area);
      } else {
        Alkeys[inarea].Grow(grower->LengthIncrease(area), grower->getWeight(area));
        if (tagAlkeys.numTagExperiments() > 0)
          tagAlkeys[inarea].Grow(grower->LengthIncrease(area), Alkeys[inarea]);
      }
    } else {
      Alkeys[inarea].Grow(grower->LengthIncrease(area), grower->getWeight(area));
      if (tagAlkeys.numTagExperiments() > 0)
        tagAlkeys[inarea].Grow(grower->LengthIncrease(area), Alkeys[inarea]);
    }
  }
  Alkeys[inarea].FilterN(filter); //mnaa
  if (tagAlkeys.numTagExperiments() > 0)
    tagAlkeys[inarea].updateNumbers(Alkeys[AreaNr[area]]);
}

void LenStock::calcForPrinting(int area, const TimeClass& time) {
  if (!iseaten || !Type() == LENSTOCKTYPE || calcDone)
    return;
  int row, col, mcol;
  const AgeBandMatrix& mean_n = ((MortPrey*)prey)->getMeanN(area);
  const DoubleVector& cons = prey->getCons(area);

  if (time.CurrentStep() == 1)
    year = time.CurrentYear() - time.FirstYear();

  C.setElementsTo(0.0);
  D1.setElementsTo(0.0);
  D2.setElementsTo(0.0);

  for (row = mean_n.minAge(); row <= mean_n.maxAge(); row++)
    for (col = 0; col < cons.Size(); col++) {
      mcol = col + mean_n.minLength(row);
      C[row - mean_n.minAge()] += mean_n[row][mcol].N*cons[col];
      D1[row - mean_n.minAge()] += mean_n[row][mcol].N*len_natm->NatMortality()[col];
      D2[row - mean_n.minAge()] += mean_n[row][mcol].N*cann_vec[col];
      (*Nbar[area])[row - mean_n.minAge()][year] += mean_n[row][mcol].N;
    }

  for (row = 0; row < E.Size(); row++) {
    E[row] = C[row] + D1[row] + D2[row];
    if (N[row] > E[row]) {
      Z[row] = log(N[row] / (N[row] - E[row]));
      (*(F[area]))[row][year] += C[row] * Z[row] / E[row];
      (*(M1[area]))[row][year] += D1[row] * Z[row] / E[row];
      (*(M2[area]))[row][year] += D2[row] * Z[row] / E[row];
    }
  }
  calcDone = 1;
}

void LenStock::SecondSpecialTransactions(int area,
  const AreaClass* const Area, const TimeClass* const TimeInfo) {

  if (doesmature)
    if (maturity->IsMaturationStep(area, TimeInfo)) {
      updateMatureStockWithTags(TimeInfo);
      maturity->Move(area, TimeInfo);
    }
}

void LenStock::setStock(StockPtrVector& stockvec) {
  int i, j, found;
  int minage, maxage, tmpsize;

  if (doesmature)
    maturity->setStock(stockvec);
  if (doesmove)
    transition->setStock(stockvec);

  if (iseaten && cannibalism) {
    ((MortPrey*)prey)->cannIsTrue(1);
    ((MortPrey*)prey)->setDimensions(areas.Size(), cann->nrOfPredators());

    for (i = 0; i < cann->nrOfPredators(); i++) {
      found = 0;
      for (j = 0; j < stockvec.Size(); j++)
        if (strcasecmp(stockvec[j]->Name(), cann->predatorName(i)) == 0) {
          found++;
          cannPredators.resize(1, stockvec[j]);
        }

      if (found == 0)
        handle.logFailure("Error in cannibalism - failed to match predator", cann->predatorName(i));

      minage = cann->getMinPredAge(i);
      maxage = cann->getMaxPredAge(i);
      tmpsize = maxage - minage + 1;

      if (cannPredators[i]->minAge() != minage || cannPredators[i]->maxAge() != maxage)
        handle.logFailure("Error in cannibalism - failed to match ages for predator", cann->predatorName(i));

      ((MortPrey*)prey)->addCannPredName(cann->predatorName(i));
      ((MortPrey*)prey)->setMinPredAge(minage);
      ((MortPrey*)prey)->setMaxPredAge(maxage);

      BandMatrix tmp(0, prey->returnLengthGroupDiv()->NoLengthGroups(), minage, tmpsize, 0.0);
      DoubleMatrix* stmp = new DoubleMatrix(areas.Size(), tmpsize, 0.0);
      ((MortPrey*)prey)->addConsMatrix(i, tmp);
      ((MortPrey*)prey)->addAgeGroupMatrix(stmp);
    }
  }

  if (doesspawn)
    spawner->setStock(stockvec);
}

const DoubleVector& LenStock::mortality() const {
  return len_natm->NatMortality();
}
