#include "formatedstockprinter.h"
#include "conversionindex.h"
#include "stockaggregator.h"
#include "areatime.h"
#include "readword.h"
#include "readaggregation.h"
#include "errorhandler.h"
#include "stockptrvector.h"
#include "stock.h"
#include "formatedprinting.h"
#include "runid.h"
#include "gadget.h"

extern RunID RUNID;
extern ErrorHandler handle;

/*  FormatedStockPrinter
 *
 *  Purpose:  Constructor
 *
 *  In:  CommentStream& infile   :Input file
 *       AreaClass* Area         :Area definition
 *       TimeClass* TimeInfo     :Time definition
 *
 *  Usage:  FormatedStockPrinter(infile, Area, Time)
 *
 *  Pre:  infile, Area, & Time are valid according to StockPrinter documentation.
 */

FormatedStockPrinter::FormatedStockPrinter(CommentStream& infile,
  const AreaClass* const Area, const TimeClass* const TimeInfo)
  : Printer(FORMATEDSTOCKPRINTER), LgrpDiv(0), aggregator(0) {

  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  int i, j;

  //read in the stocknames
  i = 0;
  infile >> text >> ws;
  if (!(strcasecmp(text, "stocknames") == 0))
    handle.Unexpected("stocknames", text);
  infile >> text >> ws;
  while (!infile.eof() && !(strcasecmp(text, "areaaggfile") == 0)) {
    stocknames.resize(1);
    stocknames[i] = new char[strlen(text) + 1];
    strcpy(stocknames[i++], text);
    infile >> text >> ws;
  }

  //read in area aggregation from file
  char filename[MaxStrLength];
  strncpy(filename, "", MaxStrLength);
  ifstream datafile;
  CommentStream subdata(datafile);

  infile >> filename >> ws;
  datafile.open(filename, ios::in);
  handle.checkIfFailure(datafile, filename);
  handle.Open(filename);
  i = readAggregation(subdata, areas, areaindex);
  handle.Close();
  datafile.close();
  datafile.clear();

  //read in age aggregation from file
  readWordAndValue(infile, "ageaggfile", filename);
  datafile.open(filename, ios::in);
  handle.checkIfFailure(datafile, filename);
  handle.Open(filename);
  i = readAggregation(subdata, ages, ageindex);
  handle.Close();
  datafile.close();
  datafile.clear();

  //read in length aggregation from file
  DoubleVector lengths;
  CharPtrVector lenindex;
  readWordAndValue(infile, "lenaggfile", filename);
  datafile.open(filename, ios::in);
  handle.checkIfFailure(datafile, filename);
  handle.Open(filename);
  i = readLengthAggregation(subdata, lengths, lenindex);
  handle.Close();
  datafile.close();
  datafile.clear();

  //Must change from outer areas to inner areas.
  for (i = 0; i < areas.Nrow(); i++)
    for (j = 0; j < areas.Ncol(i); j++)
      if ((areas[i][j] = Area->InnerArea(areas[i][j])) == -1)
        handle.UndefinedArea(areas[i][j]);

  //Finished reading from infile.
  LgrpDiv = new LengthGroupDivision(lengths);
  if (LgrpDiv->Error())
    handle.Message("Error in formatedstockprinter - failed to create length group");

  infile >> text >> ws;
  if (strcasecmp(text, "printfiles") == 0) {
    infile >> text >> ws;
    noutfile.open(text, ios::out);
    handle.checkIfFailure(noutfile, text);
    noutfile << "; ";
    RUNID.print(noutfile);
    noutfile.flush();
    infile >> text >> ws;
    woutfile.open(text, ios::out);
    handle.checkIfFailure(woutfile, text);
    woutfile << "; ";
    RUNID.print(woutfile);
    woutfile.flush();
  } else
    handle.Unexpected("printfiles", text);

  infile >> text >> ws;
  if (!(strcasecmp(text, "yearsandsteps") == 0))
    handle.Unexpected("yearsandsteps", text);
  if (!AAT.readFromFile(infile, TimeInfo))
    handle.Message("Error in formatedstockprinter - wrong format for yearsandsteps");

  //prepare for next printfile component
  infile >> ws;
  if (!infile.eof()) {
    infile >> text >> ws;
    if (!(strcasecmp(text, "[component]") == 0))
      handle.Unexpected("[component]", text);
  }

  for (i = 0; i < lenindex.Size(); i++)
    delete[] lenindex[i];
}

/*  setStock
 *
 *  Purpose:  Initialise vector of stocks to be printed
 *
 *  In:       StockPtrVector& stockvec    :vector of all stocks, the stock names from the
 *                                         input files are matched with these.
 *
 *  Usage:  setStock(stockvec)
 *
 *  Pre:  stockvec.Size() > 0, all stocks names in input file are in stockvec
 */

void FormatedStockPrinter::setStock(StockPtrVector& stockvec) {
  assert(stockvec.Size() > 0);
  StockPtrVector stocks;
  int i, j, index = 0;
  int maxa, mina;

  for (i = 0; i < stockvec.Size(); i++)
    for (j = 0; j < stocknames.Size(); j++)
      if (strcasecmp(stockvec[i]->Name(), stocknames[j]) == 0) {
        stocks.resize(1);
        stocks[index++] = stockvec[i];
      }

  if (stocks.Size() != stocknames.Size()) {
    handle.logWarning("Error in formatedstockprinter - failed to match stocks");
    for (i = 0; i < stocks.Size(); i++)
      handle.logWarning("Error in formatedstockprinter - found stock", stocks[i]->Name());
    for (i = 0; i < stocknames.Size(); i++)
      handle.logWarning("Error in formatedstockprinter - looking for stock", stocknames[i]);
    exit(EXIT_FAILURE);
  }

  if (ages.Nrow() == 0) { //fill in ages with one age per row
    maxa = 0;
    mina = stocks[0]->minAge();
    for (i = 0; i < stocks.Size();i++) {
      if (stocks[i]->minAge() < mina)
        mina = stocks[i]->minAge();
      if (stocks[i]->maxAge() > maxa)
        maxa = stocks[i]->maxAge();
    }
    for (i = mina; i <= maxa; i++)
      ages.AddRows(1, 1, i);
  }
  for (i = 0; i < ages.Nrow(); i++)
    agevector.resize(1, ages[i][0]);

  aggregator = new StockAggregator(stocks, LgrpDiv, areas, ages);

  //JMB len set to 100???
  int len = 100;
  char* name = new char[len];
  strncpy(name, "", len);
  for (i = 0; i < stocks.Size(); i++) {
    strncat(name, stocks[i]->Name(), len - 2);
    strcat(name, " ");
    len -= (strlen(stocks[i]->Name()) + 1);
  }
  printStockHeader(noutfile, name, areas, ages, *LgrpDiv);
  printStockHeader(woutfile, name, areas, ages, *LgrpDiv);
  delete[] name;
}

void FormatedStockPrinter::Print(const TimeClass* const TimeInfo) {
  if (!AAT.AtCurrentTime(TimeInfo))
    return;
  aggregator->Sum();
  int i;
  printTime(noutfile, *TimeInfo);
  for (i = 0; i < areas.Nrow(); i++) {
    printAreasHeader(noutfile, areas[i]);
    printMatrixHeader(noutfile, agevector, *LgrpDiv, "number", 1);
    printN(noutfile, aggregator->returnSum()[i], &agevector);
  }
  printTime(woutfile, *TimeInfo);
  for (i = 0; i < areas.Nrow(); i++) {
    printAreasHeader(woutfile, areas[i]);
    printMatrixHeader(woutfile, agevector, *LgrpDiv, "weight", 1);
    printW(woutfile, aggregator->returnSum()[i], &agevector);
  }
}

FormatedStockPrinter::~FormatedStockPrinter() {
  noutfile.close();
  noutfile.clear();
  woutfile.close();
  woutfile.clear();
  delete LgrpDiv;
  delete aggregator;
  int i;
  for (i = 0; i < stocknames.Size(); i++)
    delete[] stocknames[i];
  for (i = 0; i < areaindex.Size(); i++)
    delete[] areaindex[i];
  for (i = 0; i < ageindex.Size(); i++)
    delete[] ageindex[i];
}
