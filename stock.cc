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
#include "maturitya.h"
#include "maturityb.h"
#include "maturityc.h"
#include "maturityd.h"
#include "renewal.h"
#include "transition.h"
#include "catch.h"
#include "print.h"
#include "spawner.h"
#include "readword.h"
#include "readaggregation.h"
#include "checkconversion.h"
#include "gadget.h"

Stock::Stock(CommentStream& infile, const char* givenname,
  const AreaClass* const Area, const TimeClass* const TimeInfo, Keeper* const keeper)
  : BaseClass(givenname), spawner(0), catchptr(0), renewal(0), maturity(0),
  transition(0), migration(0), prey(0), predator(0), initial(0), LgrpDiv(0), grower(0), NatM(0) {

  haslgr = 1; //fixme: read this in from data file?
  int i, tmpint;
  ErrorHandler handle;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  keeper->SetString(this->Name());

  //Read the area data
  infile >> ws;
  if (infile.eof())
    handle.Eof();
  infile >> text;
  intvector tmpareas;
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

  //Read the stock age and length data
  int minage;
  int maxage;
  double minlength;
  double maxlength;
  double dl;
  ReadWordAndVariable(infile, "minage", minage);
  ReadWordAndVariable(infile, "maxage", maxage);
  ReadWordAndVariable(infile, "minlength", minlength);
  ReadWordAndVariable(infile, "maxlength", maxlength);
  ReadWordAndVariable(infile, "dl", dl);

  //JMB need to read the location of the reference weights file
  char refweight[MaxStrLength];
  strncpy(refweight, "", MaxStrLength);
  ReadWordAndValue(infile, "refweightfile", refweight);

  LgrpDiv = new LengthGroupDivision(minlength, maxlength, dl);
  if (LgrpDiv->Error())
    LengthGroupPrintError(minlength, maxlength, dl, keeper);

  //JMB need to set the lowerlgrp and size vectors to a default
  //value to allow the whole range of lengths to be calculated
  intvector lowerlgrp(maxage - minage + 1, 0);
  intvector size(maxage - minage + 1);
  for (i = 0; i < maxage - minage + 1; i++)
    size[i] = int((maxlength - minlength) / dl);

  Alkeys.resize(areas.Size(), minage, lowerlgrp, size);
  tagAlkeys.resize(areas.Size(), minage, lowerlgrp, size);
  for (i = 0; i < Alkeys.Size(); i++)
    Alkeys[i].SettoZero();

  //Read the growth length group data
  doublevector grlengths;
  charptrvector grlenindex;
  char aggfilename[MaxStrLength];
  strncpy(aggfilename, "", MaxStrLength);
  ifstream datafile;
  CommentStream subdata(datafile);

  ReadWordAndValue(infile, "growthandeatlengths", aggfilename);
  datafile.open(aggfilename);
  CheckIfFailure(datafile, aggfilename);
  handle.Open(aggfilename);
  i = ReadLengthAggregation(subdata, grlengths, grlenindex);
  handle.Close();
  datafile.close();
  datafile.clear();

  LengthGroupDivision* GrowLgrpDiv = new LengthGroupDivision(grlengths);
  if (GrowLgrpDiv->Error())
    LengthGroupPrintError(grlengths, keeper);

  CheckLengthGroupIsFiner(LgrpDiv, GrowLgrpDiv, this->Name(), "growth and eat lengths");

  //Read the growth function data
  ReadWordAndVariable(infile, "doesgrow", doesgrow);
  if (doesgrow)
    grower = new Grower(infile, LgrpDiv, GrowLgrpDiv, areas, TimeInfo, keeper, refweight, Area, grlenindex);
  else
    grower = 0;

  //Read the natural mortality data
  infile >> text;
  if (strcasecmp(text, "naturalmortality") == 0)
    NatM = new NaturalM(infile, minage, maxage, TimeInfo, keeper);
  else
    handle.Unexpected("naturalmortality", text);

  //Read the prey data
  infile >> text;
  if (strcasecmp(text, "iscaught") == 0)
    infile >> iscaught >> ws >> text;
  else
    iscaught = 0;

  if (strcasecmp(text, "iseaten") == 0)
    infile >> iseaten >> ws;
  else
    handle.Unexpected("iseaten", text);

  if (iseaten)
    prey = new StockPrey(infile, areas, this->Name(), minage, maxage, keeper);
  else
    prey = 0;

  //Read the predator data
  ReadWordAndVariable(infile, "doeseat", doeseat);
  if (doeseat)
    predator = new StockPredator(infile, this->Name(), areas, LgrpDiv,
      GrowLgrpDiv, minage, maxage, TimeInfo, keeper);
  else
    predator = 0;

  //Read the initial conditions
  infile >> text;
  if (strcasecmp(text, "initialconditions") == 0)
    initial = new InitialCond(infile, areas, keeper, refweight, Area);
  else
    handle.Unexpected("initialconditions", text);

  //Read the migration data
  ReadWordAndVariable(infile, "doesmigrate", doesmigrate);
  if (doesmigrate) {
    ReadWordAndVariable(infile, "agedependentmigration", AgeDepMigration);
    infile >> text;
    if (strcasecmp(text, "migrationfile") == 0) {
      infile >> text;
      ifstream subfile(text);
      CommentStream subcomment(subfile);
      CheckIfFailure(subfile, text);
      handle.Open(text);
      migration = new Migration(subcomment, AgeDepMigration, areas, Area, TimeInfo, keeper);
      handle.Close();
      subfile.close();
      subfile.clear();
    } else
      handle.Unexpected("migrationfile", text);
  }

  //Read the maturation data
  ReadWordAndVariable(infile, "doesmature", doesmature);
  if (doesmature) {
    ReadWordAndVariable(infile, "maturitytype", tmpint);
    infile >> text >> ws;
    if (strcasecmp(text, "maturityfile") == 0) {
      infile >> text >> ws;
      ifstream subfile(text);
      CommentStream subcomment(subfile);
      CheckIfFailure(subfile, text);
      handle.Open(text);

      switch (tmpint) {
        case 1:
          maturity = new MaturityA(subcomment, TimeInfo, keeper,
            minage, lowerlgrp, size, areas, LgrpDiv, 3);
          break;
        case 2:
          maturity = new MaturityB(subcomment, TimeInfo, keeper,
            minage, lowerlgrp, size, areas, LgrpDiv);
          break;
        case 3:
          maturity = new MaturityC(subcomment, TimeInfo, keeper,
            minage, lowerlgrp, size, areas, LgrpDiv, 4);
          break;
        case 4:
          maturity = new MaturityD(subcomment, TimeInfo, keeper,
            minage, lowerlgrp, size, areas, LgrpDiv, 4);
          break;
        default:
          handle.Message("Maturity type is expected to be 1, 2, 3 or 4");
      }

      handle.Close();
      subfile.close();
      subfile.clear();
    } else
      handle.Unexpected("maturityfile", text);

    if (!doesgrow)
      handle.Warning("The stock does not grow, so it is unlikely to mature!");
  }

  //Read the movement data
  ReadWordAndVariable(infile, "doesmove", doesmove);
  if (doesmove)
    //transition handles the movements of the age group maxage:
    transition = new Transition(infile, areas, maxage,
      lowerlgrp[maxage - minage], size[maxage - minage], keeper);

  //Read the renewal data
  ReadWordAndVariable(infile, "doesrenew", doesrenew);
  if (doesrenew) {
    infile >> text;
    if (strcasecmp(text, "renewaldatafile") == 0) {
      infile >> text;
      ifstream subfile(text);
      CommentStream subcomment(subfile);
      CheckIfFailure(subfile, text);
      handle.Open(text);
      renewal = new RenewalData(subcomment, areas, Area, TimeInfo, keeper);
      handle.Close();
      subfile.close();
      subfile.clear();
    } else
      handle.Unexpected("renewaldatafile", text);
  }

  //Read the spawning data
  ReadWordAndVariable(infile, "doesspawn", doesspawn);
  if (doesspawn) {
    infile >> text;
    if (strcasecmp(text, "spawnfile") == 0) {
      infile >> text;
      ifstream subfile(text);
      CommentStream subcomment(subfile);
      CheckIfFailure(subfile, text);
      handle.Open(text);
      spawner = new Spawner(subcomment, minage, maxage, Area, TimeInfo, keeper);
      handle.Close();
      subfile.close();
      subfile.clear();
    } else
      handle.Unexpected("spawnfile", text);
  }

  //Set the birthday for the stock
  birthdate = TimeInfo->StepsInYear();

  //Finished reading from infile - resize objects and clean up
  NumberInArea.AddRows(areas.Size(), LgrpDiv->NoLengthGroups());
  delete GrowLgrpDiv;
  keeper->ClearAll();
}

Stock::Stock(const char* givenname)
  : BaseClass(givenname), spawner(0), catchptr(0), renewal(0), maturity(0),
  transition(0), migration(0), prey(0), predator(0), initial(0),
  LgrpDiv(0), grower(0), NatM(0) {
  //dummy constructor for use in derived classes
}

Stock::~Stock() {
  //Of course all the pointers were initialized to 0 in the constructor
  //and some of them set to point to real objects after that.
  delete migration;
  delete prey;
  delete predator;
  delete initial;
  delete LgrpDiv;
  delete grower;
  delete NatM;
  delete transition;
  delete renewal;
  delete maturity;
  delete spawner;
  delete catchptr;
}

void Stock::Reset(const TimeClass* const TimeInfo) {
  //What is only done in the beginning.
  //Naturalm, maturity and growth will later be moved.

  if (TimeInfo->CurrentTime() == 1) {
    this->Clear();
    initial->Initialize(Alkeys);
    if (iseaten)
      prey->Reset();
    if (doesmature)
      maturity->Precalc(TimeInfo);
    if (doesrenew)
      renewal->Reset();
    if (doesgrow)
      grower->Reset();
    if (doesmigrate)
      migration->Reset();
  }
  NatM->Reset(TimeInfo);
  if (doeseat)
    predator->Reset(TimeInfo);
}

void Stock::Clear() {
  BaseClass::Clear();
  if (doesmigrate)
    migration->Clear();
}

Prey* Stock::ReturnPrey() const {
  return prey;
}

PopPredator* Stock::ReturnPredator() const {
  return predator;
}

void Stock::SetStock(Stockptrvector& stockvec) {
  if (doesmature)
    maturity->SetStock(stockvec);
  if (doesmove)
    transition->SetStock(stockvec);
}

void Stock::SetCatch(CatchDataptrvector& CDvector) {
  if (!iscaught)
    return;
  int i, found = 0;
  CatchData* CD = 0;
  for (i = 0; i < CDvector.Size(); i++)
    if (CDvector[i]->FindStock(this)) {
      found += 1;
      CD = CDvector[i];
      CD->AddStock(*this);
    }

  if (found != 1) {
    cerr << "Error in the stock " << this->Name()
      << "\nIt expected to find 1 catch data, but found " << found << endl;
    exit(EXIT_FAILURE);
  }
  catchptr = new Catch(CD);
}

void Stock::SetCI() {
  initial->SetCI(LgrpDiv);
  if (iscaught)
    catchptr->SetCI(LgrpDiv);
  if (iseaten)
    prey->SetCI(LgrpDiv);
  if (doesmove)
    transition->SetCI(LgrpDiv);
  if (doesrenew)
    renewal->SetCI(LgrpDiv);
}

void Stock::Print(ofstream& outfile) const {
  int i;

  outfile << "\nStock\nName" << sep << this->Name() << "\niseaten" << sep
    << iseaten << "\ndoeseat" << sep << doeseat << "\ndoesmove" << sep
    << doesmove << "\ndoesspawn" << sep << doesspawn << "\ndoesmature" << sep
    << doesmature << "\ndoesrenew" << sep << doesrenew << "\ndoesgrow" << sep
    << doesgrow << "\ndoesmigrate" << sep << doesmigrate << "\nInner areas";

  for (i = 0; i < areas.Size(); i++)
    outfile << sep << areas[i];
  outfile << endl;

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

  outfile << "\nAgelength keys\n\nCurrent status\n";
  for (i = 0; i < areas.Size(); i++) {
    outfile << "Inner Area " << areas[i] << "\nNumbers\n";
    Printagebandm(outfile, Alkeys[i]);
    outfile << "Mean weights\n";
    PrintWeightinagebandm(outfile, Alkeys[i]);
  }
}

int Stock::Birthday(const TimeClass* const TimeInfo) const {
  return(TimeInfo->CurrentStep() == birthdate);
}

const Agebandmatrix& Stock::Agelengthkeys(int area) const {
  return Alkeys[AreaNr[area]];
}

Agebandmatrix& Stock::MutableAgelengthkeys(int area) const {
  return((Agebandmatrix &) Alkeys[AreaNr[area]]);
}

const Agebandmatrix& Stock::getMeanN(int area) const {
  assert(iseaten && prey->preyType() == MORTPREY_TYPE);
  return(((MortPrey*)prey)->getMeanN(area));
}

const Stockptrvector& Stock::GetMatureStocks() {
  assert(doesmature != 0);
  return maturity->GetMatureStocks();
}
