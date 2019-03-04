#include "areatime.h"
#include "errorhandler.h"
#include "stockstdprinter.h"
#include "stockaggregator.h"
#include "stockpreyaggregator.h"
#include "charptrvector.h"
#include "stockptrvector.h"
#include "preyptrvector.h"
#include "stock.h"
#include "stockprey.h"
#include "conversionindex.h"
#include "readword.h"
#include "gadget.h"
#include "runid.h"
#include "global.h"

StockStdPrinter::StockStdPrinter(CommentStream& infile, const TimeClass* const TimeInfo)
  : Printer(STOCKSTDPRINTER), stockname(0), LgrpDiv(0), saggregator(0), paggregator(0), salptr(0), palptr(0) {

  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);

  filename = new char[MaxStrLength];
  strncpy(filename, "", MaxStrLength);

  isaprey = 0;
  stockname = new char[MaxStrLength];
  strncpy(stockname, "", MaxStrLength);
  readWordAndValue(infile, "stockname", stockname);

  infile >> text >> ws;
  if (strcasecmp(text, "scale") == 0) {
    infile >> scale >> ws >> text >> ws;
    if (scale < rathersmall) {
      handle.logFileMessage(LOGWARN, "scale should be a positive integer - set to default value 1");
      scale = 1.0;
    }

  } else
    scale = 1.0;

  //JMB - removed the need to read in the area aggregation file
  if (strcasecmp(text, "areaaggfile") == 0) {
    infile >> text >> ws;
    handle.logMessage(LOGWARN, "Warning in stockstdprinter - area aggregation file ignored");
    infile >> text >> ws;
  }

  //open the printfile
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
    handle.logFileMessage(LOGFAIL, "\nError in stockstdprinter - invalid value of precision");

  if (strcasecmp(text, "printatstart") == 0)
    infile >> printtimeid >> ws >> text >> ws;
  else
    printtimeid = 0;

  if (printtimeid != 0 && printtimeid != 1)
    handle.logFileMessage(LOGFAIL, "\nError in stockstdprinter - invalid value of printatstart");

  if (strcasecmp(text, "yearsandsteps") != 0)
    handle.logFileUnexpected(LOGFAIL, "yearsandsteps", text);
  if (!AAT.readFromFile(infile, TimeInfo))
    handle.logFileMessage(LOGFAIL, "\nError in stockstdprinter - wrong format for yearsandsteps");

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
  outfile << "; Standard output file for the stock " << stockname;

  if (scale != 1.0) {
    outfile << "\n; Scaling factor for the number and number consumed is " << scale;
    //JMB - store this as 1/scale
    scale = 1.0 / scale;
  }

  if (printtimeid == 0)
    outfile << "\n; Printing the following information at the end of each timestep";
  else
    outfile << "\n; Printing the following information at the start of each timestep";

  // BTHE compatibility with Rgadget::read.gadget.file
  outfile << "\n; -- data --";

  outfile << "\n; year step area age number mean_length mean_weight"
    << " stddev_length number_consumed biomass_consumed\n";
  outfile.flush();
}

StockStdPrinter::~StockStdPrinter() {
  outfile.close();
  outfile.clear();
  if (isaprey)
    delete paggregator;
  delete saggregator;
  delete LgrpDiv;
  delete[] stockname;
}

void StockStdPrinter::setStock(StockPtrVector& stockvec, const AreaClass* const Area) {
  StockPtrVector stocks;
  int i, maxage;

  for (i = 0; i < stockvec.Size(); i++)
    if (strcasecmp(stockvec[i]->getName(), stockname) == 0)
      stocks.resize(stockvec[i]);

  if (stocks.Size() != 1) {
    handle.logMessage(LOGWARN, "Error in stockstdprinter - failed to match stocks");
    for (i = 0; i < stocks.Size(); i++)
      handle.logMessage(LOGWARN, "Error in stockstdprinter - found stock", stocks[i]->getName());
    handle.logMessage(LOGFAIL, "Error in stockstdprinter - looking for stock", stockname);
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
    handle.logMessage(LOGFAIL, "Error in stockstdprinter - failed to create length group");
  saggregator = new StockAggregator(stocks, LgrpDiv, areamatrix, agematrix);

  if (stocks[0]->isEaten()) {
    isaprey = 1;
    PreyPtrVector preys;
    preys.resize(stocks[0]->getPrey());
    //need to construct length group based on the min/max lengths of the stock
    double minl, maxl;
    minl = stocks[0]->getLengthGroupDiv()->minLength();
    maxl = stocks[0]->getLengthGroupDiv()->maxLength();
    LengthGroupDivision* tmpLgrpDiv = new LengthGroupDivision(minl, maxl, maxl - minl);
    if (LgrpDiv->Error())
      handle.logMessage(LOGFAIL, "Error in stockstdprinter - failed to create length group");

    paggregator = new StockPreyAggregator(preys, tmpLgrpDiv, areamatrix, agematrix);
    //JMB tmpLgrpDiv is no longer needed so delete it to free memory
    delete tmpLgrpDiv;
  }
}

void StockStdPrinter::Print(const TimeClass* const TimeInfo, int printtime) {

  if ((!AAT.atCurrentTime(TimeInfo)) || (printtime != printtimeid))
    return;

  saggregator->Sum();
  salptr = &saggregator->getSum();
  if (isaprey) {
    paggregator->Sum();
    palptr = &paggregator->getSum();
  }

  int a, age;
  for (a = 0; a < outerareas.Size(); a++) {
    for (age = (*salptr)[a].minAge(); age <= (*salptr)[a].maxAge(); age++) {
      outfile << setw(lowwidth) << TimeInfo->getYear() << sep
        << setw(lowwidth) << TimeInfo->getStep() << sep
        << setw(lowwidth) << outerareas[a] << sep << setw(lowwidth)
        << age + minage << sep;

      ps.calcStatistics((*salptr)[a][age], LgrpDiv);
      //JMB crude filters to remove the 'silly' values from the output
      if (ps.totalNumber() < rathersmall) {
        outfile << setw(width) << 0 << sep << setw(printwidth) << 0
          << sep << setw(printwidth) << 0 << sep << setw(printwidth) << 0
          << sep << setw(width) << 0 << sep << setw(width) << 0 << endl;

      } else {
        outfile << setprecision(precision) << setw(width) << ps.totalNumber() * scale << sep
          << setprecision(printprecision) << setw(printwidth) << ps.meanLength() << sep
          << setprecision(printprecision) << setw(printwidth) << ps.meanWeight() << sep
          << setprecision(printprecision) << setw(printwidth) << ps.sdevLength() << sep;

        if (isaprey) {
          //JMB crude filter to remove the 'silly' values from the output
          if (((*palptr)[a][age][0].N < rathersmall) || ((*palptr)[a][age][0].W < 0.0))
            outfile << setw(width) << 0 << sep << setw(width) << 0 << endl;
          else
            outfile << setprecision(precision) << setw(width) << (*palptr)[a][age][0].N
              << sep << setprecision(precision) << setw(width)
              << (*palptr)[a][age][0].W * (*palptr)[a][age][0].N << endl;

        } else
          outfile << setw(width) << 0 << sep << setw(width) << 0 << endl;

      }
    }
  }
  outfile.flush();
}
