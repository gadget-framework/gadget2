#include "areatime.h"
#include "errorhandler.h"
#include "stockstdprinter.h"
#include "stockaggregator.h"
#include "charptrvector.h"
#include "stockptrvector.h"
#include "stock.h"
#include "popstatistics.h"
#include "conversionindex.h"
#include "stockpreystdinfo.h"
#include "readword.h"
#include "readaggregation.h"
#include "gadget.h"
#include "runid.h"

extern RunID RUNID;
extern ErrorHandler handle;

StockStdPrinter::StockStdPrinter(CommentStream& infile, const TimeClass* const TimeInfo)
  : Printer(STOCKSTDPRINTER), stockname(0), LgrpDiv(0), aggregator(0), preyinfo(0) {

  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);

  filename = new char[MaxStrLength];
  strncpy(filename, "", MaxStrLength);

  stockname = new char[MaxStrLength];
  strncpy(stockname, "", MaxStrLength);
  readWordAndValue(infile, "stockname", stockname);

  infile >> text >> ws;
  if (strcasecmp(text, "scale") == 0)
    infile >> scale >> ws >> text >> ws;
  else
    scale = 1.0;

  if (scale <= 0)
    handle.Message("Error in stockstdprinter - invalid value of scale");

  //JMB - removed the need to read in the area aggregation file
  if (strcasecmp(text, "areaaggfile") == 0) {
    infile >> text >> ws;
    handle.Warning("Warning in stockstdprinter - area aggreagtion file ignored");
    infile >> text >> ws;
  }

  //open the printfile
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
    handle.Message("Error in stockstdprinter - invalid value of precision");

  if (strcasecmp(text, "printatstart") == 0)
    infile >> printtimeid >> ws >> text >> ws;
  else
    printtimeid = 0;

  if (printtimeid != 0 && printtimeid != 1)
    handle.Message("Error in stockstdprinter - invalid value of printatstart");

  if (!(strcasecmp(text, "yearsandsteps") == 0))
    handle.Unexpected("yearsandsteps", text);
  if (!AAT.readFromFile(infile, TimeInfo))
    handle.Message("Error in stockstdprinter - wrong format for yearsandsteps");

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
  outfile << "; Standard output file for the stock " << stockname;

  if (scale != 1.0)
    outfile << "\n; Scaling factor for the number and number consumed is " << scale;

  if (printtimeid == 0)
    outfile << "\n; Printing the following information at the end of each timestep";
  else
    outfile << "\n; Printing the following information at the start of each timestep";

  outfile << "\n; year-step-area-age-number-mean length-mean weight-"
    << "stddev length-number consumed-biomass consumed\n";
  outfile.flush();
}

StockStdPrinter::~StockStdPrinter() {
  outfile.close();
  outfile.clear();
  delete preyinfo;
  delete aggregator;
  delete LgrpDiv;
  delete[] filename;
  delete[] stockname;
}

void StockStdPrinter::setStock(StockPtrVector& stockvec) {
  CharPtrVector stocknames(1, stockname);
  StockPtrVector stocks;
  int index = 0;
  int i, j, tmpage;

  for (i = 0; i < stockvec.Size(); i++)
    for (j = 0; j < stocknames.Size(); j++)
      if (strcasecmp(stockvec[i]->getName(), stocknames[j]) == 0) {
        stocks.resize(1);
        stocks[index++] = stockvec[i];
      }

  if (stocks.Size() != stocknames.Size()) {
    handle.logWarning("Error in stockstdprinter - failed to match stocks");
    for (i = 0; i < stocks.Size(); i++)
      handle.logWarning("Error in stockstdprinter - found stock", stocks[i]->getName());
    for (i = 0; i < stocknames.Size(); i++)
      handle.logWarning("Error in stockstdprinter - looking for stock", stocknames[i]);
    exit(EXIT_FAILURE);
  }

  areas = stocks[0]->Areas();
  outerareas.resize(areas.Size(), 0);
  for (i = 0; i < outerareas.Size(); i++)
    outerareas[i] = stocks[0]->getPrintArea(stocks[0]->areaNum(areas[i]));

  LgrpDiv = new LengthGroupDivision(*stocks[0]->returnLengthGroupDiv());
  if (stocks[0]->isEaten())
    preyinfo = new StockPreyStdInfo((StockPrey*)stocks[0]->returnPrey(), areas);

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

void StockStdPrinter::Print(const TimeClass* const TimeInfo, int printtime) {

  if ((!AAT.AtCurrentTime(TimeInfo)) || (printtime != printtimeid))
    return;

  aggregator->Sum();
  int a, age, p, w;
  double tmpnumber, tmpbiomass;

  if (precision == 0) {
    p = largeprecision;
    w = largewidth;
  } else {
    p = precision;
    w = precision + 4;
  }

  for (a = 0; a < areas.Size(); a++) {
    if (preyinfo)
      preyinfo->Sum(TimeInfo, areas[a]);
    const AgeBandMatrix& alk = aggregator->returnSum()[a];

    for (age = alk.minAge(); age <= alk.maxAge(); age++) {
      PopStatistics popstat(alk[age], LgrpDiv);
      outfile << setw(lowwidth) << TimeInfo->CurrentYear() << sep
        << setw(lowwidth) << TimeInfo->CurrentStep() << sep
        << setw(lowwidth) << outerareas[a] << sep << setw(lowwidth)
        << age + minage << sep;

        //JMB crude filters to remove the 'silly' values from the output
        if (popstat.totalNumber() < rathersmall) {
          outfile << setw(w) << 0 << sep << setw(printwidth) << 0 << sep << setw(printwidth)
            << 0 << sep << setw(printwidth) << 0 << sep << setw(w) << 0 << sep << setw(w) << 0;

        } else {
          outfile << setprecision(p) << setw(w) << popstat.totalNumber() / scale << sep
            << setprecision(printprecision) << setw(printwidth) << popstat.meanLength() << sep
            << setprecision(printprecision) << setw(printwidth) << popstat.meanWeight() << sep
            << setprecision(printprecision) << setw(printwidth) << popstat.sdevLength() << sep;

          if (preyinfo) {
            tmpnumber = preyinfo->NconsumptionByAge(areas[a])[age + minage];
            tmpbiomass = preyinfo->BconsumptionByAge(areas[a])[age + minage];

            if ((tmpnumber < rathersmall) || (tmpbiomass < rathersmall)) {
              outfile << setw(w) << 0 << sep << setw(w) << 0;
            } else {
              outfile << setprecision(p) << setw(w) << tmpnumber / scale
                << sep << setprecision(p) << setw(w) << tmpbiomass;
            }
          } else
            outfile << setw(largewidth) << 0 << sep << setw(largewidth) << 0;

        }
      outfile << endl;
    }
  }
  outfile.flush();
}
