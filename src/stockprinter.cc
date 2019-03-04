#include "stockprinter.h"
#include "conversionindex.h"
#include "stockaggregator.h"
#include "areatime.h"
#include "readfunc.h"
#include "mathfunc.h"
#include "errorhandler.h"
#include "stockptrvector.h"
#include "stock.h"
#include "readword.h"
#include "readaggregation.h"
#include "gadget.h"
#include "runid.h"
#include "global.h"

StockPrinter::StockPrinter(CommentStream& infile, const TimeClass* const TimeInfo)
  : Printer(STOCKPRINTER), LgrpDiv(0), aggregator(0), alptr(0) {

  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  int i, j;

  //read in the stocknames
  i = 0;
  infile >> text >> ws;
  if (strcasecmp(text, "stocknames") != 0)
    handle.logFileUnexpected(LOGFAIL, "stocknames", text);
  infile >> text >> ws;
  while (!infile.eof() && (strcasecmp(text, "areaaggfile") != 0)) {
    stocknames.resize(new char[strlen(text) + 1]);
    strcpy(stocknames[i++], text);
    infile >> text >> ws;
  }
  if (stocknames.Size() == 0)
    handle.logFileMessage(LOGFAIL, "\nError in stockprinter - failed to read stocks");
  handle.logMessage(LOGMESSAGE, "Read stock data - number of stocks", stocknames.Size());

  //read in area aggregation from file
  filename = new char[MaxStrLength];
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
  readWordAndValue(infile, "lenaggfile", filename);
  datafile.open(filename, ios::in);
  handle.checkIfFailure(datafile, filename);
  handle.Open(filename);
  i = readLengthAggregation(subdata, lengths, lenindex);
  handle.Close();
  datafile.close();
  datafile.clear();

  //Finished reading from infile.
  LgrpDiv = new LengthGroupDivision(lengths);
  if (LgrpDiv->Error())
    handle.logFileMessage(LOGFAIL, "\nError in stockprinter - failed to create length group");

  //Open the printfile
  readWordAndValue(infile, "printfile", filename);
  outfile.open(filename, ios::out);
  handle.checkIfFailure(outfile, filename);

  infile >> text >> ws;
  if (strcasecmp(text, "precision") == 0) {
    infile >> precision >> ws >> text >> ws;
    width = precision + 4;
  } else {
    // use default values
    precision = largeprecision;
    width = largewidth;
  }

  if (precision < 0)
    handle.logFileMessage(LOGFAIL, "\nError in stockprinter - invalid value of precision");

  if (strcasecmp(text, "printatstart") == 0)
    infile >> printtimeid >> ws >> text >> ws;
  else
    printtimeid = 0;

  if (printtimeid != 0 && printtimeid != 1)
    handle.logFileMessage(LOGFAIL, "\nError in stockprinter - invalid value of printatstart");

  if (strcasecmp(text, "yearsandsteps") != 0)
    handle.logFileUnexpected(LOGFAIL, "yearsandsteps", text);
  if (!AAT.readFromFile(infile, TimeInfo))
    handle.logFileMessage(LOGFAIL, "\nError in stockprinter - wrong format for yearsandsteps");

  //prepare for next printfile component
  infile >> ws;
  if (!infile.eof()) {
    infile >> text >> ws;
    if (strcasecmp(text, "[component]") != 0)
      handle.logFileUnexpected(LOGFAIL, "[component]", text);
  }

  //finished initializing. Now print first lines
  outfile << "; ";
  RUNID.Print(outfile);
  outfile << "; Output file for the following stocks";
  for (i = 0; i < stocknames.Size(); i++)
    outfile << sep << stocknames[i];

  if (printtimeid == 0)
    outfile << "\n; Printing the following information at the end of each timestep";
  else
    outfile << "\n; Printing the following information at the start of each timestep";
  
  // BTHE compatibility with Rgadget::read.gadget.file
  outfile << "\n; -- data --";

  outfile << "\n; year step area age length number mean_weight\n";
  outfile.flush();
}

void StockPrinter::setStock(StockPtrVector& stockvec, const AreaClass* const Area) {
  StockPtrVector stocks;
  delete aggregator;
  int i, j, k, found, minage, maxage;

  for (i = 0; i < stockvec.Size(); i++)
    for (j = 0; j < stocknames.Size(); j++)
      if (strcasecmp(stockvec[i]->getName(), stocknames[j]) == 0)
        stocks.resize(stockvec[i]);

  if (stocks.Size() != stocknames.Size()) {
    handle.logMessage(LOGWARN, "Error in stockprinter - failed to match stocks");
    for (i = 0; i < stocks.Size(); i++)
      handle.logMessage(LOGWARN, "Error in stockprinter - found stock", stocks[i]->getName());
    for (i = 0; i < stocknames.Size(); i++)
      handle.logMessage(LOGWARN, "Error in stockprinter - looking for stock", stocknames[i]);
    handle.logMessage(LOGFAIL, ""); //JMB this will exit gadget
  }

  for (i = 0; i < stocks.Size(); i++)
    for (j = 0; j < stocks.Size(); j++)
      if ((strcasecmp(stocks[i]->getName(), stocks[j]->getName()) == 0) && (i != j))
        handle.logMessage(LOGFAIL, "Error in stockprinter - repeated stock", stocks[i]->getName());

  //change from outer areas to inner areas.
  for (i = 0; i < areas.Nrow(); i++)
    for (j = 0; j < areas.Ncol(i); j++)
      areas[i][j] = Area->getInnerArea(areas[i][j]);

  //check stock areas, ages and lengths
  if (handle.getLogLevel() >= LOGWARN) {
    for (j = 0; j < areas.Nrow(); j++) {
      found = 0;
      for (i = 0; i < stocks.Size(); i++)
        for (k = 0; k < areas.Ncol(j); k++)
          if (stocks[i]->isInArea(areas[j][k]))
            found++;
      if (found == 0)
        handle.logMessage(LOGWARN, "Warning in stockprinter - stock not defined on all areas");
    }

    minage = 9999;
    maxage = -1;
    for (i = 0; i < ages.Nrow(); i++) {
      for (j = 0; j < ages.Ncol(i); j++) {
        minage = min(ages[i][j], minage);
        maxage = max(ages[i][j], maxage);
      }
    }

    found = 0;
    for (i = 0; i < stocks.Size(); i++)
      if (minage >= stocks[i]->minAge())
        found++;
    if (found == 0)
      handle.logMessage(LOGWARN, "Warning in stockprinter - minimum age less than stock age");

    found = 0;
    for (i = 0; i < stocks.Size(); i++)
      if (maxage <= stocks[i]->maxAge())
        found++;
    if (found == 0)
      handle.logMessage(LOGWARN, "Warning in stockprinter - maximum age greater than stock age");

    found = 0;
    for (i = 0; i < stocks.Size(); i++)
      if (LgrpDiv->maxLength(0) > stocks[i]->getLengthGroupDiv()->minLength())
        found++;
    if (found == 0)
      handle.logMessage(LOGWARN, "Warning in stockprinter - minimum length group less than stock length");

    found = 0;
    for (i = 0; i < stocks.Size(); i++)
      if (LgrpDiv->minLength(LgrpDiv->numLengthGroups()) < stocks[i]->getLengthGroupDiv()->maxLength())
        found++;
    if (found == 0)
      handle.logMessage(LOGWARN, "Warning in stockprinter - maximum length group greater than stock length");
  }

  aggregator = new StockAggregator(stocks, LgrpDiv, areas, ages);
}

void StockPrinter::Print(const TimeClass* const TimeInfo, int printtime) {

  if ((!AAT.atCurrentTime(TimeInfo)) || (printtime != printtimeid))
    return;

  aggregator->Sum();
  int a, age, len;

  alptr = &aggregator->getSum();
  for (a = 0; a < areas.Nrow(); a++) {
    for (age = (*alptr)[a].minAge(); age <= (*alptr)[a].maxAge(); age++) {
      for (len = (*alptr)[a].minLength(age); len < (*alptr)[a].maxLength(age); len++) {
        outfile << setw(lowwidth) << TimeInfo->getYear() << sep
          << setw(lowwidth) << TimeInfo->getStep() << sep
          << setw(printwidth) << areaindex[a] << sep << setw(printwidth)
          << ageindex[age] << sep << setw(printwidth) << lenindex[len] << sep;

        //JMB crude filter to remove the 'silly' values from the output
        if (((*alptr)[a][age][len].N < rathersmall) || ((*alptr)[a][age][len].W < 0.0))
          outfile << setw(width) << 0 << sep << setw(width) << 0 << endl;
        else
          outfile << setprecision(precision) << setw(width) << (*alptr)[a][age][len].N << sep
            << setprecision(precision) << setw(width) << (*alptr)[a][age][len].W << endl;

      }
    }
  }
  outfile.flush();
}

StockPrinter::~StockPrinter() {
  outfile.close();
  outfile.clear();
  delete LgrpDiv;
  delete aggregator;
  int i;
  for (i = 0; i < stocknames.Size(); i++)
    delete[] stocknames[i];
  for (i = 0; i < lenindex.Size(); i++)
    delete[] lenindex[i];
  for (i = 0; i < ageindex.Size(); i++)
    delete[] ageindex[i];
  for (i = 0; i < areaindex.Size(); i++)
    delete[] areaindex[i];
}
