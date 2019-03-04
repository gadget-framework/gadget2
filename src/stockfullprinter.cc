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
#include "global.h"

StockFullPrinter::StockFullPrinter(CommentStream& infile, const TimeClass* const TimeInfo)
  : Printer(STOCKFULLPRINTER), stockname(0), aggregator(0), LgrpDiv(0), alptr(0) {

  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);

  stockname = new char[MaxStrLength];
  strncpy(stockname, "", MaxStrLength);
  readWordAndValue(infile, "stockname", stockname);

  //JMB - removed the need to read in the area aggregation file
  infile >> text >> ws;
  if (strcasecmp(text, "areaaggfile") == 0) {
    infile >> text >> ws;
    handle.logMessage(LOGWARN, "Warning in stockfullprinter - area aggregation file ignored");
    infile >> text >> ws;
  }

  //open the printfile
  filename = new char[MaxStrLength];
  strncpy(filename, "", MaxStrLength);
  //readWordAndValue(infile, "printfile", filename);
  if (strcasecmp(text, "printfile") != 0)
    handle.logFileUnexpected(LOGFAIL, "printfile", text);
  infile >> filename >> ws;

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
    handle.logFileMessage(LOGFAIL, "\nError in stockfullprinter - invalid value of precision");

  if (strcasecmp(text, "printatstart") == 0)
    infile >> printtimeid >> ws >> text >> ws;
  else
    printtimeid = 0;

  if (printtimeid != 0 && printtimeid != 1)
    handle.logFileMessage(LOGFAIL, "\nError in stockfullprinter - invalid value of printatstart");

  if (strcasecmp(text, "yearsandsteps") != 0)
    handle.logFileUnexpected(LOGFAIL, "yearsandsteps", text);
  if (!AAT.readFromFile(infile, TimeInfo))
    handle.logFileMessage(LOGFAIL, "\nError in stockfullprinter - wrong format for yearsandsteps");

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
  outfile << "; Full output file for the stock " << stockname;

  if (printtimeid == 0)
    outfile << "\n; Printing the following information at the end of each timestep";
  else
    outfile << "\n; Printing the following information at the start of each timestep";

  // BTHE compatibility with Rgadget::read.gadget.file
  outfile << "\n; -- data --";

  outfile << "\n; year step area age length number mean_weight\n";
  outfile.flush();
}

StockFullPrinter::~StockFullPrinter() {
  outfile.close();
  outfile.clear();
  delete aggregator;
  delete LgrpDiv;
  delete[] stockname;
}

void StockFullPrinter::setStock(StockPtrVector& stockvec, const AreaClass* const Area) {
  StockPtrVector stocks;
  int i, maxage;

  for (i = 0; i < stockvec.Size(); i++)
    if (strcasecmp(stockvec[i]->getName(), stockname) == 0)
      stocks.resize(stockvec[i]);

  if (stocks.Size() != 1) {
    handle.logMessage(LOGWARN, "Error in stockfullprinter - failed to match stocks");
    for (i = 0; i < stocks.Size(); i++)
      handle.logMessage(LOGWARN, "Error in stockfullprinter - found stock", stocks[i]->getName());
    handle.logMessage(LOGFAIL, "Error in stockfullprinter - looking for stock", stockname);
  }

  IntVector areas = stocks[0]->getAreas();
  outerareas.resize(areas.Size(), 0);
  for (i = 0; i < outerareas.Size(); i++)
    outerareas[i] = Area->getModelArea(areas[i]);

  //prepare for the creation of the aggregator
  minage = stocks[0]->minAge();
  maxage = stocks[0]->maxAge();
  IntMatrix agematrix(maxage - minage + 1, 1, 0);
  for (i = 0; i < agematrix.Nrow(); i++)
    agematrix[i][0] = i + minage;
  IntMatrix areamatrix(areas.Size(), 1, 0);
  for (i = 0; i < areamatrix.Nrow(); i++)
    areamatrix[i][0] = areas[i];

  LgrpDiv = new LengthGroupDivision(*stocks[0]->getLengthGroupDiv());
  if (LgrpDiv->Error())
    handle.logMessage(LOGFAIL, "Error in stockfullprinter - failed to create length group");
  aggregator = new StockAggregator(stocks, LgrpDiv, areamatrix, agematrix);
}

void StockFullPrinter::Print(const TimeClass* const TimeInfo, int printtime) {

  if ((!AAT.atCurrentTime(TimeInfo)) || (printtime != printtimeid))
    return;

  aggregator->Sum();
  int a, age, len;

  alptr = &aggregator->getSum();
  for (a = 0; a < outerareas.Size(); a++) {
    for (age = (*alptr)[a].minAge(); age <= (*alptr)[a].maxAge(); age++) {
      for (len = (*alptr)[a].minLength(age); len < (*alptr)[a].maxLength(age); len++) {
        outfile << setw(lowwidth) << TimeInfo->getYear() << sep
          << setw(lowwidth) << TimeInfo->getStep() << sep
          << setw(lowwidth) << outerareas[a] << sep << setw(lowwidth)
          << age + minage << sep << setw(lowwidth)
          << LgrpDiv->meanLength(len) << sep;

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
