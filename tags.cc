#include "tags.h"
#include "areatime.h"
#include "errorhandler.h"
#include "readfunc.h"
#include "readword.h"
#include "stock.h"
#include "gadget.h"

Tags::Tags(CommentStream& infile, const char* givenname, const AreaClass* const Area,
  const TimeClass* const TimeInfo, Keeper* const keeper) : HasName(givenname) {

  LgrpDiv = NULL;
  ErrorHandler handle;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  ifstream subfile;
  CommentStream subcomment(subfile);

  keeper->AddString("tags");
  keeper->AddString(givenname);

  //Currently can only have one stock per tagging experiment
  ReadWordAndValue(infile, "stock", text);
  stocknames.resize(1);
  stocknames[0] = new char[strlen(text) + 1];
  strcpy(stocknames[0], text);

  int tmparea;
  //Currently can only have one area per tagging experiment
  ReadWordAndVariable(infile, "tagarea", tmparea);
  if ((tagarea = Area->InnerArea(tmparea)) == -1)
    handle.UndefinedArea(tmparea);

  ReadWordAndVariable(infile, "tagyear", tagyear);
  ReadWordAndVariable(infile, "tagstep", tagstep);
  ReadWordAndVariable(infile, "endyear", endyear);
  ReadWordAndVariable(infile, "endstep", endstep);

  if (!TimeInfo->IsWithinPeriod(tagyear, tagstep))
    handle.Message("Time for start of tagging experiment is not within given time for the model");

  if (!TimeInfo->IsWithinPeriod(endyear, endstep))
    handle.Message("Time for end of tagging experiment is not within given time for the model");

  if ((tagyear > endyear) || (tagyear == endyear && tagstep > endstep))
    handle.Message("Error in reading time data for tagging experiment");

  //Now read in the length information
  double minlength, maxlength, dl;
  ReadWordAndVariable(infile, "minlength", minlength);
  ReadWordAndVariable(infile, "maxlength", maxlength);
  ReadWordAndVariable(infile, "dl", dl);

  LgrpDiv = new LengthGroupDivision(minlength, maxlength, dl);
  if (LgrpDiv->Error())
    LengthGroupPrintError(minlength, maxlength, dl, "length groups for tags");
  NumberByLength.resize(LgrpDiv->NoLengthGroups(), 0.0);

  //Now read in the tagloss information - not currently used
  infile >> text >> ws;
  if (strcasecmp(text, "tagloss") == 0)
    infile >> tagloss >> ws;
  else
    handle.Unexpected("tagloss", text);

  //Read in the numbers format: tagid - length - number
  ReadWordAndValue(infile, "numbers", text);
  subfile.open(text);
  CheckIfFailure(subfile, text);
  handle.Open(text);
  ReadNumbers(subcomment, givenname, minlength, dl);
  handle.Close();
  subfile.close();
  subfile.clear();
  keeper->ClearLast();
  keeper->ClearLast();
}

void Tags::ReadNumbers(CommentStream& infile, const char* tagname, double minlength, double dl) {
  int lenid, keepdata;
  double tmplength, tmpnumber;
  char tmpname[MaxStrLength];
  strncpy(tmpname, "", MaxStrLength);
  ErrorHandler handle;

  infile >> ws;
  //Check the number of columns in the inputfile
  if (CountColumns(infile) != 3)
    handle.Message("Wrong number of columns in inputfile - should be 3");

  while (!infile.eof()) {
    keepdata = 0;
    infile >> tmpname >> tmplength >> tmpnumber >> ws;

    //only keep the data if tmpname matches tagname
    if (!(strcasecmp(tagname, tmpname) == 0))
      keepdata = 1;

    //only keep the data if 0 <= lenid < number of length groups.
    lenid = int((tmplength - minlength)/dl);
    if (lenid < 0 || lenid >= NumberByLength.Size())
      keepdata = 1;

    //only keep the data if the number is positive
    if (tmpnumber < 0) {
      cerr << "Found a negative number for the tagging experiment - number set to zero\n";
      keepdata = 1;
    }

    if (keepdata == 0) {
      //numbers data is required, so store it
      NumberByLength[lenid] = tmpnumber;
    }
  }
}

Tags::~Tags() {
  int i;
  for (i = 0; i < stocknames.Size(); i++)
    delete [] stocknames[i];
  if (LgrpDiv != NULL)
    delete LgrpDiv;
  for (i = 0; i < AgeLengthStock.Size(); i++)
    delete AgeLengthStock[i];
}

void Tags::SetStock(Stockptrvector& Stocks) {
  int i, j, found, temparea, numareas, numMatStock;
  Stockptrvector tempMatureStock;

  for (i = 0; i < stocknames.Size(); i++) {
    j = 0;
    found = 0;
    while (found == 0 && j < Stocks.Size()) {
      if (strcasecmp(Stocks[j]->Name(), stocknames[i]) == 0) {
        tagstocks.resize(1,Stocks[j]);
        found++;
      }
      j++;
    }
    if (found == 0) {
      cerr << "Error when searching for names of stocks for tags\n"
        << "Did not find any name matching " << stocknames[i] << endl;
      exit(EXIT_FAILURE);
    }
  }

  //Make sure that given stocks are defined on tagarea and have same
  //lengthgroup division for the tagging experiment and the stock
  const LengthGroupDivision* tempLgrpDiv;
  for (i = 0; i < tagstocks.Size(); i++) {
    temparea = tagstocks[i]->IsInArea(tagarea);
    if (temparea == 0) {
      cerr << "Error in tagging experiment - stock " << tagstocks[i]->Name()
        << " does not live on area: " << tagarea << "\nthe stock lives on areas: ";
      const intvector* tempareas = &tagstocks[i]->Areas();
      for (numareas= 0; numareas < tempareas->Size(); numareas++)
        cerr << (*tempareas)[numareas] << sep;
      cerr << endl;
      exit(EXIT_FAILURE);
    }

    tempLgrpDiv = tagstocks[i]->ReturnLengthGroupDiv();
    if (LgrpDiv->NoLengthGroups() != tempLgrpDiv->NoLengthGroups()) {
      cerr << "Error in tagging experiment - invalid length groups\n";
      exit(EXIT_FAILURE);
    }
    if(LgrpDiv->dl() != tempLgrpDiv->dl()) {
      cerr << "Error in tagging experiment - invalid length groups\n";
      exit(EXIT_FAILURE);
    }
    if (LgrpDiv->Minlength(0) != tempLgrpDiv->Minlength(0)) {
      cerr << "Error in tagging experiment - invalid length groups\n";
      exit(EXIT_FAILURE);
    }
  }

  for (i = 0; i < tagstocks.Size(); i++) {
    if (tagstocks[i]->DoesMature()) {
      tempMatureStock = tagstocks[i]->GetMatureStocks();
      for (numMatStock = 0; numMatStock < tempMatureStock.Size(); numMatStock++)
        maturestocks.resize(1, tempMatureStock[numMatStock]);
    }
  }
}

//Must have set stocks according to stocknames using SetStock before calling Update()
//Now we need to distribute the tagged fish to the same age/length groups as the tagged stock.
void Tags::Update() {
  Agebandmatrixvector* tagpopulation;
  int i;
  popinfovector NumberInArea;
  NumberInArea.resize(LgrpDiv->NoLengthGroups());
  popinfo nullpop;
  for (i = 0; i < NumberInArea.Size(); i++)
    NumberInArea[i] = nullpop;

  //This only works if have only one stock...
  //probably best to use aggregator if have more than one stock.
  const Agebandmatrix* stockPopInArea;
  stockPopInArea = &(tagstocks[0]->Agelengthkeys(tagarea));
  stockPopInArea->Colsum(NumberInArea);

  //Now I have total number of stock per length in tagarea, N(., l) (NumberInArea) and
  //number of stock per age/length, N(a, l) (stockPopInArea) so we must initialize
  //AgeLengthStock so that it can hold all information of number of tagged stock
  //per area/age/length after endtime. We must make agebandmatrixvector same size as
  //the one in stock even though have only one area entry at the beginning
  intvector stockareas = tagstocks[0]->Areas();
  int j = 0;
  int tagareaindex = -1;
  while (j < stockareas.Size() && tagareaindex == -1) {
    if (tagarea == stockareas[j])
      tagareaindex = j;
    j++;
  }
  if (j == stockareas.Size() && tagareaindex == -1) {
    cerr << "Could not find area of tagging in stock area while updating stock: " << this->Name() << endl;
    exit(EXIT_FAILURE);
  }

  int numAreas = stockareas.Size();
  int maxage = stockPopInArea->Maxage();
  int minage = stockPopInArea->Minage();
  int numberofagegroups = maxage - minage + 1;
  int upperlgrp, minl, maxl, age, length;
  intvector sizeoflengthgroups(numberofagegroups);
  intvector lowerlengthgroups(numberofagegroups);

  for (i = 0; i < numberofagegroups; i++) {
    lowerlengthgroups[i]= stockPopInArea->Minlength(i + minage);
    upperlgrp = stockPopInArea->Maxlength(i + minage);
    sizeoflengthgroups[i] = upperlgrp - lowerlengthgroups[i];
  }

  tagpopulation= new Agebandmatrixvector(stockareas.Size(), minage, lowerlengthgroups, sizeoflengthgroups);
  AgeLengthStock.resize(1, tagpopulation);
  for (age = stockPopInArea->Minage(); age <= stockPopInArea->Maxage(); age++) {
    minl = stockPopInArea->Minlength(age);
    maxl = stockPopInArea->Maxlength(age);
    for (length = minl; length < maxl; length++) {
      if (!(NumberInArea[length].N > 0))
        (*AgeLengthStock[0])[tagareaindex][age][length].N = 0.0;
       else
        (*AgeLengthStock[0])[tagareaindex][age][length].N =
          (NumberByLength[length - minl] * (*stockPopInArea)[age][length].N) / (NumberInArea[length].N);
    }
  }
  tagstocks[0]->UpdateTags(AgeLengthStock[0], this);
  //Have not set meanweight in AgeLengthStock. W == 0.

  const Agebandmatrix* matureStockPopInArea;
  for (i = 0; i < maturestocks.Size(); i++) {
    matureStockPopInArea = &maturestocks[i]->Agelengthkeys(tagarea);
    stockareas = maturestocks[i]->Areas();
    numAreas = stockareas.Size();
    maxage = matureStockPopInArea->Maxage();
    minage = matureStockPopInArea->Minage();
    numberofagegroups = maxage - minage + 1;
    while (sizeoflengthgroups.Size() > 0) {
      sizeoflengthgroups.Delete(0);
      lowerlengthgroups.Delete(0);
    }
    sizeoflengthgroups.resize(numberofagegroups);
    lowerlengthgroups.resize(numberofagegroups);
    for (i = 0; i < numberofagegroups; i++) {
      lowerlengthgroups[i] = matureStockPopInArea->Minlength(i + minage);
      upperlgrp = matureStockPopInArea->Maxlength(i + minage);
      sizeoflengthgroups[i] = upperlgrp - lowerlengthgroups[i];
    }
    tagpopulation = new Agebandmatrixvector(stockareas.Size(), minage, lowerlengthgroups, sizeoflengthgroups);
    AgeLengthStock.resize(1, tagpopulation);
  }
}

void Tags::DeleteFromStock() {
  int i;
  for (i = 0; i < tagstocks.Size(); i++)
    tagstocks[i]->DeleteTags(this->Name());
  for (i = 0; i < maturestocks.Size(); i++)
    maturestocks[i]->DeleteTags(this->Name());
}

void Tags::printPopInfo(char* filename) {

  ofstream outfile;
  outfile.open(filename);
  if (!outfile) {
    cerr << "Error - can't open outputfile for printing tagging information\n";
    exit(EXIT_FAILURE);
  }

  int age, length, areas, numareas;
  int minage, maxage, minlength, maxlength;
  if (AgeLengthStock.Size() == 0) {
    numareas = -1;
    minage = -1;
    maxage = -1;
  } else {
    numareas = AgeLengthStock[0]->Size();
    minage = (*AgeLengthStock[0])[0].Minage();
    maxage = (*AgeLengthStock[0])[0].Maxage();
  }

  outfile << "Tagging numbers\n";
  for (areas = 0; areas < numareas; areas++) {
    for (age = minage; age <= maxage; age++) {
      minlength = (*AgeLengthStock[0])[areas].Minlength(age);
      maxlength = (*AgeLengthStock[0])[areas].Maxlength(age);
      for (length = minlength; length < maxlength; length++)
        outfile << (*AgeLengthStock[0])[areas][age][length].N << sep;
      outfile << endl;
    }
  }
}

void Tags::UpdateMatureStock(const TimeClass* const TimeInfo) {

  int i;
  int currentYear = TimeInfo->CurrentYear();
  int currentStep = TimeInfo->CurrentStep();

  if (endyear < currentYear || (endyear == currentYear && endstep < currentStep))
    cerr << "Warning - tagging experiment is not part of the simulation anymore\n";
  else
    for (i = 0; i < maturestocks.Size(); i++)
      maturestocks[i]->UpdateTags(AgeLengthStock[i + 1], this);
}
