#include "stockpreyfullprinter.h"
#include "areatime.h"
#include "stockpreystdinfo.h"
#include "errorhandler.h"
#include "conversionindex.h"
#include "charptrvector.h"
#include "stock.h"
#include "readword.h"
#include "readaggregation.h"
#include "gadget.h"
#include "runid.h"

extern RunID RUNID;
extern ErrorHandler handle;

StockPreyFullPrinter::StockPreyFullPrinter(CommentStream& infile, const TimeClass* const TimeInfo)
  : Printer(STOCKPREYFULLPRINTER), stockname(0), preyinfo(0) {

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
    handle.Warning("Warning in stockpreyfullprinter - area aggreagtion file ignored");
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
    handle.Message("Error in stockpreyfullprinter - invalid value of precision");

  if (strcasecmp(text, "printatstart") == 0)
    infile >> printtimeid >> ws >> text >> ws;
  else
    printtimeid = 0;

  if (printtimeid != 0 && printtimeid != 1)
    handle.Message("Error in stockpreyfullprinter - invalid value of printatstart");

  if (!(strcasecmp(text, "yearsandsteps") == 0))
    handle.Unexpected("yearsandsteps", text);
  if (!AAT.readFromFile(infile, TimeInfo))
    handle.Message("Error in stockpreyfullprinter - wrong format for yearsandsteps");

  //prepare for next printfile component
  infile >> ws;
  if (!infile.eof()) {
    infile >> text >> ws;
    if (!(strcasecmp(text, "[component]") == 0))
      handle.Unexpected("[component]", text);
  }

  //finished initializing. Now print first line.
  outfile << "; ";
  RUNID.print(outfile);
  outfile << "; Output file for the prey " << stockname;

  if (printtimeid == 0)
    outfile << "\n; Printing the following information at the end of each timestep";
  else
    outfile << "\n; Printing the following information at the start of each timestep";

  outfile << "\n; year-step-area-age-length-number consumed-biomass consumed"
    << "-number consumed by length-biomass consumed by length\n";
  outfile.flush();
}

StockPreyFullPrinter::~StockPreyFullPrinter() {
  outfile.close();
  outfile.clear();
  delete preyinfo;
  delete[] stockname;
}

void StockPreyFullPrinter::setStock(StockPtrVector& stockvec) {
  CharPtrVector stocknames(1, stockname);
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
    handle.logWarning("Error in stockpreyfullprinter - failed to match stocks");
    for (i = 0; i < stocks.Size(); i++)
      handle.logWarning("Error in stockpreyfullprinter - found stock", stocks[i]->Name());
    for (i = 0; i < stocknames.Size(); i++)
      handle.logWarning("Error in stockpreyfullprinter - looking for stock", stocknames[i]);
    exit(EXIT_FAILURE);
  }

  areas = stocks[0]->Areas();
  outerareas.resize(areas.Size(), 0);
  for (i = 0; i < outerareas.Size(); i++)
    outerareas[i] = stocks[0]->getPrintArea(stocks[0]->areaNum(areas[i]));

  //Here comes some code that is only useful when handling one stock.
  if (stocks[0]->isEaten())
    preyinfo = new StockPreyStdInfo((StockPrey*)stocks[0]->returnPrey(), areas);
  else
    handle.logFailure("Error in stockpreyfullprinter - stock is not a prey");
}

void StockPreyFullPrinter::Print(const TimeClass* const TimeInfo, int printtime) {

  if ((!AAT.AtCurrentTime(TimeInfo)) || (printtime != printtimeid))
    return;

  int a, age, l, p, w;
  const LengthGroupDivision* LgrpDiv = preyinfo->returnPreyLengthGroupDiv();

  if (precision == 0) {
    p = printprecision;
    w = printwidth;
  } else {
    p = precision;
    w = precision + 4;
  }

  for (a = 0; a < areas.Size(); a++) {
    preyinfo->Sum(TimeInfo, areas[a]);
    const BandMatrix& Nbyageandl = preyinfo->NconsumptionByAgeAndLength(areas[a]);

    for (age = Nbyageandl.minAge(); age <= Nbyageandl.maxAge(); age++)
      for (l = 0; l < LgrpDiv->numLengthGroups(); l++) {
        outfile << setw(lowwidth) << TimeInfo->CurrentYear() << sep << setw(lowwidth)
          << TimeInfo->CurrentStep() << sep << setw(lowwidth) << outerareas[a] << sep
          << setw(lowwidth) << age << sep << setprecision(smallprecision)
          << setw(smallwidth) << LgrpDiv->meanLength(l) << sep;

        //JMB crude filter to remove the 'silly' values from the output
        if ((preyinfo->NconsumptionByAgeAndLength(areas[a])[age][l] < rathersmall)
           || (preyinfo->BconsumptionByAgeAndLength(areas[a])[age][l] < rathersmall)
           || (preyinfo->NconsumptionByLength(areas[a])[l] < rathersmall)
           || (preyinfo->BconsumptionByLength(areas[a])[l] < rathersmall))

          outfile << setw(w) << 0 << sep << setw(w) << 0
            << sep << setw(w) << 0 << sep << setw(w) << 0 << endl;

        else
          outfile << setprecision(p) << setw(w) << preyinfo->NconsumptionByAgeAndLength(areas[a])[age][l] << sep
            << setprecision(p) << setw(w) << preyinfo->BconsumptionByAgeAndLength(areas[a])[age][l] << sep
            << setprecision(p) << setw(w) << preyinfo->NconsumptionByLength(areas[a])[l] << sep
            << setprecision(p) << setw(w) << preyinfo->BconsumptionByLength(areas[a])[l] << endl;

      }
  }
  outfile.flush();
}
