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
extern ErrorHandler handle;

StockFullPrinter::StockFullPrinter(CommentStream& infile, const TimeClass* const TimeInfo)
  : Printer(STOCKFULLPRINTER), stockname(0), aggregator(0), LgrpDiv(0) {

  char text[MaxStrLength];
  char filename[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  strncpy(filename, "", MaxStrLength);

  stockname = new char[MaxStrLength];
  strncpy(stockname, "", MaxStrLength);
  readWordAndValue(infile, "stockname", stockname);

  //JMB - removed the need to read in the area aggregation file
  infile >> text >> ws;
  if (strcasecmp(text, "areaaggfile") == 0) {
    infile >> text >> ws;
    handle.Warning("Warning in stockfullprinter - area aggreagtion file ignored");
    infile >> text >> ws;
  }

  //open the printfile
  //readWordAndValue(infile, "printfile", filename);
  if (strcasecmp(text, "printfile") == 0)
    infile >> filename >> ws >> text >> ws;
  else
    handle.Unexpected("printfile", text);

  outfile.open(filename, ios::out);
  handle.checkIfFailure(outfile, filename);

  //infile >> text >> ws;
  if (strcasecmp(text, "precision") == 0)
    infile >> precision >> ws >> text >> ws;
  else
    precision = 0;

  if (precision < 0)
    handle.Message("Error in stockfullprinter - invalid value of precision");

  if (strcasecmp(text, "printatstart") == 0)
    infile >> printtimeid >> ws >> text >> ws;
  else
    printtimeid = 0;

  if (printtimeid != 0 && printtimeid != 1)
    handle.Message("Error in stockfullprinter - invalid value of printatstart");

  if (!(strcasecmp(text, "yearsandsteps") == 0))
    handle.Unexpected("yearsandsteps", text);
  if (!AAT.readFromFile(infile, TimeInfo))
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
  outfile << "; Full output file for the stock " << stockname;

  if (printtimeid == 0)
    outfile << "\n; Printing the following information at the end of each timestep";
  else
    outfile << "\n; Printing the following information at the start of each timestep";

  outfile << "\n; year-step-area-age-length-number-mean weight\n";
  outfile.flush();
}

StockFullPrinter::~StockFullPrinter() {
  outfile.close();
  outfile.clear();
  delete aggregator;
  delete LgrpDiv;
  delete[] stockname;
}

void StockFullPrinter::setStock(StockPtrVector& stockvec) {
  CharPtrVector stocknames(1, stockname);
  StockPtrVector stocks;
  int index = 0;
  int i, j, tmpage;

  for (i = 0; i < stockvec.Size(); i++)
    for (j = 0; j < stocknames.Size(); j++)
      if (strcasecmp(stockvec[i]->Name(), stocknames[j]) == 0) {
        stocks.resize(1);
        stocks[index++] = stockvec[i];
      }

  if (stocks.Size() != stocknames.Size()) {
    handle.logWarning("Error in stockfullprinter - failed to match stocks");
    for (i = 0; i < stocks.Size(); i++)
      handle.logWarning("Error in stockfullprinter - found stock", stocks[i]->Name());
    for (i = 0; i < stocknames.Size(); i++)
      handle.logWarning("Error in stockfullprinter - looking for stock", stocknames[i]);
    exit(EXIT_FAILURE);
  }

  areas = stocks[0]->Areas();
  outerareas.resize(areas.Size(), 0);
  for (i = 0; i < outerareas.Size(); i++)
    outerareas[i] = stocks[0]->getPrintArea(stocks[0]->areaNum(areas[i]));

  LgrpDiv = new LengthGroupDivision(*stocks[0]->returnLengthGroupDiv());

  //prepare for the creation of the aggregator
  minage = 100;
  maxage = 0;
  for (i = 0; i < areas.Size(); i++) {
    tmpage = stocks[0]->getAgeLengthKeys(areas[i]).minAge();
    if (tmpage < minage)
      minage = tmpage;
    tmpage = stocks[0]->getAgeLengthKeys(areas[i]).maxAge();
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

void StockFullPrinter::Print(const TimeClass* const TimeInfo, int printtime) {

  if ((!AAT.AtCurrentTime(TimeInfo)) || (printtime != printtimeid))
    return;

  aggregator->Sum();
  int a, age, l, p, w;

  if (precision == 0) {
    p = largeprecision;
    w = largewidth;
  } else {
    p = precision;
    w = precision + 4;
  }

  for (a = 0; a < areas.Size(); a++) {
    const AgeBandMatrix& alk = aggregator->returnSum()[a];
    for (age = alk.minAge(); age <= alk.maxAge(); age++) {
      for (l = alk.minLength(age); l < alk.maxLength(age); l++) {
        outfile << setw(lowwidth) << TimeInfo->CurrentYear() << sep
          << setw(lowwidth) << TimeInfo->CurrentStep() << sep
          << setw(lowwidth) << outerareas[a] << sep << setw(lowwidth)
          << age + minage << sep << setw(lowwidth)
          << LgrpDiv->meanLength(l) << sep;

        //JMB crude filter to remove the 'silly' values from the output
        if ((alk[age][l].N < rathersmall) || (alk[age][l].W < 0))
          outfile << setw(w) << 0 << sep << setw(w) << 0 << endl;
        else
          outfile << setprecision(p) << setw(w) << alk[age][l].N << sep
            << setprecision(p) << setw(w) << alk[age][l].W << endl;

      }
    }
  }
  outfile.flush();
}
