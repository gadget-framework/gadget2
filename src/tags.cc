#include "tags.h"
#include "areatime.h"
#include "errorhandler.h"
#include "readfunc.h"
#include "readword.h"
#include "stock.h"
#include "stockprey.h"
#include "gadget.h"
#include "global.h"

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
  stocknames.resize(new char[strlen(text) + 1]);
  strcpy(stocknames[0], text);

  //Currently can only have one area per tagging experiment
  readWordAndVariable(infile, "tagarea", tagarea);
  tagarea = Area->getInnerArea(tagarea);

  infile >> ws;
  char c = infile.peek();
  if ((c == 'e') || (c == 'E'))
    readWordAndVariable(infile, "endyear", endyear);
  else
    endyear = TimeInfo->getLastYear();

  int i, found = 0;
  for (i = 0; i < stockvec.Size(); i++) {
    if (strcasecmp(stockvec[i]->getName(), stocknames[0]) == 0) {
      if (found == 0) {
        taggingstock = stockvec[i];
        LgrpDiv = new LengthGroupDivision(*(taggingstock->getLengthGroupDiv()));
        if (LgrpDiv->Error())
          handle.logMessage(LOGFAIL, "Error in tags - failed to create length group");
        tagStocks.resize(taggingstock);
        taggingstock->setTaggedStock();
      }
      found++;
    }
  }
  if (found == 0)
    handle.logMessage(LOGFAIL, "Error in tags - failed to match stock", stocknames[0]);
  if (found > 1)
    handle.logMessage(LOGFAIL, "Error in tags - repeated stock", stocknames[0]);

  //Now read in the tagloss information
  readWordAndVariable(infile, "tagloss", tagloss);
  tagloss.Inform(keeper);

  //read in the numbers format: tagid - length - number
  readWordAndValue(infile, "numbers", text);
  subfile.open(text, ios::binary);
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

  int year, step, count, reject;
  int i, lenid, keepdata, timeid;
  int numlen = LgrpDiv->numLengthGroups();
  double tmplength, tmpnumber;
  char tmpname[MaxStrLength];
  strncpy(tmpname, "", MaxStrLength);

  infile >> ws;
  //Check the number of columns in the inputfile
  if (countColumns(infile) != 5)
    handle.logFileMessage(LOGFAIL, "wrong number of columns in inputfile - should be 5");

  year = step = count = reject = 0;
  while (!infile.eof()) {
    keepdata = 1;
    infile >> tmpname >> year >> step >> tmplength >> tmpnumber >> ws;

    //only keep the data if tmpname matches tagname
    if (strcasecmp(tagname, tmpname) != 0)
      keepdata = 0;

    //only keep the data if the length is valid
    lenid = -1;
    for (i = 0; i < numlen; i++)
      if (isEqual(tmplength, LgrpDiv->minLength(i)))
        lenid = i;

    //OK the length doesnt match a minimum length so find the length group it is in
    if ((lenid == -1) && (tmplength > LgrpDiv->minLength()) && (tmplength < LgrpDiv->maxLength())) {
      for (i = 1; i < numlen; i++) {
        if (tmplength < LgrpDiv->minLength(i)) {
          lenid = i - 1;
          break;
        }
      }
      if (lenid == -1)
        lenid = numlen - 1;  //then this must be the last length group
    }

    if (lenid == -1)
      keepdata = 0;

    //only keep the data if the number is positive
    if (tmpnumber < 0.0) {
      handle.logMessage(LOGWARN, "Warning in tags - found negative number of tags", tmpnumber);
      keepdata = 0;
    }

    timeid = -1;
    if ((TimeInfo->isWithinPeriod(year, step)) && (keepdata == 1)) {
      for (i = 0; i < Years.Size(); i++)
        if ((Years[i] == year) && (Steps[i] == step))
          timeid = i;

      if (timeid == -1) {
        Years.resize(1, year);
        Steps.resize(1, step);
        timeid = Years.Size() - 1;
        NumberByLength.resize(new DoubleMatrix(1, numlen, 0.0));
      }

    } else
      keepdata = 0;

    if (keepdata == 1) {
      count++;
      (*NumberByLength[timeid])[0][lenid] += tmpnumber;
    } else
      reject++;  //count number of rejected data points read from file
  }

  if (count == 0)
    handle.logMessage(LOGWARN, "Warning in tags - found no data in the data file for", tagname);
  if (reject != 0)
    handle.logMessage(LOGMESSAGE, "Discarded invalid tags data - number of invalid entries", reject);
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
  while (AgeLengthStock.Nrow() > 0)
    AgeLengthStock.Delete(0);
  while (NumBeforeEating.Nrow() > 0)
    NumBeforeEating.Delete(0);
  delete LgrpDiv;
}

void Tags::Reset() {
  int i;
  while (AgeLengthStock.Nrow() > 0)
    AgeLengthStock.Delete(0);
  while (NumBeforeEating.Nrow() > 0)
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

void Tags::setStock(StockPtrVector& Stocks) {
  int i, j, found;
  StockPtrVector tmpStockVector;
  char* stockname;

  preyindex.resize(1, -1);
  updated.resize(1, 0);
  if (!(taggingstock->isInArea(tagarea)))
    handle.logMessage(LOGFAIL, "Error in tags - stock isnt defined on tagging area");

  if (taggingstock->doesMove()) {
    tmpStockVector = taggingstock->getTransitionStocks();
    for (i = 0; i < tmpStockVector.Size(); i++) {
      transitionStocks.resize(tmpStockVector[i]);
      preyindex.resize(1, -1);
      updated.resize(1, 0);
      tagStocks.resize(tmpStockVector[i]);
      tmpStockVector[i]->setTaggedStock();
    }
  }

  if (taggingstock->doesMature()) {
    tmpStockVector = taggingstock->getMatureStocks();
    for (i = 0; i < tmpStockVector.Size(); i++) {
      matureStocks.resize(tmpStockVector[i]);
      found = 0;
      for (j = 0; j < transitionStocks.Size(); j++)
        if (strcasecmp(transitionStocks[j]->getName(), tmpStockVector[i]->getName()) != 0)
          found++;

      if (found == 0) {
        preyindex.resize(1, -1);
        updated.resize(1, 0);
        tagStocks.resize(tmpStockVector[i]);
        tmpStockVector[i]->setTaggedStock();
      }
    }
  }

  if (taggingstock->doesStray()) {
    tmpStockVector = taggingstock->getStrayStocks();
    for (i = 0; i < tmpStockVector.Size(); i++) {
      strayStocks.resize(tmpStockVector[i]);
      found = 0;
      for (j = 0; j < transitionStocks.Size(); j++)
        if (strcasecmp(transitionStocks[j]->getName(), tmpStockVector[i]->getName()) != 0)
          found++;

      for (j = 0; j < matureStocks.Size(); j++)
        if (strcasecmp(matureStocks[j]->getName(), tmpStockVector[i]->getName()) != 0)
          found++;

      if (found == 0) {
        preyindex.resize(1, -1);
        updated.resize(1, 0);
        tagStocks.resize(tmpStockVector[i]);
        tmpStockVector[i]->setTaggedStock();
      }
    }
  }

  for (i = 1; i < tagStocks.Size(); i++) {
    stockname = new char[strlen(tagStocks[i]->getName()) + 1];
    strcpy(stockname, tagStocks[i]->getName());
    stocknames.resize(stockname);
  }
}

//Must have set stocks according to stocknames using setStock before calling Update()
//Now we need to distribute the tagged fish to the same age/length groups as the tagged stock.
void Tags::Update(int timeid) {
  int i, j;
  PopInfoVector NumberInArea;
  NumberInArea.resizeBlank(LgrpDiv->numLengthGroups());

  const AgeBandMatrix* stockPopInArea;
  const LengthGroupDivision* tmpLgrpDiv;

  stockPopInArea = &(taggingstock->getCurrentALK(tagarea));
  stockPopInArea->sumColumns(NumberInArea);

  //Now we have total number of stock per length in tagarea, N(., l) (NumberInArea) and
  //number of stock per age/length, N(a, l) (stockPopInArea) so we must initialise
  //AgeLengthStock so that it can hold all information of number of tagged stock
  //per area/age/length after endtime. We must make AgeBandMatrixPtrVector same size as
  //the one in stock even though have only one area entry at the beginning
  IntVector stockareas = taggingstock->getAreas();
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
  IntVector lgrpsize(numberofagegroups, 0);
  IntVector lgrpmin(numberofagegroups, 0);

  for (i = 0; i < numberofagegroups; i++) {
    lgrpmin[i]= stockPopInArea->minLength(i + minage);
    upperlgrp = stockPopInArea->maxLength(i + minage);
    lgrpsize[i] = upperlgrp - lgrpmin[i];
  }

  AgeLengthStock.resize(new AgeBandMatrixPtrVector(numareas, minage, lgrpmin, lgrpsize));
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
  taggingstock->addTags(AgeLengthStock[0], this, exp(-tagloss));
  updated[0] = 1;

  if (taggingstock->isEaten()) {
    tmpLgrpDiv = taggingstock->getPrey()->getLengthGroupDiv();
    lgrpmin.Reset();
    lgrpsize.Reset();
    lgrpmin.resize(numberofagegroups, 0);
    lgrpsize.resize(numberofagegroups, tmpLgrpDiv->numLengthGroups());
    NumBeforeEating.resize(new AgeBandMatrixPtrVector(numareas, minage, lgrpmin, lgrpsize));
    CI.resize(new ConversionIndex(LgrpDiv, tmpLgrpDiv));
    if (CI[CI.Size() - 1]->Error())
      handle.logMessage(LOGFAIL, "Error in tags - error when checking length structure");

    stockid = stockIndex(taggingstock->getName());
    if (stockid < 0 || stockid >= preyindex.Size())
      handle.logMessage(LOGFAIL, "Error in tags - invalid stock identifier");

    preyindex[stockid] = NumBeforeEating.Nrow() - 1;
  }

  for (i = 1; i < tagStocks.Size(); i++) {
    stockPopInArea = &tagStocks[i]->getCurrentALK(tagarea);
    stockareas = tagStocks[i]->getAreas();
    numareas = stockareas.Size();
    maxage = stockPopInArea->maxAge();
    minage = stockPopInArea->minAge();
    numberofagegroups = maxage - minage + 1;
    lgrpmin.Reset();
    lgrpsize.Reset();
    lgrpmin.resize(numberofagegroups, 0);
    lgrpsize.resize(numberofagegroups, 0);
    for (j = 0; j < numberofagegroups; j++) {
      lgrpmin[j] = stockPopInArea->minLength(j + minage);
      upperlgrp = stockPopInArea->maxLength(j + minage);
      lgrpsize[j] = upperlgrp - lgrpmin[j];
    }

    AgeLengthStock.resize(new AgeBandMatrixPtrVector(numareas, minage, lgrpmin, lgrpsize));
    if (tagStocks[i]->isEaten()) {
      tmpLgrpDiv = tagStocks[i]->getPrey()->getLengthGroupDiv();
      lgrpmin.Reset();
      lgrpsize.Reset();
      lgrpmin.resize(numberofagegroups, 0);
      lgrpsize.resize(numberofagegroups, tmpLgrpDiv->numLengthGroups());
      NumBeforeEating.resize(new AgeBandMatrixPtrVector(numareas, minage, lgrpmin, lgrpsize));
      CI.resize(new ConversionIndex(LgrpDiv, tmpLgrpDiv));
      if (CI[CI.Size() - 1]->Error())
        handle.logMessage(LOGFAIL, "Error in tags - error when checking length structure");

      stockid = stockIndex(tagStocks[i]->getName());
      if (stockid < 0 || stockid >= preyindex.Size())
        handle.logMessage(LOGFAIL, "Error in tags - invalid stock identifier");

      preyindex[stockid] = NumBeforeEating.Nrow() - 1;
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

  if (endyear <= TimeInfo->getYear())
    handle.logMessage(LOGWARN, "Warning in tags - tagging experiment has finished");
  else {
    for (i = 0; i < matureStocks.Size(); i++) {
      id = stockIndex(matureStocks[i]->getName());
      if (id < 0 || id >= AgeLengthStock.Nrow())
        handle.logMessage(LOGFAIL, "Error in tags - invalid stock identifier");

      if (updated[id] == 0) {
        matureStocks[i]->addTags(AgeLengthStock[id], this, exp(-tagloss));
        updated[id] = 1;
      }
    }
  }
}

void Tags::updateTransitionStock(const TimeClass* const TimeInfo) {
  int i, id;

  if (endyear <= TimeInfo->getYear())
    handle.logMessage(LOGWARN, "Warning in tags - tagging experiment has finished");
  else {
    for (i = 0; i < transitionStocks.Size(); i++) {
      id = stockIndex(transitionStocks[i]->getName());
      if (id < 0 || id >= AgeLengthStock.Nrow())
        handle.logMessage(LOGFAIL, "Error in tags - invalid stock identifier");

      if (updated[id] == 0) {
        transitionStocks[i]->addTags(AgeLengthStock[id], this, exp(-tagloss));
        updated[id] = 1;
      }
    }
  }
}

void Tags::updateStrayStock(const TimeClass* const TimeInfo) {
  int i, id;

  if (endyear <= TimeInfo->getYear())
    handle.logMessage(LOGWARN, "Warning in tags - tagging experiment has finished");
  else {
    for (i = 0; i < strayStocks.Size(); i++) {
      id = stockIndex(strayStocks[i]->getName());
      if (id < 0 || id >= AgeLengthStock.Nrow())
        handle.logMessage(LOGFAIL, "Error in tags - invalid stock identifier");

      if (updated[id] == 0) {
        strayStocks[i]->addTags(AgeLengthStock[id], this, exp(-tagloss));
        updated[id] = 1;
      }
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
  for (i = 0; i < tagStocks.Size(); i++) {
    if (strcasecmp(stockname, tagStocks[i]->getName()) == 0) {
      IntVector stockareas = tagStocks[i]->getAreas();
      for (j = 0; j < stockareas.Size(); j++)
        if (stockareas[j] == area)
          return j;
      return -1;
    }
  }
  return -1;
}

void Tags::storeConsumptionALK(int area, const char* stockname) {
  int stockid, preyid, areaid;
  stockid = stockIndex(stockname);
  if (stockid < 0)
    handle.logMessage(LOGFAIL, "Error in tags - invalid stock identifier");

  preyid = preyindex[stockid];
  if (preyid > NumBeforeEating.Nrow() || preyid < 0)
    handle.logMessage(LOGFAIL, "Error in tags - invalid prey identifier");

  areaid = areaIndex(stockname, area);
  if (areaid < 0)
    handle.logMessage(LOGFAIL, "Error in tags - invalid area identifier");

  (*NumBeforeEating[preyid])[areaid].setToZero();
  (*NumBeforeEating[preyid])[areaid].Add((*AgeLengthStock[stockid])[areaid], *CI[preyid]);
}

const AgeBandMatrix& Tags::getConsumptionALK(int area, const char* stockname) {
  int stockid, preyid, areaid;
  stockid = stockIndex(stockname);
  if (stockid < 0)
    handle.logMessage(LOGFAIL, "Error in tags - invalid stock identifier");

  preyid = preyindex[stockid];
  if (preyid > NumBeforeEating.Nrow() || preyid < 0)
    handle.logMessage(LOGFAIL, "Error in tags - invalid prey identifier");

  areaid = areaIndex(stockname, area);
  if (areaid < 0)
    handle.logMessage(LOGFAIL, "Error in tags - invalid area identifier");

  return (*NumBeforeEating[preyid])[areaid];
}

int Tags::isWithinPeriod(int year, int step) {
  if ((year > tagyear || (year == tagyear && step >= tagstep)) && (year <= endyear))
    return 1;
  return 0;
}

void Tags::addToTagStock(int timeid) {
  int i, tagareaindex;
  PopInfoVector NumberInArea;
  NumberInArea.resizeBlank(LgrpDiv->numLengthGroups());
  const AgeBandMatrix* stockPopInArea;
  stockPopInArea = &(taggingstock->getCurrentALK(tagarea));
  stockPopInArea->sumColumns(NumberInArea);

  i = 0;
  tagareaindex = -1;
  IntVector stockareas = taggingstock->getAreas();
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

void Tags::Print(ofstream& outfile) const {
//  outfile << "\nTag\nName" << sep << this->getName() << endl;
}
