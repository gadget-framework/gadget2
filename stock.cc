#include "stock.h"
#include "keeper.h"
#include "areatime.h"
#include "naturalm.h"
#include "lennaturalm.h"
#include "grower.h"
#include "stockprey.h"
#include "mortprey.h"
#include "stockpredator.h"
#include "initialcond.h"
#include "migration.h"
#include "readfunc.h"
#include "errorhandler.h"
#include "maturity.h"
#include "renewal.h"
#include "transition.h"
#include "spawner.h"
#include "readword.h"
#include "readaggregation.h"
#include "gadget.h"

extern ErrorHandler handle;

Stock::Stock(CommentStream& infile, const char* givenname,
  const AreaClass* const Area, const TimeClass* const TimeInfo, Keeper* const keeper)
  : BaseClass(givenname), spawner(0), renewal(0), maturity(0), transition(0),
    migration(0), prey(0), predator(0), initial(0), LgrpDiv(0), grower(0), NatM(0) {

  type = STOCKTYPE;
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
  if (strcasecmp(text, "livesonareas") == 0) {
    infile >> ws;
    i = 0;
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
    handle.Message("Error in stock - failed to create growth length group");

  //Check the growth length groups cover the stock length groups
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

  //read the natural mortality data
  infile >> text;
  if (strcasecmp(text, "naturalmortality") == 0)
    NatM = new NaturalM(infile, minage, maxage, TimeInfo, keeper);
  else
    handle.Unexpected("naturalmortality", text);
  handle.logMessage("Read natural mortality data for stock", this->Name());

  //read the prey data
  readWordAndVariable(infile, "iseaten", iseaten);
  if (iseaten)
    prey = new StockPrey(infile, areas, this->Name(), minage, maxage, keeper);
  else
    prey = 0;
  handle.logMessage("Read prey data for stock", this->Name());

  //read the predator data
  readWordAndVariable(infile, "doeseat", doeseat);
  if (doeseat)
    predator = new StockPredator(infile, this->Name(), areas, LgrpDiv,
      GrowLgrpDiv, minage, maxage, TimeInfo, keeper);
  else
    predator = 0;
  handle.logMessage("Read predator data for stock", this->Name());

  //read the initial conditions
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

  infile >> ws;
  if (!infile.eof()) {
    infile >> text >> ws;
    handle.Unexpected("<end of file>", text);
  }

  //set the birthday for the stock
  birthdate = TimeInfo->StepsInYear();

  //Finished reading from infile - resize objects and clean up
  NumberInArea.AddRows(areas.Size(), LgrpDiv->NoLengthGroups());
  delete GrowLgrpDiv;
  for (i = 0; i < grlenindex.Size(); i++)
    delete[] grlenindex[i];
  keeper->clearAll();
}

Stock::Stock(const char* givenname)
  : BaseClass(givenname), spawner(0), renewal(0), maturity(0), transition(0),
    migration(0), prey(0), predator(0), initial(0), LgrpDiv(0), grower(0), NatM(0) {

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
  if (NatM != 0)
    delete NatM;
  if (transition != 0)
    delete transition;
  if (renewal != 0)
    delete renewal;
  if (maturity != 0)
    delete maturity;
  if (spawner != 0)
    delete spawner;
}

void Stock::Reset(const TimeClass* const TimeInfo) {
  //What is only done in the beginning.
  //Naturalm, maturity and growth will later be moved.

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

    handle.logMessage("Reset stock data for stock", this->Name());
  }
  NatM->Reset(TimeInfo);
  if (doeseat)
    predator->Reset(TimeInfo);
}

void Stock::Clear() {
  if (doesmigrate)
    migration->Clear();
}

Prey* Stock::returnPrey() const {
  return prey;
}

PopPredator* Stock::returnPredator() const {
  return predator;
}

void Stock::setStock(StockPtrVector& stockvec) {
  if (doesmature)
    maturity->setStock(stockvec);
  if (doesmove)
    transition->setStock(stockvec);
  if (doesspawn)
    spawner->setStock(stockvec);
}

void Stock::setCI() {
  initial->setCI(LgrpDiv);
  if (iseaten)
    prey->setCI(LgrpDiv);
  if (doesrenew)
    renewal->setCI(LgrpDiv);
}

void Stock::Print(ofstream& outfile) const {
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
  NatM->Print(outfile);
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

int Stock::Birthday(const TimeClass* const TimeInfo) const {
  return (TimeInfo->CurrentStep() == birthdate);
}

const AgeBandMatrix& Stock::Agelengthkeys(int area) const {
  return Alkeys[AreaNr[area]];
}

const AgeBandMatrix& Stock::getMeanN(int area) const {
  assert(iseaten && prey->Type() == MORTPREYTYPE);
  return (((MortPrey*)prey)->getMeanN(area));
}

const StockPtrVector& Stock::getMatureStocks() {
  if (doesmature == 0)
    handle.logFailure("Error in stock - no mature stocks defined for", this->Name());
  return maturity->getMatureStocks();
}

const StockPtrVector& Stock::getTransitionStocks() {
  if (doesmove == 0)
    handle.logFailure("Error in stock - no transition stocks defined for", this->Name());
  return transition->getTransitionStocks();
}

const DoubleIndexVector& Stock::mortality() const {
  return NatM->getMortality();
}
