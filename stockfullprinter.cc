#include "stockaggregator.h"
#include "stockfullprinter.h"
#include "errorhandler.h"
#include "stock.h"
#include "conversionindex.h"
#include "stockptrvector.h"
#include "readword.h"
#include "readaggregation.h"
#include "gadget.h"

#include "runid.h"
extern RunID RUNID;

StockFullPrinter::StockFullPrinter(CommentStream& infile,
  const AreaClass* const Area,  const TimeClass* const TimeInfo)
  : Printer(STOCKFULLPRINTER), minage(0), maxage(0),
    stockname(0), aggregator(0), LgrpDiv(0) {

  ErrorHandler handle;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  int i;

  stockname = new char[MaxStrLength];
  strncpy(stockname, "", MaxStrLength);
  readWordAndValue(infile, "stockname", stockname);

  //Read in area aggregation from file
  char filename[MaxStrLength];
  strncpy(filename, "", MaxStrLength);
  ifstream datafile;
  CommentStream subdata(datafile);

  CharPtrVector areaindex;
  IntMatrix tmpareas;
  readWordAndValue(infile, "areaaggfile", filename);
  datafile.open(filename);
  checkIfFailure(datafile, filename);
  handle.Open(filename);
  i = readAggregation(subdata, tmpareas, areaindex);
  handle.Close();
  datafile.close();
  datafile.clear();

  //Check if we read correct input
  if (tmpareas.Nrow() != 1)
    handle.Message("Error - there should be only one aggregated area for stockfullprinter");

  for (i = 0; i < tmpareas.Ncol(0); i++)
    outerareas.resize(1, tmpareas[0][i]);

  //Must change from outer areas to inner areas.
  areas.resize(outerareas.Size());
  for (i = 0; i < areas.Size(); i++)
    if ((areas[i] = Area->InnerArea(outerareas[i])) == -1)
      handle.UndefinedArea(outerareas[i]);

  //Open the printfile
  readWordAndValue(infile, "printfile", filename);
  outfile.open(filename, ios::out);
  checkIfFailure(outfile, filename);

  infile >> text >> ws;
  if (!(strcasecmp(text, "yearsandsteps") == 0))
    handle.Unexpected("yearsandsteps", text);
  if (!aat.ReadFromFile(infile, TimeInfo))
    handle.Message("Error in stockfullprinter - wrong format for yearsandsteps");

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
  outfile << "; Full output file for the stock " << stockname
    << "\n; year-step-area-age-length-number-mean weight\n";
  outfile.flush();

  //areaindex is not required - free up memory
  for (i = 0; i < areaindex.Size(); i++)
    delete[] areaindex[i];
}

StockFullPrinter::~StockFullPrinter() {
  outfile.close();
  outfile.clear();
  delete aggregator;
  delete LgrpDiv;
  delete[] stockname;
}

void StockFullPrinter::SetStock(StockPtrVector& stockvec) {
  assert(stockvec.Size() > 0);
  Stock* stock = 0;
  int err = 0;
  int i, tmpage;
  for (i = 0; i < stockvec.Size(); i++) {
    if (strcasecmp(stockvec[i]->Name(), stockname) == 0) {
      if (stock == 0)
        stock = stockvec[i];
      else
        err = 1;
    }
  }

  if (err || stock == 0) {
    cerr << "Error in printer when searching for stock with name matching: "
      << stockname << "\nFound instead the following stocks:\n";
    for (i = 0; i < stockvec.Size(); i++)
      cerr << stockvec[i]->Name() << sep;
    cerr << endl;
    exit(EXIT_FAILURE);
  }

  StockPtrVector stocks = StockPtrVector(1, stock);
  LgrpDiv = new LengthGroupDivision(*stock->ReturnLengthGroupDiv());

  //Prepare for the creation of the aggregator
  minage = 100;
  maxage = 0;
  for (i = 0; i < areas.Size(); i++) {
    tmpage = stock->Agelengthkeys(areas[i]).Minage();
    if (tmpage < minage)
      minage = tmpage;
    tmpage = stock->Agelengthkeys(areas[i]).Maxage();
    if (tmpage > maxage)
      maxage = tmpage;
  }

  IntMatrix agematrix(maxage - minage + 1, 1);
  for (i = 0; i < agematrix.Nrow(); i++)
    agematrix[i][0] = i + minage;
  IntMatrix areamatrix(areas.Size(), 1);
  for (i = 0; i < areamatrix.Nrow(); i++)
    areamatrix[i][0] = areas[i];
  aggregator = new StockAggregator(stocks, LgrpDiv, areamatrix, agematrix);
}

void StockFullPrinter::Print(const TimeClass* const TimeInfo) {
  if (!aat.AtCurrentTime(TimeInfo))
    return;
  aggregator->Sum();
  int a, age, l;

  for (a = 0; a < areas.Size(); a++) {
    const AgeBandMatrix& alk = aggregator->ReturnSum()[a];
    for (age = alk.Minage(); age <= alk.Maxage(); age++) {
      for (l = alk.Minlength(age); l < alk.Maxlength(age); l++) {
        outfile << setw(lowwidth) << TimeInfo->CurrentYear() << sep
          << setw(lowwidth) << TimeInfo->CurrentStep() << sep
          << setw(lowwidth) << outerareas[a] << sep << setw(lowwidth)
          << age + minage << sep << setw(lowwidth)
          << LgrpDiv->Meanlength(l) << sep;

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
