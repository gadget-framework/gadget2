#include "stockprinter.h"
#include "conversionindex.h"
#include "stockaggregator.h"
#include "areatime.h"
#include "readfunc.h"
#include "errorhandler.h"
#include "stockptrvector.h"
#include "stock.h"
#include "readword.h"
#include "readaggregation.h"
#include "gadget.h"
#include "runid.h"

extern RunID RUNID;
extern ErrorHandler handle;

StockPrinter::StockPrinter(CommentStream& infile,
  const AreaClass* const Area, const TimeClass* const TimeInfo)
  : Printer(STOCKPRINTER), LgrpDiv(0), aggregator(0) {

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
    handle.Message("Error in stockprinter - failed to create length group");

  //Open the printfile
  readWordAndValue(infile, "printfile", filename);
  outfile.open(filename, ios::out);
  handle.checkIfFailure(outfile, filename);

  infile >> text >> ws;
  if (!(strcasecmp(text, "yearsandsteps") == 0))
    handle.Unexpected("yearsandsteps", text);
  if (!AAT.readFromFile(infile, TimeInfo))
    handle.Message("Error in stockprinter - wrong format for yearsandsteps");

  //prepare for next printfile component
  infile >> ws;
  if (!infile.eof()) {
    infile >> text >> ws;
    if (!(strcasecmp(text, "[component]") == 0))
      handle.Unexpected("[component]", text);
  }

  //finished initializing. Now print first lines
  outfile << "; ";
  RUNID.print(outfile);
  outfile << "; Output file for the following stocks";
  for (i = 0; i < stocknames.Size(); i++)
    outfile << sep << stocknames[i];

  outfile << "\n; year-step-area-age-length-number-weight\n";
  outfile.flush();
}

void StockPrinter::setStock(StockPtrVector& stockvec) {
  StockPtrVector stocks;
  int index = 0;
  int i, j;

  for (i = 0; i < stockvec.Size(); i++)
    for (j = 0; j < stocknames.Size(); j++)
      if (strcasecmp(stockvec[i]->Name(), stocknames[j]) == 0) {
        stocks.resize(1);
        stocks[index++] = stockvec[i];
      }

  if (stocks.Size() != stocknames.Size()) {
    handle.logWarning("Error in stockprinter - failed to match stocks");
    for (i = 0; i < stocks.Size(); i++)
      handle.logWarning("Error in stockprinter - found stock", stocks[i]->Name());
    for (i = 0; i < stocknames.Size(); i++)
      handle.logWarning("Error in stockprinter - looking for stock", stocknames[i]);
    exit(EXIT_FAILURE);
  }
  
  for (i = 0; i < stocks.Size(); i++)
    for (j = 0; j < stocks.Size(); j++)
      if ((strcasecmp(stocks[i]->Name(), stocks[j]->Name()) == 0) && (i != j))
        handle.logFailure("Error in stockprinter - repeated stock", stocks[i]->Name());
  
  aggregator = new StockAggregator(stocks, LgrpDiv, areas, ages);
}

void StockPrinter::Print(const TimeClass* const TimeInfo) {
  int a, age, l;
  if (!AAT.AtCurrentTime(TimeInfo))
    return;
  aggregator->Sum();

  for (a = 0; a < areas.Nrow(); a++) {
    const AgeBandMatrix& alk = aggregator->returnSum()[a];
    for (age = alk.minAge(); age <= alk.maxAge(); age++) {
      for (l = alk.minLength(age); l < alk.maxLength(age); l++) {
        outfile << setw(lowwidth) << TimeInfo->CurrentYear() << sep
          << setw(lowwidth) << TimeInfo->CurrentStep() << sep
          << setw(printwidth) << areaindex[a] << sep << setw(printwidth)
          << ageindex[age] << sep << setw(printwidth) << lenindex[l] << sep;

        //JMB crude filter to remove the 'silly' values from the output
        if ((alk[age][l].N < rathersmall) || (alk[age][l].W < 0))
          outfile << setw(largewidth) << 0 << sep << setw(largewidth) << 0 << endl;
        else
          outfile << setprecision(largeprecision) << setw(largewidth) << alk[age][l].N << sep
            << setprecision(largeprecision) << setw(largewidth) << alk[age][l].W << endl;

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
