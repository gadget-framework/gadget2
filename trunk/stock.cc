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
#include "errorhandler.h"
#include "maturity.h"
#include "renewal.h"
#include "transition.h"
#include "spawner.h"
#include "stray.h"
#include "readword.h"
#include "readaggregation.h"
#include "gadget.h"
#include "global.h"

Stock::Stock(CommentStream& infile, const char* givenname,
  const AreaClass* const Area, const TimeClass* const TimeInfo, Keeper* const keeper)
  : BaseClass(givenname), stray(0), spawner(0), renewal(0), maturity(0), transition(0),
    migration(0), prey(0), predator(0), initial(0), LgrpDiv(0), grower(0), naturalm(0) {

  doesgrow = doeseat = iseaten = doesmigrate = istagged = 0;
  doesmove = doesrenew = doesmature = doesspawn = doesstray = 0;
  int i, tmpint = 0;
  char c;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  char filename[MaxStrLength];
  strncpy(filename, "", MaxStrLength);
  IntVector tmpareas;

  ifstream datafile;
  CommentStream subdata(datafile);
  keeper->setString(this->getName());

  //read the area data
  infile >> text >> ws;
  if (strcasecmp(text, "livesonareas") != 0)
    handle.logFileUnexpected(LOGFAIL, "livesonareas", text);

  c = infile.peek();
  while (isdigit(c) && !infile.eof()) {
    infile >> tmpint >> ws;
    tmpareas.resize(1, Area->getInnerArea(tmpint));
    c = infile.peek();
  }
  this->storeAreas(tmpareas);

  //read the stock age and length data
  int minage, maxage, numage;
  double minlength, maxlength, dl;
  readWordAndVariable(infile, "minage", minage);
  readWordAndVariable(infile, "maxage", maxage);
  numage = maxage - minage + 1;
  if (minage < 0)
    handle.logMessage(LOGFAIL, "Error in stock - stock age must be positive");
  if (numage < 1)
    handle.logMessage(LOGFAIL, "Error in stock - failed to create age groups");

  readWordAndVariable(infile, "minlength", minlength);
  readWordAndVariable(infile, "maxlength", maxlength);
  readWordAndVariable(infile, "dl", dl);
  if (minlength < 0.0)
    handle.logMessage(LOGFAIL, "Error in stock - stock length must be positive");
  LgrpDiv = new LengthGroupDivision(minlength, maxlength, dl);
  if (LgrpDiv->Error() || isZero(dl))
    handle.logMessage(LOGFAIL, "Error in stock - failed to create length group");

  //JMB need to read the location of the reference weights file
  char refweight[MaxStrLength];
  strncpy(refweight, "", MaxStrLength);
  readWordAndValue(infile, "refweightfile", refweight);

  //JMB need to set the lowerlgrp and size vectors to a default
  //value to allow the whole range of lengths to be calculated
  IntVector lower(numage, 0);
  IntVector agesize(numage, LgrpDiv->numLengthGroups());
  Alkeys.resize(areas.Size(), minage, lower, agesize);
  for (i = 0; i < Alkeys.Size(); i++)
    Alkeys[i].setToZero();

  //read the growth length group data
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
    handle.logMessage(LOGFAIL, "Error in stock - failed to create growth length group for", this->getName());

  //Check the growth length groups cover the stock length groups
  if (!checkLengthGroupStructure(LgrpDiv, GrowLgrpDiv))
    handle.logMessage(LOGFAIL, "Error in stock - invalid length group structure for growth of", this->getName());
  if (LgrpDiv->minLength() < GrowLgrpDiv->minLength())
    handle.logMessage(LOGFAIL, "Error in stock - invalid minimum length group for growth of", this->getName());
  if (!isSmall(LgrpDiv->minLength() - GrowLgrpDiv->minLength()))
    handle.logMessage(LOGWARN, "Warning in stock - minimum lengths don't match for growth of", this->getName());
  if (LgrpDiv->maxLength() > GrowLgrpDiv->maxLength())
    handle.logMessage(LOGFAIL, "Error in stock - invalid maximum length group for growth of", this->getName());
  if (!isSmall(LgrpDiv->maxLength() - GrowLgrpDiv->maxLength()))
    handle.logMessage(LOGWARN, "Warning in stock - maximum lengths don't match for growth of", this->getName());

  //JMB check that the growth is defined on evenly spaced length groups
  if (isZero(GrowLgrpDiv->dl()))
    handle.logMessage(LOGWARN, "Warning in stock - growth length structure not evenly spaced for", this->getName());
  handle.logMessage(LOGMESSAGE, "Read basic stock data for stock", this->getName());

  //read the growth function data
  PopInfo nullpop;
  tmpPopulation.AddRows(areas.Size(), LgrpDiv->numLengthGroups(), nullpop);
  readWordAndVariable(infile, "doesgrow", doesgrow);
  if (doesgrow) {
    grower = new Grower(infile, LgrpDiv, GrowLgrpDiv, areas, TimeInfo, keeper, refweight, this->getName(), Area, grlenindex);

  } else
    grower = 0;
  handle.logMessage(LOGMESSAGE, "Read growth data for stock", this->getName());

  //read the natural mortality data
  infile >> text;
  if (strcasecmp(text, "naturalmortality") != 0)
    handle.logFileUnexpected(LOGFAIL, "naturalmortality", text);
  naturalm = new NaturalMortality(infile, minage, numage, this->getName(), areas, TimeInfo, keeper);
  handle.logMessage(LOGMESSAGE, "Read natural mortality data for stock", this->getName());

  //read the prey data
  readWordAndVariable(infile, "iseaten", iseaten);
  if (iseaten) {
    prey = new StockPrey(infile, areas, this->getName(), minage, numage, TimeInfo, keeper);

  } else
    prey = 0;
  handle.logMessage(LOGMESSAGE, "Read prey data for stock", this->getName());

  //read the predator data
  readWordAndVariable(infile, "doeseat", doeseat);
  if (doeseat) {
    predator = new StockPredator(infile, this->getName(), areas, LgrpDiv,
      GrowLgrpDiv, minage, numage, TimeInfo, keeper);

  } else
    predator = 0;
  handle.logMessage(LOGMESSAGE, "Read predator data for stock", this->getName());

  //read the initial conditions
  infile >> text;
  if (strcasecmp(text, "initialconditions") != 0)
    handle.logFileUnexpected(LOGFAIL, "initialconditions", text);
  initial = new InitialCond(infile, areas, keeper, refweight, this->getName(), Area, dl);
  handle.logMessage(LOGMESSAGE, "Read initial conditions data for stock", this->getName());

  //read the migration data
  readWordAndVariable(infile, "doesmigrate", doesmigrate);
  if (doesmigrate) {
    tmpMigrate.resizeBlank(areas.Size());
    infile >> ws;
    c = infile.peek();
    if ((c == 'y') || (c == 'Y')) {
      //about to read the word yearstepfile
      migration = new MigrationNumbers(infile, areas, Area, TimeInfo, this->getName(), keeper);
    } else if ((c == 'd') || (c == 'D')) {
      //about to read the word diffusion
      migration = new MigrationFunction(infile, areas, Area, TimeInfo, this->getName(), keeper);
    } else {
      infile >> text >> ws;
      handle.logFileUnexpected(LOGFAIL, "migrationnumbers or migrationfunction", text);
    }

  } else
    migration = 0;
  handle.logMessage(LOGMESSAGE, "Read migration data for stock", this->getName());

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

    if (strcasecmp(text, "continuous") == 0) {
      maturity = new MaturityA(subcomment, TimeInfo, keeper, minage, numage, areas, this->getName(), LgrpDiv);

    } else if (strcasecmp(text, "fixedlength") == 0) {
      maturity = new MaturityB(subcomment, TimeInfo, keeper, minage, numage, areas, this->getName(), LgrpDiv);

    } else if (strcasecmp(text, "newconstant") == 0) {
      maturity = new MaturityC(subcomment, TimeInfo, keeper, minage, numage, areas, this->getName(), LgrpDiv, 4);

    } else if (strcasecmp(text, "newconstantweight") == 0) {
      maturity = new MaturityD(subcomment, TimeInfo, keeper, minage, numage, areas, this->getName(), LgrpDiv, 6, refweight);

    } else if (strcasecmp(text, "ageandlength") == 0) {
      handle.logMessage(LOGFAIL, "\nThe ageandlength maturity function is no longer supported");

    } else if (strcasecmp(text, "constant") == 0) {
      handle.logMessage(LOGFAIL, "\nThe constant maturity function is no longer supported\nUse the newconstant maturity function instead\nNote that this function has had a factor of 4 removed from the source code");

    } else if (strcasecmp(text, "constantweight") == 0) {
      handle.logMessage(LOGFAIL, "\nThe constantweight maturity function is no longer supported\nUse the newconstantweight maturity function instead\nNote that this function has had a factor of 4 removed from the source code");

    } else
      handle.logMessage(LOGFAIL, "unrecognised maturity function", text);

    handle.Close();
    subfile.close();
    subfile.clear();

    if (!doesgrow)
      handle.logMessage(LOGFAIL, "Error in stock - maturation without growth is not implemented");

  } else
    maturity = 0;
  handle.logMessage(LOGMESSAGE, "Read maturity data for stock", this->getName());

  //read the movement data
  readWordAndVariable(infile, "doesmove", doesmove);
  if (doesmove) {
    //transition handles the movements of the age group maxage
    transition = new Transition(infile, areas, maxage, LgrpDiv, this->getName(), TimeInfo, keeper);

  } else
    transition = 0;
  handle.logMessage(LOGMESSAGE, "Read transition data for stock", this->getName());

  //read the renewal data
  readWordAndVariable(infile, "doesrenew", doesrenew);
  if (doesrenew) {
    renewal = new RenewalData(infile, areas, Area, TimeInfo, keeper, refweight, this->getName(), minage, maxage, dl);

  } else
    renewal = 0;
  handle.logMessage(LOGMESSAGE, "Read renewal data for stock", this->getName());

  //read the spawning data
  readWordAndVariable(infile, "doesspawn", doesspawn);
  if (doesspawn) {
    readWordAndValue(infile, "spawnfile", filename);
    ifstream subfile;
    subfile.open(filename, ios::in);
    CommentStream subcomment(subfile);
    handle.checkIfFailure(subfile, filename);
    handle.Open(filename);
    spawner = new SpawnData(subcomment, maxage, LgrpDiv, areas, Area, this->getName(), TimeInfo, keeper);
    handle.Close();
    subfile.close();
    subfile.clear();

  } else
    spawner = 0;
  handle.logMessage(LOGMESSAGE, "Read spawning data for stock", this->getName());

  infile >> ws;
  if (!infile.eof()) {
    //read the optional straying data
    readWordAndVariable(infile, "doesstray", doesstray);
    if (doesstray) {
      readWordAndValue(infile, "strayfile", filename);
      ifstream subfile;
      subfile.open(filename, ios::in);
      CommentStream subcomment(subfile);
      handle.checkIfFailure(subfile, filename);
      handle.Open(filename);
      stray = new StrayData(subcomment, LgrpDiv, areas, Area, this->getName(), TimeInfo, keeper);
      handle.Close();
      subfile.close();
      subfile.clear();

    } else
      stray = 0;
    handle.logMessage(LOGMESSAGE, "Read straying data for stock", this->getName());
  }

  //set the birthday for the stock
  birthdate = TimeInfo->numSteps();

  //finished reading from infile
  delete GrowLgrpDiv;
  for (i = 0; i < grlenindex.Size(); i++)
    delete[] grlenindex[i];
  keeper->clearAll();
}

Stock::Stock(const char* givenname)
  : BaseClass(givenname), stray(0), spawner(0), renewal(0), maturity(0), transition(0),
    migration(0), prey(0), predator(0), initial(0), LgrpDiv(0), grower(0), naturalm(0) {

  doesgrow = doeseat = iseaten = doesmigrate = istagged = 0;
  doesmove = doesrenew = doesmature = doesspawn = doesstray = 0;
}

Stock::~Stock() {
  if (migration != 0)
    delete migration;
  if (prey != 0)
    delete prey;
  if (predator != 0)
    delete predator;
  if (initial != 0)
    delete initial;
  if (LgrpDiv != 0)
    delete LgrpDiv;
  if (grower != 0)
    delete grower;
  if (naturalm != 0)
    delete naturalm;
  if (transition != 0)
    delete transition;
  if (renewal != 0)
    delete renewal;
  if (maturity != 0)
    delete maturity;
  if (spawner != 0)
    delete spawner;
  if (stray != 0)
    delete stray;
}

void Stock::Reset(const TimeClass* const TimeInfo) {
  naturalm->Reset(TimeInfo);
  if (doeseat)
    predator->Reset(TimeInfo);
  if (doesmature)
    maturity->Reset(TimeInfo);
  if (doesspawn)
    spawner->Reset(TimeInfo);
  if (doesstray)
    stray->Reset(TimeInfo);
  if (iseaten)
    prey->Reset(TimeInfo);

  if (TimeInfo->getTime() == 1) {
    initial->Initialise(Alkeys);
    if (doesrenew)
      renewal->Reset();
    if (doesgrow)
      grower->Reset();
    if (doesmigrate)
      migration->Reset();
    if (doesmove)
      transition->Reset();
  }

  if (handle.getLogLevel() >= LOGMESSAGE)
    handle.logMessage(LOGMESSAGE, "Reset stock data for stock", this->getName());
}

void Stock::setStock(StockPtrVector& stockvec) {

  initial->setCI(LgrpDiv);
  if (iseaten)
    prey->setCI(LgrpDiv);
  if (doesrenew)
    renewal->setCI(LgrpDiv);

  //JMB for the stock variable stuff to work we need to initialise the population
  //very early in the simulation so we do it here and then do it again in the
  //reset() function that gets called later (as part of the optimisation runs)
  initial->Initialise(Alkeys);

  int i;
  StockPtrVector tmpStockVector;
  if (doesmature) {
    maturity->setStock(stockvec);
    tmpStockVector = maturity->getMatureStocks();
    for (i = 0; i < tmpStockVector.Size(); i++)
      if (strcasecmp(tmpStockVector[i]->getName(), this->getName()) == 0)
        handle.logMessage(LOGWARN, "Warning in stock - stock maturing into itself", this->getName());
  }
  if (doesmove) {
    transition->setStock(stockvec);
    tmpStockVector = transition->getTransitionStocks();
    for (i = 0; i < tmpStockVector.Size(); i++)
      if (strcasecmp(tmpStockVector[i]->getName(), this->getName()) == 0)
        handle.logMessage(LOGWARN, "Warning in stock - stock moving into itself", this->getName());
  }
  if (doesstray) {
    stray->setStock(stockvec);
    tmpStockVector = stray->getStrayStocks();
    for (i = 0; i < tmpStockVector.Size(); i++)
      if (strcasecmp(tmpStockVector[i]->getName(), this->getName()) == 0)
        handle.logMessage(LOGWARN, "Warning in stock - stock straying into itself", this->getName());
  }
  if (doesspawn)
    spawner->setStock(stockvec);
}

void Stock::setTagged() {
  //resize tagAlkeys to be the same size as Alkeys
  int i, minage, maxage;
  minage = Alkeys[0].minAge();
  maxage = Alkeys[0].maxAge();
  IntVector lower(maxage - minage + 1, 0);
  IntVector size(maxage - minage + 1, LgrpDiv->numLengthGroups());
  tagAlkeys.resize(areas.Size(), minage, lower, size);
  for (i = 0; i < tagAlkeys.Size(); i++)
    tagAlkeys[i].setToZero();

  if (doesmature)
    maturity->setTagged();
  if (doesmove)
    transition->setTagged();
  if (doesstray)
    stray->setTagged();
}

void Stock::Print(ofstream& outfile) const {

  outfile << "\nStock\nName " << this->getName() << "\nLives on internal areas";

  int i;
  for (i = 0; i < areas.Size(); i++)
    outfile << sep << areas[i];
  outfile << endl;

  outfile << "\ndoes grow " << doesgrow << "\nis eaten " << iseaten
    << "\ndoes eat " << doeseat << "\ndoes migrate " << doesmigrate
    << "\ndoes mature " << doesmature << "\ndoes move " << doesmove
    << "\ndoes renew " << doesrenew << "\ndoes spawn " << doesspawn
    << "\ndoes stray " << doesstray << "\nis tagged " << istagged << endl << endl;

  LgrpDiv->Print(outfile);
  initial->Print(outfile);
  naturalm->Print(outfile);
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
  if (doesstray)
    stray->Print(outfile);

  outfile << "\nAge length keys\n";
  for (i = 0; i < areas.Size(); i++) {
    outfile << "\tInternal area " << areas[i] << "\n\tNumber\n";
    Alkeys[i].printNumbers(outfile);
    outfile << "\tMean weight\n";
    Alkeys[i].printWeights(outfile);
  }
}

int Stock::isBirthday(const TimeClass* const TimeInfo) const {
  return (TimeInfo->getStep() == birthdate);
}

StockPrey* Stock::getPrey() const {
  if (!iseaten)
    handle.logMessage(LOGFAIL, "Error in stock - no prey defined for", this->getName());
  return prey;
}

Migration* Stock::getMigration() const {
  if (!doesmigrate)
    handle.logMessage(LOGFAIL, "Error in stock - no migration defined for", this->getName());
  return migration;
}

PopPredator* Stock::getPredator() const {
  if (!doeseat)
    handle.logMessage(LOGFAIL, "Error in stock - no predator defined for", this->getName());
  return predator;
}

const StockPtrVector& Stock::getMatureStocks() {
  if (!doesmature)
    handle.logMessage(LOGFAIL, "Error in stock - no mature stocks defined for", this->getName());
  return maturity->getMatureStocks();
}

const StockPtrVector& Stock::getTransitionStocks() {
  if (!doesmove)
    handle.logMessage(LOGFAIL, "Error in stock - no transition stocks defined for", this->getName());
  return transition->getTransitionStocks();
}

const StockPtrVector& Stock::getStrayStocks() {
  if (!doesstray)
    handle.logMessage(LOGFAIL, "Error in stock - no straying stocks defined for", this->getName());
  return stray->getStrayStocks();
}

AgeBandMatrix& Stock::getConsumptionALK(int area) {
//  if (!iseaten)
//    handle.logMessage(LOGWARN, "Error in stock - no prey for", this->getName());
//  return prey->getConsumptionALK(area);
  return Alkeys[this->areaNum(area)];
}
