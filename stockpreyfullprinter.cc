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

//This printer prints all the information on the consumption of the prey,
//at the level of most disaggregation.

StockPreyFullPrinter::StockPreyFullPrinter(CommentStream& infile,
  const AreaClass* const Area, const TimeClass* const TimeInfo)
  : Printer(STOCKPREYFULLPRINTER), stockname(0), preyinfo(0) {

  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  int i;

  stockname = new char[MaxStrLength];
  strncpy(stockname, "", MaxStrLength);
  readWordAndValue(infile, "stockname", stockname);

  //read in area aggregation from file
  char filename[MaxStrLength];
  strncpy(filename, "", MaxStrLength);
  ifstream datafile;
  CommentStream subdata(datafile);

  CharPtrVector areaindex;
  IntMatrix tmpareas;
  readWordAndValue(infile, "areaaggfile", filename);
  datafile.open(filename, ios::in);
  handle.checkIfFailure(datafile, filename);
  handle.Open(filename);
  i = readAggregation(subdata, tmpareas, areaindex);
  handle.Close();
  datafile.close();
  datafile.clear();

  //Check if we read correct input
  if (tmpareas.Nrow() != 1)
    handle.Message("Error - there should be only one aggregated area for stockpreyfullprinter");

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
  handle.checkIfFailure(outfile, filename);

  infile >> text >> ws;
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
  outfile << "; Output file for the prey " << stockname
    << "\n; year-step-area-age-length-number consumed-biomass consumed"
    << "-number consumed by length-biomass consumed by length\n";
  outfile.flush();

  //areaindex is not required - free up memory
  for (i = 0; i < areaindex.Size(); i++)
    delete[] areaindex[i];
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

  //check that the stock lives in the areas.
  for (i = 0; i < stocks.Size(); i++)
    for (j = 0; j < areas.Size(); j++)
      if (!stocks[i]->IsInArea(areas[j]))
        handle.logFailure("Error in stockpreyfullprinter - stocks arent defined on all areas");

  //Here comes some code that is only useful when handling one stock.
  if (stocks[0]->IsEaten())
    preyinfo = new StockPreyStdInfo((StockPrey*)stocks[0]->returnPrey(), areas);
  else
    handle.logFailure("Error in stockpreyfullprinter - stock is not a prey");
}

void StockPreyFullPrinter::Print(const TimeClass* const TimeInfo) {
  if (!AAT.AtCurrentTime(TimeInfo))
    return;
  int a, age, l;

  for (a = 0; a < areas.Size(); a++)
    preyinfo->Sum(TimeInfo, areas[a]);

  const LengthGroupDivision* LgrpDiv = preyinfo->returnPreyLengthGroupDiv();

  for (a = 0; a < areas.Size(); a++) {
    const BandMatrix& Nbyageandl = preyinfo->NconsumptionByAgeAndLength(areas[a]);
    for (age = Nbyageandl.minAge(); age <= Nbyageandl.maxAge(); age++)
      for (l = 0; l < LgrpDiv->NoLengthGroups(); l++) {
        outfile << setw(lowwidth) << TimeInfo->CurrentYear() << sep << setw(lowwidth)
          << TimeInfo->CurrentStep() << sep << setw(lowwidth) << outerareas[a] << sep
          << setw(lowwidth) << age << sep << setprecision(smallprecision)
          << setw(smallwidth) << LgrpDiv->meanLength(l) << sep;

        //JMB crude filter to remove the 'silly' values from the output
        if ((preyinfo->NconsumptionByAgeAndLength(areas[a])[age][l] < rathersmall)
           || (preyinfo->BconsumptionByAgeAndLength(areas[a])[age][l] < rathersmall)
           || (preyinfo->NconsumptionByLength(areas[a])[l] < rathersmall)
           || (preyinfo->BconsumptionByLength(areas[a])[l] < rathersmall))

          outfile << setw(printwidth) << 0 << sep << setw(printwidth) << 0
            << sep << setw(printwidth) << 0 << sep << setw(printwidth) << 0 << endl;

        else
          outfile << setprecision(printprecision) << setw(printwidth) << preyinfo->NconsumptionByAgeAndLength(areas[a])[age][l] << sep
            << setprecision(printprecision) << setw(printwidth) << preyinfo->BconsumptionByAgeAndLength(areas[a])[age][l] << sep
            << setprecision(printprecision) << setw(printwidth) << preyinfo->NconsumptionByLength(areas[a])[l] << sep
            << setprecision(printprecision) << setw(printwidth) << preyinfo->BconsumptionByLength(areas[a])[l] << endl;

      }
  }
  outfile.flush();
}
