/* Changes done and left to do
0. Using class HasName for the name of the tag not char* name just to conform to other code.
0.5 Added reading of minlength, maxlength and dl even not yet clear is this should be done but seems likely
0.75 Not reading in the actual lengths, cm of the length groups. Do not think it is needed here?
1. removed aggregator as only using one stock. Add if allow more than one stock later.
2. Not using stockptrvector as parameter to constructor. This might be a good idea but means that must have read stocks before tagging info. Instead use Setstock to set the stock tags uses and check if this stock is OK for Tags.
3. Meanweight is not read in and not set anywhere yet, could set them in SetStock ...

3. need to change to use errorhandler not exit...
4. The code makes the agebandmatrixvector for the result of the tagging population exacly the same size as the stock even though the tagged fish might only be in one area and the stock be defined in more than one area.
5. Not using livesoneareas. Migth be a good idea, depending on maybe output from this class. But at the moment the class only knows that it lives on tagarea. But info of areas can be got from tagstocks and maturestocks.
6. Have not run with mature stock so code for updating mature stock is not tested.

100. popratio, can give it value from other popratio but must have initialized it first with an address from popinfo? This is the plan at the moment.
Must check how to pass the popinfo, no problem just do
N = a.&N. Think about using const so that can not change address..
Have not tested with mature stock, can use norwegian dataset..
*/

#include "tags.h"
#include "areatime.h"
#include "errorhandler.h"
#include "readfunc.h"
#include "readword.h"
#include "stock.h"
#include "gadget.h"

Tags::Tags(CommentStream& infile, const char* givenname, const AreaClass* const Area,
   const TimeClass* const TimeInfo, Keeper* const keeper) : HasName(givenname) {

  ErrorHandler handle;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  ifstream subfile;
  CommentStream subcomment(subfile);

  keeper->AddString("tags");
  keeper->AddString(givenname);

  //Currently can only have one stock
  ReadWordAndValue(infile, "stock", text);
  stocknames.resize(1);
  stocknames[0] = new char[strlen(text) + 1];
  strcpy(stocknames[0], text);

  int tmparea;
  //Currently can only have one area
  ReadWordAndVariable(infile, "tagarea", tmparea);
  if ((tagarea = Area->InnerArea(tmparea)) == -1)
    handle.UndefinedArea(tmparea);

  //ReadWordAndVariable(infile, "tagloss", tagloss);
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
    LengthGroupPrintError(minlength, maxlength, dl, keeper);

  NumberByLength.resize(LgrpDiv->Size(), 0.0);

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
  ReadNumbers(subcomment, givenname, minlength);
  handle.Close();
  subfile.close();
  subfile.clear();
  keeper->ClearLast();
  keeper->ClearLast();
}

void Tags::ReadNumbers(CommentStream& infile, const char* tagname, double minlength) {
  int lenid, keepdata;
  double tmplength, tmpnumber;
  char tmpname[MaxStrLength];
  strncpy(tmpname, "", MaxStrLength);
  ErrorHandler handle;

  infile >> ws;
  //Check the number of columns in the inputfile
  if (!(CheckColumns(infile, 3)))
    handle.Message("Wrong number of columns in inputfile - should be 3");

  while (!infile.eof()) {
    keepdata = 0;
    infile >> tmpname >> tmplength >> tmpnumber >> ws;

    //only keep the data if tmpname matches tagname
    if (!(strcasecmp(tagname, tmpname) == 0))
      keepdata = 1;

    lenid = (int)(tmplength - minlength - 1);
    if (lenid < 0)
      keepdata = 1;

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
    delete[] stocknames[i];

  if (LgrpDiv != NULL) {
    delete LgrpDiv;
  }

  for (i = 0; i < AgeLengthStock.Size(); i++)
    delete[] AgeLengthStock[i];
}

void Tags::SetStock(Stockptrvector& Stocks) {
  assert(Stocks.Size() > 0);
  assert(stocknames.Size() > 0);

  int i, j, found;
  int temparea, numareas;

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
      cerr << "Error when searching for names of stocks for Tags\n"
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
      cerr << "Error - tagging experiment for stock: " << tagstocks[i]->Name() << " does not live on area: " << tagarea << endl;
      cout << "The stock lives on areas: ";
      const intvector* tempareas = &tagstocks[i]->Areas();
      for (numareas = 0; numareas < tempareas->Size(); numareas++)
        cout << (*tempareas)[numareas] << sep;

      cout << endl;
      exit(EXIT_FAILURE);
    }

    tempLgrpDiv = tagstocks[i]->ReturnLengthGroupDiv();
    if (LgrpDiv->Size() != tempLgrpDiv->Size()) {
      cerr << "tag and stock do not have same number of length groups\n";
      exit(EXIT_FAILURE);
    }

    if(LgrpDiv->dl() != tempLgrpDiv->dl()) {
      cerr << "tag and stock do not have same dl between lengthgroups\n";
      exit(EXIT_FAILURE);
    }

    if (LgrpDiv->Minlength(0) != tempLgrpDiv->Minlength(0)) {
      cerr << "tag and stock do not have the same length group division\n";
      exit(EXIT_FAILURE);
    }
  } // have the same minlength, dl and size so should be the same length group division.

  // Initialize taggedStock agebandmatrixvector...
  // needs to be done...
  // Think about mature stock .....
  Stockptrvector tempMatureStock;
  int numMatStock;
  for (i = 0; i < tagstocks.Size(); i++) {
    if (tagstocks[i]->DoesMature()) {
      tempMatureStock = tagstocks[i]->GetMatureStocks();
      for (numMatStock = 0; numMatStock < tempMatureStock.Size(); numMatStock++) {
        maturestocks.resize(1, tempMatureStock[numMatStock]);
      }
    }
  }

  // maturestock could have a different lengtgroup division, should be OK??
  // If have more than one stock then have to delete entries in mature stock could have multiple entries.
  // Check for multiple entries.
  for (i = 0; i < tagstocks.Size(); i++) {
    for (j = 0; j < maturestocks.Size(); j ++) {
      if (strcasecmp(tagstocks[i]->Name(), maturestocks[j]->Name()) == 0) {
        maturestocks.Delete(j);
      }
    }
  }
}

void Tags::Update() {
  //Must have set stocks according to stocknames using SetStock before calling Update()
  //Now we need to distribute the tagged fish to the same age/length groups as the tagged stock.
  Agebandmatrixvector* tagpopulation;
  int i, j, age, length;
  assert(tagstocks.Size() > 0);
  assert(tagstocks.Size() == stocknames.Size());

  popinfovector NumberInArea;
  NumberInArea.resize(LgrpDiv->Size());
  popinfo nullpop;
  for (i = 0; i < NumberInArea.Size(); i++)
     NumberInArea[i] = nullpop;

  // This only works if have only one stock... probably best to use aggregator if have more than one stock.
  const Agebandmatrix* stockPopInArea;
  stockPopInArea = &(tagstocks[0]->Agelengthkeys(tagarea));
  stockPopInArea->Colsum(NumberInArea);

  // Now I have total number of stock per length in tagarea, N(., l) (NumberInArea) and
  // number of stock per age/length, N(a, l) (stockPopInArea)
  // Now must initialize AgeLengthStock so that it can hold all information of number of
  // tagged stock per area/age/length after endtime.
  // Must make agebandmatrixvector same size as the one in stock even though have only
  // one area entry at the beginning?

  intvector stockareas = tagstocks[0]->Areas();
  j = 0;
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
  int upperlgrp, minl, maxl;
  intvector sizeoflengthgroups(numberofagegroups);
  intvector lowerlengthgroups(numberofagegroups);

  for (i = 0; i < numberofagegroups; i++) {
    lowerlengthgroups[i]= stockPopInArea->Minlength(i+minage);
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
        (*AgeLengthStock[0])[tagareaindex][age][length].N = (NumberByLength[length - minl] * (*stockPopInArea)[age][length].N) / (NumberInArea[length].N);
    }
  }
  tagstocks[0]->UpdateTags(AgeLengthStock[0], this->Name());

  // Have not set meanweight in AgeLengthStock. W == 0.
  // Adding a nullpop agebandmatrixvector to maturestock.
  // This might not be the best idea, maybe better to let stock add this at the correct time.
  const Agebandmatrix* matureStockPopInArea;
  for (i = 0; i < maturestocks.Size(); i++)  {
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
      lowerlengthgroups[i]= matureStockPopInArea->Minlength(i + minage);
      upperlgrp = matureStockPopInArea->Maxlength(i + maxage);
      sizeoflengthgroups[i] = upperlgrp - lowerlengthgroups[i];
    }
    tagpopulation= new Agebandmatrixvector(stockareas.Size(), minage, lowerlengthgroups, sizeoflengthgroups);
    AgeLengthStock.resize(1, tagpopulation);
    maturestocks[i]->UpdateTags(AgeLengthStock[AgeLengthStock.Size() - 1], this->Name());
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
  if (!outfile)
    exit(EXIT_FAILURE);

  int age, length, areas, numareas;
  int minage, maxage;
  int minlength, maxlength;

  if (AgeLengthStock.Size() == 0) {
    numareas = -1;
    minage = -1;
    maxage = -1;
  } else {
    numareas = AgeLengthStock[0]->Size();
    minage = (*AgeLengthStock[0])[0].Minage();
    maxage = (*AgeLengthStock[0])[0].Maxage();
  }

  for (areas = 0; areas < numareas; areas++) {
    for (age = minage; age <= maxage; age++) {
      minlength = (*AgeLengthStock[0])[areas].Minlength(age);
      maxlength = (*AgeLengthStock[0])[areas].Maxlength(age);

      for (length = minlength; length < maxlength; length++) {
        outfile << (*AgeLengthStock[0])[areas][age][length].N << sep;
      }
      outfile << endl;
    }
  }
}
