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

StockPrinter::StockPrinter(CommentStream& infile,
  const AreaClass* const Area, const TimeClass* const TimeInfo)
  : Printer(STOCKPRINTER), LgrpDiv(0), aggregator(0) {

  ErrorHandler handle;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  int i, j;

  //Read in the stocknames
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

  //Read in area aggregation from file
  char filename[MaxStrLength];
  strncpy(filename, "", MaxStrLength);
  ifstream datafile;
  CommentStream subdata(datafile);

  infile >> filename >> ws;
  datafile.open(filename, ios::in);
  checkIfFailure(datafile, filename);
  handle.Open(filename);
  i = readAggregation(subdata, areas, areaindex);
  handle.Close();
  datafile.close();
  datafile.clear();

  //Read in age aggregation from file
  readWordAndValue(infile, "ageaggfile", filename);
  datafile.open(filename, ios::in);
  checkIfFailure(datafile, filename);
  handle.Open(filename);
  i = readAggregation(subdata, ages, ageindex);
  handle.Close();
  datafile.close();
  datafile.clear();

  //Read in length aggregation from file
  DoubleVector lengths;
  readWordAndValue(infile, "lenaggfile", filename);
  datafile.open(filename, ios::in);
  checkIfFailure(datafile, filename);
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
    printLengthGroupError(lengths, "stockprinter");

  //Open the printfile
  readWordAndValue(infile, "printfile", filename);
  outfile.open(filename, ios::out);
  checkIfFailure(outfile, filename);

  infile >> text >> ws;
  if (!(strcasecmp(text, "yearsandsteps") == 0))
    handle.Unexpected("yearsandsteps", text);
  if (!aat.ReadFromFile(infile, TimeInfo))
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

void StockPrinter::SetStock(StockPtrVector& stockvec) {
  assert(stockvec.Size() > 0);
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
    cerr << "Error in printer when searching for stock(s) with name matching:\n";
    for (i = 0; i < stocknames.Size(); i++)
      cerr << (const char*)stocknames[i] << sep;
    cerr << "\nDid only find the stock(s)\n";
    for (i = 0; i < stocks.Size(); i++)
      cerr << (const char*)stocks[i]->Name() << sep;
    cerr << endl;
    exit(EXIT_FAILURE);
  }
  aggregator = new StockAggregator(stocks, LgrpDiv, areas, ages);
}

void StockPrinter::Print(const TimeClass* const TimeInfo) {
  int a, age, l;
  if (!aat.AtCurrentTime(TimeInfo))
    return;
  aggregator->Sum();

  for (a = 0; a < areas.Nrow(); a++) {
    const AgeBandMatrix& alk = aggregator->ReturnSum()[a];
    for (age = alk.Minage(); age <= alk.Maxage(); age++) {
      for (l = alk.Minlength(age); l < alk.Maxlength(age); l++) {
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
