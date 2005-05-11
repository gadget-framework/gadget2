#include "tags.h"
#include "areatime.h"
#include "errorhandler.h"
#include "readfunc.h"
#include "readword.h"
#include "stock.h"
#include "stockprey.h"
#include "gadget.h"

extern ErrorHandler handle;

Tags::Tags(CommentStream& infile, const char* givenname, const AreaClass* const Area,
  const TimeClass* const TimeInfo, Keeper* const keeper, StockPtrVector stockvec)
  : HasName(givenname) {

  taggingstock = 0;
  numtagtimesteps = 0;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  ifstream subfile;
  CommentStream subcomment(subfile);

  keeper->addString("tags");
  keeper->addString(givenname);

  //Currently can only have one stock per tagging experiment
  readWordAndValue(infile, "stock", text);
  stocknames.resize(1);
  stocknames[0] = new char[strlen(text) + 1];
  strcpy(stocknames[0], text);

  int tmparea;
  //Currently can only have one area per tagging experiment
  readWordAndVariable(infile, "tagarea", tmparea);
  tagarea = Area->InnerArea(tmparea);

  infile >> ws;
  char c = infile.peek();
  if ((c == 'e') || (c == 'E'))
    readWordAndVariable(infile, "endyear", endyear);
  else
    endyear = TimeInfo->getLastYear();

  int i = 0, found = 0;
  while (found == 0 && i < stockvec.Size()) {
    if (strcasecmp(stockvec[i]->getName(), stocknames[0]) == 0) {
      LgrpDiv = new LengthGroupDivision(*(stockvec[i]->returnLengthGroupDiv()));
      found++;
    }
    i++;
  }
  if (found == 0)
    handle.logMessage(LOGFAIL, "Error in tags - failed to match stock", stocknames[0]);

  //Now read in the tagloss information
  readWordAndFormula(infile, "tagloss", tagloss);
  tagloss.Inform(keeper);

  //read in the numbers format: tagid - length - number
  readWordAndValue(infile, "numbers", text);
  subfile.open(text, ios::in);
  handle.checkIfFailure(subfile, text);
  handle.Open(text);
  readNumbers(subcomment, givenname, TimeInfo);
  handle.Close();
  subfile.close();
  subfile.clear();
  keeper->clearLast();
  keeper->clearLast();
}

void Tags::readNumbers(CommentStream& infile, const char* tagname, const TimeClass* const TimeInfo) {

  int year, step;
  int i, lenid, keepdata, timeid;
  double tmplength, tmpnumber;
  char tmpname[MaxStrLength];
  strncpy(tmpname, "", MaxStrLength);
  int count = 0;

  infile >> ws;
  //Check the number of columns in the inputfile
  if (countColumns(infile) != 5)
    handle.logFileMessage(LOGFAIL, "Wrong number of columns in inputfile - should be 5");

  while (!infile.eof()) {
    keepdata = 0;
    infile >> tmpname >> year >> step >> tmplength >> tmpnumber >> ws;

    //only keep the data if tmpname matches tagname
    if (!(strcasecmp(tagname, tmpname) == 0))
      keepdata = 1;

    //only keep the data if the length is valid
    lenid = -1;
    for (i = 0; i < LgrpDiv->numLengthGroups(); i++)
      if (tmplength == LgrpDiv->minLength(i))
        lenid = i;

    if (lenid == -1)
      keepdata = 1;

    //only keep the data if the number is positive
    if (tmpnumber < 0) {
      handle.logMessage(LOGWARN, "Warning in tags - found negative number of tags", tmpnumber);
      keepdata = 1;
    }

    timeid = -1;
    if ((TimeInfo->isWithinPeriod(year, step)) && (keepdata == 0)) {
      for (i = 0; i < Years.Size(); i++)
        if ((Years[i] == year) && (Steps[i] == step))
          timeid = i;

      if (timeid == -1) {
        Years.resize(1, year);
        Steps.resize(1, step);
        timeid = Years.Size() - 1;
        NumberByLength.resize(1, new DoubleMatrix(1, LgrpDiv->numLengthGroups(), 0.0));
      }

    } else
      keepdata = 1;

    if (keepdata == 0) {
      count++;
      (*NumberByLength[timeid])[0][lenid] = tmpnumber;
    }
  }

  if ((handle.getLogLevel() >= LOGWARN) && (count == 0))
    handle.logMessage(LOGWARN, "Warning in tags - found no data in the data file for", tagname);
  if (handle.getLogLevel() >= LOGMESSAGE)
    handle.logMessage(LOGMESSAGE, "Read tags data file - number of entries", count);

  tagyear = 9999;
  tagstep = 9999;
  for (i = 0; i < Years.Size(); i++)
    if ((Years[i] < tagyear) || (Years[i] == tagyear && Steps[i] < tagstep)) {
      tagyear = Years[i];
      tagstep = Steps[i];
    }

  timeid = -1;
  for (i = 0; i < Years.Size(); i++)
    if ((Years[i] == tagyear) && (Steps[i] == tagstep))
      timeid = i;

  if (timeid == -1)
    handle.logMessage(LOGFAIL, "Error in tags - calculated invalid timestep");

  numtagtimesteps = (TimeInfo->numSteps() * (endyear - tagyear)) - tagstep + 1;
  if (endyear == TimeInfo->getLastYear())
    numtagtimesteps += TimeInfo->getLastStep();
  else
    numtagtimesteps += TimeInfo->numSteps();
}

Tags::~Tags() {
  int i;
  for (i = 0; i < stocknames.Size(); i++)
    delete[] stocknames[i];
  for (i = 0; i < NumberByLength.Size(); i++)
    delete NumberByLength[i];
  for (i = 0; i < CI.Size(); i++)
    delete CI[i];
  while (AgeLengthStock.Size() > 0)
    AgeLengthStock.Delete(0);
  while (NumBeforeEating.Size() > 0)
    NumBeforeEating.Delete(0);
  delete LgrpDiv;
}

void Tags::Reset(const TimeClass* const TimeInfo) {
  int i;
  if (TimeInfo->getTime() == 1) {
    while (AgeLengthStock.Size() > 0)
      AgeLengthStock.Delete(0);
    while (NumBeforeEating.Size() > 0)
      NumBeforeEating.Delete(0);
    while (CI.Size() > 0) {
      delete CI[0];
      CI.Delete(0);
    }
    for (i = 0; i < preyindex.Size(); i++)
      preyindex[i] = -1;
    for (i = 0; i < updated.Size(); i++)
      updated[i] = 0;
  }
}

void Tags::setStock(StockPtrVector& Stocks) {
  int i, j, found;
  StockPtrVector tempMatureStock, tempTransitionStock, tempStrayStock;
  char* stockname;

  j = 0;
  found = 0;
  while (found == 0 && j < Stocks.Size()) {
    if (strcasecmp(Stocks[j]->getName(), stocknames[0]) == 0) {
      tagStocks.resize(1, Stocks[j]);
      taggingstock = Stocks[j];
      found++;
    }
    j++;
  }
  if (found == 0)
    handle.logMessage(LOGFAIL, "Error in tags - failed to match stock", stocknames[0]);

  //Make sure that given stocks are defined on tagarea and have same
  //lengthgroup division for the tagging experiment and the stock
  if (!(taggingstock->isInArea(tagarea)))
    handle.logMessage(LOGFAIL, "Error in tags - stock isnt defined on tagging area");

  const LengthGroupDivision* tempLgrpDiv = taggingstock->returnLengthGroupDiv();
  if (LgrpDiv->numLengthGroups() != tempLgrpDiv->numLengthGroups())
    handle.logMessage(LOGFAIL, "Error in tags - invalid length group for tagged stock");
  if (LgrpDiv->dl() != tempLgrpDiv->dl())
    handle.logMessage(LOGFAIL, "Error in tags - invalid length group for tagged stock");
  if (!(isZero(LgrpDiv->minLength() - tempLgrpDiv->minLength())))
    handle.logMessage(LOGFAIL, "Error in tags - invalid length group for tagged stock");

  preyindex.resize(1, -1);
  updated.resize(1, 0);

  if (taggingstock->doesMove()) {
    tempTransitionStock = taggingstock->getTransitionStocks();
    for (i = 0; i < tempTransitionStock.Size(); i++) {
      transitionStocks.resize(1, tempTransitionStock[i]);
      preyindex.resize(1, -1);
      updated.resize(1, 0);
      tagStocks.resize(1, tempTransitionStock[i]);
    }
  }

  if (taggingstock->doesMature()) {
    tempMatureStock = taggingstock->getMatureStocks();
    for (i = 0; i < tempMatureStock.Size(); i++) {
      matureStocks.resize(1, tempMatureStock[i]);
      found = 0;
      for (j = 0; j < transitionStocks.Size(); j++)
        if (!(strcasecmp(transitionStocks[j]->getName(), tempMatureStock[i]->getName()) == 0))
          found++;

      if (found == 0) {
        preyindex.resize(1, -1);
        updated.resize(1, 0);
        tagStocks.resize(1, tempMatureStock[i]);
      }
    }
  }

  if (taggingstock->doesStray()) {
    tempStrayStock = taggingstock->getStrayStocks();
    for (i = 0; i < tempStrayStock.Size(); i++) {
      strayStocks.resize(1, tempStrayStock[i]);
      found = 0;
      for (j = 0; j < transitionStocks.Size(); j++)
        if (!(strcasecmp(transitionStocks[j]->getName(), tempStrayStock[i]->getName()) == 0))
          found++;

      for (j = 0; j < matureStocks.Size(); j++)
        if (!(strcasecmp(matureStocks[j]->getName(), tempStrayStock[i]->getName()) == 0))
          found++;

      if (found == 0) {
        preyindex.resize(1, -1);
        updated.resize(1, 0);
        tagStocks.resize(1, tempStrayStock[i]);
      }
    }
  }

  for (i = 1; i < tagStocks.Size(); i++) {
    stockname = new char[strlen(tagStocks[i]->getName()) + 1];
    strcpy(stockname, tagStocks[i]->getName());
    stocknames.resize(1, stockname);
  }
}

//Must have set stocks according to stocknames using setStock before calling Update()
//Now we need to distribute the tagged fish to the same age/length groups as the tagged stock.
void Tags::Update(int timeid) {
  int i, j;
  PopInfoVector NumberInArea;
  NumberInArea.resize(LgrpDiv->numLengthGroups());

  const AgeBandMatrix* stockPopInArea;
  const LengthGroupDivision* tmpLgrpDiv;

  stockPopInArea = &(taggingstock->getAgeLengthKeys(tagarea));
  stockPopInArea->sumColumns(NumberInArea);

  //Now we have total number of stock per length in tagarea, N(., l) (NumberInArea) and
  //number of stock per age/length, N(a, l) (stockPopInArea) so we must initialise
  //AgeLengthStock so that it can hold all information of number of tagged stock
  //per area/age/length after endtime. We must make AgeBandMatrixPtrVector same size as
  //the one in stock even though have only one area entry at the beginning
  IntVector stockareas = taggingstock->Areas();
  int numareas = stockareas.Size();
  int tagareaindex = -1;
  j = 0;
  while (j <= numareas && tagareaindex == -1) {
    if (tagarea == stockareas[j])
      tagareaindex = j;
    j++;
  }
  if (tagareaindex == -1)
    handle.logMessage(LOGFAIL, "Error in tags - invalid area for tagged stock");

  int maxage = stockPopInArea->maxAge();
  int minage = stockPopInArea->minAge();
  int numberofagegroups = maxage - minage + 1;
  int upperlgrp, minl, maxl, age, length, stockid;
  double numfishinarea, numstockinarea;
  IntVector sizeoflengthgroups(numberofagegroups);
  IntVector lowerlengthgroups(numberofagegroups);

  for (i = 0; i < numberofagegroups; i++) {
    lowerlengthgroups[i]= stockPopInArea->minLength(i + minage);
    upperlgrp = stockPopInArea->maxLength(i + minage);
    sizeoflengthgroups[i] = upperlgrp - lowerlengthgroups[i];
  }

  AgeLengthStock.resize(1, new AgeBandMatrixPtrVector(numareas, minage, lowerlengthgroups, sizeoflengthgroups));
  for (age = minage; age <= maxage; age++) {
    minl = stockPopInArea->minLength(age);
    maxl = stockPopInArea->maxLength(age);
    for (length = minl; length < maxl; length++) {
      numfishinarea = NumberInArea[length].N;
      numstockinarea = (*stockPopInArea)[age][length].N;
      if (numfishinarea > verysmall && numstockinarea > verysmall)
        (*AgeLengthStock[0])[tagareaindex][age][length].N = (*NumberByLength[timeid])[0][length - minl] * numstockinarea / numfishinarea;
      else
        (*AgeLengthStock[0])[tagareaindex][age][length].N = 0.0;
    }
  }
  taggingstock->updateTags(AgeLengthStock[0], this, exp(-tagloss));
  updated[0] = 1;

  if (taggingstock->isEaten()) {
    tmpLgrpDiv = taggingstock->returnPrey()->returnLengthGroupDiv();
    IntVector preysize(numberofagegroups, tmpLgrpDiv->numLengthGroups());
    IntVector preyminlength(numberofagegroups, 0);
    NumBeforeEating.resize(1, new AgeBandMatrixPtrVector(numareas, minage, preyminlength, preysize));
    CI.resize(1);
    CI[CI.Size() - 1] = new ConversionIndex(LgrpDiv, tmpLgrpDiv);

    stockid = stockIndex(taggingstock->getName());
    if (stockid < 0 || stockid >= preyindex.Size())
      handle.logMessage(LOGFAIL, "Error in tags - invalid stock identifier");

    preyindex[stockid] = NumBeforeEating.Size() - 1;
  }

  const AgeBandMatrix* allStockPopInArea;
  for (i = 1; i < tagStocks.Size(); i++) {
    Stock* tmpStock = tagStocks[i];
    allStockPopInArea = &tagStocks[i]->getAgeLengthKeys(tagarea);

    stockareas = tmpStock->Areas();
    numareas = stockareas.Size();
    maxage = allStockPopInArea->maxAge();
    minage = allStockPopInArea->minAge();
    numberofagegroups = maxage - minage + 1;
    while (sizeoflengthgroups.Size() > 0) {
      sizeoflengthgroups.Delete(0);
      lowerlengthgroups.Delete(0);
    }
    sizeoflengthgroups.resize(numberofagegroups);
    lowerlengthgroups.resize(numberofagegroups);
    for (j = 0; j < numberofagegroups; j++) {
      lowerlengthgroups[j] = allStockPopInArea->minLength(j + minage);
      upperlgrp = allStockPopInArea->maxLength(j + minage);
      sizeoflengthgroups[j] = upperlgrp - lowerlengthgroups[j];
    }

    AgeLengthStock.resize(1, new AgeBandMatrixPtrVector(numareas, minage, lowerlengthgroups, sizeoflengthgroups));
    if (tmpStock->isEaten()) {
      tmpLgrpDiv = tmpStock->returnPrey()->returnLengthGroupDiv();
      IntVector preysize(numberofagegroups, tmpLgrpDiv->numLengthGroups());
      IntVector preyminlength(numberofagegroups, 0);
      NumBeforeEating.resize(1, new AgeBandMatrixPtrVector(numareas, minage, preyminlength, preysize));
      CI.resize(1);
      CI[CI.Size() - 1] = new ConversionIndex(LgrpDiv, tmpLgrpDiv);

      stockid = stockIndex(tmpStock->getName());
      if (stockid < 0 || stockid >= preyindex.Size())
        handle.logMessage(LOGFAIL, "Error in tags - invalid stock identifier");

      preyindex[stockid] = NumBeforeEating.Size() - 1;
    }
  }
}

void Tags::updateTags(int year, int step) {
  int i, timeid;

  timeid = -1;
  for (i = 0; i < Years.Size(); i++)
    if (Years[i] == year && Steps[i] == step)
      timeid = i;

  if (timeid != -1) {
    if (tagyear == year && tagstep == step)
      this->Update(timeid);
    else
      this->addToTagStock(timeid);
  }
}

void Tags::deleteStockTags() {
  int i;
  for (i = 0; i < tagStocks.Size(); i++) {
    if (updated[i] == 1) {
      tagStocks[i]->deleteTags(this->getName());
      updated[i] = 2;
    }
  }
}

void Tags::updateMatureStock(const TimeClass* const TimeInfo) {
  int i, id;
  int currentYear = TimeInfo->getYear();
  int currentStep = TimeInfo->getStep();

  if (endyear <= currentYear)
    handle.logMessage(LOGWARN, "Warning in tags - tagging experiment has finished");
  else
    for (i = 0; i < matureStocks.Size(); i++) {
      id = stockIndex(matureStocks[i]->getName());
      if (id < 0 || id >= AgeLengthStock.Size())
        handle.logMessage(LOGFAIL, "Error in tags - invalid stock identifier");

      if (updated[id] == 0) {
        matureStocks[i]->updateTags(AgeLengthStock[id], this, exp(-tagloss));
        updated[id] = 1;
      }
    }
}

void Tags::updateTransitionStock(const TimeClass* const TimeInfo) {
  int i, id;
  int currentYear = TimeInfo->getYear();
  int currentStep = TimeInfo->getStep();

  if (endyear <= currentYear)
    handle.logMessage(LOGWARN, "Warning in tags - tagging experiment has finished");
  else
    for (i = 0; i < transitionStocks.Size(); i++) {
      id = stockIndex(transitionStocks[i]->getName());
      if (id < 0 || id >= AgeLengthStock.Size())
        handle.logMessage(LOGFAIL, "Error in tags - invalid stock identifier");

      if (updated[id] == 0) {
        transitionStocks[i]->updateTags(AgeLengthStock[id], this, exp(-tagloss));
        updated[id] = 1;
      }
    }
}

void Tags::updateStrayStock(const TimeClass* const TimeInfo) {
  int i, id;
  int currentYear = TimeInfo->getYear();
  int currentStep = TimeInfo->getStep();

  if (endyear <= currentYear)
    handle.logMessage(LOGWARN, "Warning in tags - tagging experiment has finished");
  else
    for (i = 0; i < strayStocks.Size(); i++) {
      id = stockIndex(strayStocks[i]->getName());
      if (id < 0 || id >= AgeLengthStock.Size())
        handle.logMessage(LOGFAIL, "Error in tags - invalid stock identifier");

      if (updated[id] == 0) {
        strayStocks[i]->updateTags(AgeLengthStock[id], this, exp(-tagloss));
        updated[id] = 1;
      }
    }
}

int Tags::stockIndex(const char* stockname) {
  int i;

  for (i = 0; i < tagStocks.Size(); i++)
    if (strcasecmp(stockname, tagStocks[i]->getName()) == 0)
      return i;

  return -1;
}

int Tags::areaIndex(const char* stockname, int area) {
  int i, j;

  for (i = 0; i < tagStocks.Size(); i++)
    if (strcasecmp(stockname, tagStocks[i]->getName()) == 0) {
      IntVector stockareas = tagStocks[i]->Areas();
      for (j = 0; j < stockareas.Size(); j++)
        if (stockareas[j] == area)
          return j;
      return -1;
    }

  return -1;
}

void Tags::storeNumberPriorToEating(int area, const char* stockname) {
  int stockid, preyid, areaid;
  stockid = stockIndex(stockname);
  if (stockid < 0)
    handle.logMessage(LOGFAIL, "Error in tags - invalid stock identifier");

  preyid = preyindex[stockid];
  if (preyid > NumBeforeEating.Size() || preyid < 0)
    handle.logMessage(LOGFAIL, "Error in tags - invalid prey identifier");

  areaid = areaIndex(stockname, area);
  if (areaid < 0)
    handle.logMessage(LOGFAIL, "Error in tags - invalid area identifier");

  (*NumBeforeEating[preyid])[areaid].setToZero();
  (*NumBeforeEating[preyid])[areaid].Add((*AgeLengthStock[stockid])[areaid], *CI[preyid]);
}

const AgeBandMatrix& Tags::getNumberPriorToEating(int area, const char* stockname) {
  int stockid, preyid, areaid;
  stockid = stockIndex(stockname);
  if (stockid < 0)
    handle.logMessage(LOGFAIL, "Error in tags - invalid stock identifier");

  preyid = preyindex[stockid];
  if (preyid > NumBeforeEating.Size() || preyid < 0)
    handle.logMessage(LOGFAIL, "Error in tags - invalid prey identifier");

  areaid = areaIndex(stockname, area);
  if (areaid < 0)
    handle.logMessage(LOGFAIL, "Error in tags - invalid area identifier");

  return (*NumBeforeEating[preyid])[areaid];
}

int Tags::isWithinPeriod(int year, int step) {
  if ((year > tagyear || (year == tagyear && step >= tagstep)) && (year <= endyear))
    return 1;
  else
    return 0;
}

void Tags::addToTagStock(int timeid) {
  int i, tagareaindex;
  PopInfoVector NumberInArea;
  NumberInArea.resize(LgrpDiv->numLengthGroups());
  const AgeBandMatrix* stockPopInArea;
  stockPopInArea = &(taggingstock->getAgeLengthKeys(tagarea));
  stockPopInArea->sumColumns(NumberInArea);

  i = 0;
  tagareaindex = -1;
  IntVector stockareas = taggingstock->Areas();
  while (i <= stockareas.Size() && tagareaindex == -1) {
    if (tagarea == stockareas[i])
      tagareaindex = i;
    i++;
  }
  if (tagareaindex == -1)
    handle.logMessage(LOGFAIL, "Error in tags - invalid area for tagged stock");

  int maxage = stockPopInArea->maxAge();
  int minage = stockPopInArea->minAge();
  int minl, maxl, age, length;
  double numfishinarea, numstockinarea;

  for (age = minage; age <= maxage; age++) {
    minl = stockPopInArea->minLength(age);
    maxl = stockPopInArea->maxLength(age);
    for (length = minl; length < maxl; length++) {
      numfishinarea = NumberInArea[length].N;
      numstockinarea = (*stockPopInArea)[age][length].N;
      if (numfishinarea > verysmall && numstockinarea > verysmall)
        (*AgeLengthStock[0])[tagareaindex][age][length].N += (*NumberByLength[timeid])[0][length - minl] * numstockinarea / numfishinarea;
    }
  }
}
