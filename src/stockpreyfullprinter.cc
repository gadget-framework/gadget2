#include "stockpreyfullprinter.h"
#include "areatime.h"
#include "errorhandler.h"
#include "conversionindex.h"
#include "stockpreyaggregator.h"
#include "preyptrvector.h"
#include "stockprey.h"
#include "readword.h"
#include "gadget.h"
#include "runid.h"
#include "global.h"

StockPreyFullPrinter::StockPreyFullPrinter(CommentStream& infile, const TimeClass* const TimeInfo)
  : Printer(STOCKPREYFULLPRINTER), preyname(0), LgrpDiv(0) {

  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);

  preyname = new char[MaxStrLength];
  strncpy(preyname, "", MaxStrLength);
  //JMB - changed to read either preyname or stockname
  infile >> ws;
  char c = infile.peek();
  if ((c == 's') || (c == 'S'))
    readWordAndValue(infile, "stockname", preyname);
  else
    readWordAndValue(infile, "preyname", preyname);

  //JMB - removed the need to read in the area aggregation file
  infile >> text >> ws;
  if (strcasecmp(text, "areaaggfile") == 0) {
    infile >> text >> ws;
    handle.logMessage(LOGWARN, "Warning in stockpreyfullprinter - area aggregation file ignored");
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
    precision = printprecision;
    width = printwidth;
  }

  if (precision < 0)
    handle.logFileMessage(LOGFAIL, "\nError in stockpreyfullprinter - invalid value of precision");

  if (strcasecmp(text, "printatstart") == 0)
    infile >> printtimeid >> ws >> text >> ws;
  else
    printtimeid = 0;

  if (printtimeid != 0 && printtimeid != 1)
    handle.logFileMessage(LOGFAIL, "\nError in stockpreyfullprinter - invalid value of printatstart");

  if (strcasecmp(text, "yearsandsteps") != 0)
    handle.logFileUnexpected(LOGFAIL, "yearsandsteps", text);
  if (!AAT.readFromFile(infile, TimeInfo))
    handle.logFileMessage(LOGFAIL, "\nError in stockpreyfullprinter - wrong format for yearsandsteps");

  //prepare for next printfile component
  infile >> ws;
  if (!infile.eof()) {
    infile >> text >> ws;
    if (strcasecmp(text, "[component]") != 0)
      handle.logFileUnexpected(LOGFAIL, "[component]", text);
  }

  //finished initializing. Now print first line.
  outfile << "; ";
  RUNID.Print(outfile);
  outfile << "; Output file for the prey " << preyname;

  if (printtimeid == 0)
    outfile << "\n; Printing the following information at the end of each timestep";
  else
    outfile << "\n; Printing the following information at the start of each timestep";

  // BTHE compatibility with Rgadget::read.gadget.file
  outfile << "\n; -- data --";

  outfile << "\n; year step area age length number_consumed biomass_consumed\n";
  outfile.flush();
}

StockPreyFullPrinter::~StockPreyFullPrinter() {
  outfile.close();
  outfile.clear();
  delete LgrpDiv;
  delete aggregator;
  delete[] preyname;
}

void StockPreyFullPrinter::setPrey(PreyPtrVector& preyvec, const AreaClass* const Area) {
  PreyPtrVector preys;
  int i, maxage;

  for (i = 0; i < preyvec.Size(); i++)
    if (strcasecmp(preyvec[i]->getName(), preyname) == 0)
      preys.resize(preyvec[i]);

  if (preys.Size() != 1) {
    handle.logMessage(LOGWARN, "Error in stockpreyfullprinter - failed to match preys");
    for (i = 0; i < preys.Size(); i++)
      handle.logMessage(LOGWARN, "Error in stockpreyfullprinter - found prey", preys[i]->getName());
    handle.logMessage(LOGFAIL, "Error in stockpreyfullprinter - looking for prey", preyname);
  }

  //check that the prey is a stock and not otherfood
  if (preys[0]->getType() == LENGTHPREY)
    handle.logMessage(LOGFAIL, "Error in stockpreyfullprinter - cannot print prey", preys[i]->getName());

  //JMB need to construct a matrix of all the areas and ages for the prey
  IntVector areas = preys[0]->getAreas();
  IntMatrix areamatrix(areas.Size(), 1, 0);
  for (i = 0; i < areamatrix.Nrow(); i++)
    areamatrix[i][0] = areas[i];

  outerareas.resize(areas.Size(), 0);
  for (i = 0; i < outerareas.Size(); i++)
    outerareas[i] = Area->getModelArea(areas[i]);

  minage = ((StockPrey*)preys[0])->minAge();
  maxage = ((StockPrey*)preys[0])->maxAge();
  IntMatrix agematrix(maxage - minage + 1, 1, 0);
  for (i = 0; i < agematrix.Nrow(); i++)
    agematrix[i][0] = i + minage;

  LgrpDiv = new LengthGroupDivision(*preys[0]->getLengthGroupDiv());
  if (LgrpDiv->Error())
    handle.logMessage(LOGFAIL, "Error in stockpreyfullprinter - failed to create length group");
  aggregator = new StockPreyAggregator(preys, LgrpDiv, areamatrix, agematrix);
}

void StockPreyFullPrinter::Print(const TimeClass* const TimeInfo, int printtime) {

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
          << age + minage << sep << setw(lowwidth) << LgrpDiv->meanLength(len) << sep;

        //JMB crude filter to remove the 'silly' values from the output
        if (((*alptr)[a][age][len].N < rathersmall) || ((*alptr)[a][age][len].W < 0.0))
          outfile << setw(width) << 0 << sep << setw(width) << 0 << endl;
        else
          outfile << setprecision(precision) << setw(width) << (*alptr)[a][age][len].N << sep
            << setprecision(precision) << setw(width) << (*alptr)[a][age][len].W * (*alptr)[a][age][len].N << endl;

      }
    }
  }
  outfile.flush();
}
