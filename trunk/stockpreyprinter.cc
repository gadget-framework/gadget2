#include "stockpreyprinter.h"
#include "conversionindex.h"
#include "areatime.h"
#include "readfunc.h"
#include "mathfunc.h"
#include "errorhandler.h"
#include "preyptrvector.h"
#include "stockprey.h"
#include "readword.h"
#include "readaggregation.h"
#include "gadget.h"
#include "runid.h"
#include "global.h"

StockPreyPrinter::StockPreyPrinter(CommentStream& infile, const TimeClass* const TimeInfo)
  : Printer(STOCKPREYPRINTER), LgrpDiv(0), aggregator(0), alptr(0) {

  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  int i, j;

  //read in the stocknames
  i = 0;
  infile >> text >> ws;
  if (strcasecmp(text, "preynames") != 0)
    handle.logFileUnexpected(LOGFAIL, "preynames", text);
  infile >> text >> ws;
  while (!infile.eof() && (strcasecmp(text, "areaaggfile") != 0)) {
    preynames.resize(new char[strlen(text) + 1]);
    strcpy(preynames[i++], text);
    infile >> text >> ws;
  }
  if (preynames.Size() == 0)
    handle.logFileMessage(LOGFAIL, "\nError in stockpreyprinter - failed to read preys");
  handle.logMessage(LOGMESSAGE, "Read stock data - number of preys", preynames.Size());

  //read in area aggregation from file
  filename = new char[MaxStrLength];
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

  //Finished reading from infile.
  LgrpDiv = new LengthGroupDivision(lengths);
  if (LgrpDiv->Error())
    handle.logFileMessage(LOGFAIL, "\nError in stockpreyprinter - failed to create length group");

  //Open the printfile
  readWordAndValue(infile, "printfile", filename);
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
    handle.logFileMessage(LOGFAIL, "\nError in stockpreyprinter - invalid value of precision");

  if (strcasecmp(text, "printatstart") == 0)
    infile >> printtimeid >> ws >> text >> ws;
  else
    printtimeid = 0;

  if (printtimeid != 0 && printtimeid != 1)
    handle.logFileMessage(LOGFAIL, "\nError in stockpreyprinter - invalid value of printatstart");

  if (strcasecmp(text, "yearsandsteps") != 0)
    handle.logFileUnexpected(LOGFAIL, "yearsandsteps", text);
  if (!AAT.readFromFile(infile, TimeInfo))
    handle.logFileMessage(LOGFAIL, "\nError in stockpreyprinter - wrong format for yearsandsteps");

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
  outfile << "; Output file for the following preys";
  for (i = 0; i < preynames.Size(); i++)
    outfile << sep << preynames[i];

  if (printtimeid == 0)
    outfile << "\n; Printing the following information at the end of each timestep";
  else
    outfile << "\n; Printing the following information at the start of each timestep";

  outfile << "\n; year-step-area-age-length-number consumed-biomass consumed\n";
  outfile.flush();
}

void StockPreyPrinter::setPrey(PreyPtrVector& preyvec, const AreaClass* const Area) {
  PreyPtrVector preys;
  delete aggregator;
  int i, j, k, found, minage, maxage;

  for (i = 0; i < preyvec.Size(); i++)
    for (j = 0; j < preynames.Size(); j++)
      if (strcasecmp(preyvec[i]->getName(), preynames[j]) == 0)
        preys.resize(preyvec[i]);

  if (preys.Size() != preynames.Size()) {
    handle.logMessage(LOGWARN, "Error in stockpreyprinter - failed to match preys");
    for (i = 0; i < preys.Size(); i++)
      handle.logMessage(LOGWARN, "Error in stockpreyprinter - found prey", preys[i]->getName());
    for (i = 0; i < preynames.Size(); i++)
      handle.logMessage(LOGWARN, "Error in stockpreyprinter - looking for prey", preynames[i]);
    handle.logMessage(LOGFAIL, ""); //JMB this will exit gadget
  }

  for (i = 0; i < preys.Size(); i++)
    for (j = 0; j < preys.Size(); j++)
      if ((strcasecmp(preys[i]->getName(), preys[j]->getName()) == 0) && (i != j))
        handle.logMessage(LOGFAIL, "Error in stockpreyprinter - repeated prey", preys[i]->getName());

  //check that the preys are stocks and not otherfood
  for (i = 0; i < preys.Size(); i++)
    if (preys[i]->getType() == LENGTHPREY)
      handle.logMessage(LOGFAIL, "Error in stockpreyprinter - cannot print prey", preys[i]->getName());

  //change from outer areas to inner areas.
  for (i = 0; i < areas.Nrow(); i++)
    for (j = 0; j < areas.Ncol(i); j++)
      areas[i][j] = Area->getInnerArea(areas[i][j]);

  //check stock areas, ages and lengths
  if (handle.getLogLevel() >= LOGWARN) {
    for (j = 0; j < areas.Nrow(); j++) {
      found = 0;
      for (i = 0; i < preys.Size(); i++)
        for (k = 0; k < areas.Ncol(j); k++)
          if (preys[i]->isInArea(areas[j][k]))
            found++;
      if (found == 0)
        handle.logMessage(LOGWARN, "Warning in stockpreyprinter - prey not defined on all areas");
    }

    minage = 9999;
    maxage = -1;
    for (i = 0; i < ages.Nrow(); i++) {
      for (j = 0; j < ages.Ncol(i); j++) {
        minage = min(ages[i][j], minage);
        maxage = max(ages[i][j], maxage);
      }
    }

    found = 0;
    for (i = 0; i < preys.Size(); i++)
      if (minage >= ((StockPrey*)preys[i])->minAge())
        found++;
    if (found == 0)
      handle.logMessage(LOGWARN, "Warning in stockpreyprinter - minimum age less than prey age");

    found = 0;
    for (i = 0; i < preys.Size(); i++)
      if (maxage <= ((StockPrey*)preys[i])->maxAge())
        found++;
    if (found == 0)
      handle.logMessage(LOGWARN, "Warning in stockpreyprinter - maximum age greater than prey age");

    found = 0;
    for (i = 0; i < preys.Size(); i++)
      if (LgrpDiv->maxLength(0) > preys[i]->getLengthGroupDiv()->minLength())
        found++;
    if (found == 0)
      handle.logMessage(LOGWARN, "Warning in stockpreyprinter - minimum length group less than prey length");

    found = 0;
    for (i = 0; i < preys.Size(); i++)
      if (LgrpDiv->minLength(LgrpDiv->numLengthGroups()) < preys[i]->getLengthGroupDiv()->maxLength())
        found++;
    if (found == 0)
      handle.logMessage(LOGWARN, "Warning in stockpreyprinter - maximum length group greater than prey length");
  }

  aggregator = new StockPreyAggregator(preys, LgrpDiv, areas, ages);
}

void StockPreyPrinter::Print(const TimeClass* const TimeInfo, int printtime) {

  if ((!AAT.atCurrentTime(TimeInfo)) || (printtime != printtimeid))
    return;

  aggregator->Sum();
  int a, age, len;

  alptr = &aggregator->getSum();
  for (a = 0; a < areas.Nrow(); a++) {
    for (age = (*alptr)[a].minAge(); age <= (*alptr)[a].maxAge(); age++) {
      for (len = (*alptr)[a].minLength(age); len < (*alptr)[a].maxLength(age); len++) {
        outfile << setw(lowwidth) << TimeInfo->getYear() << sep
          << setw(lowwidth) << TimeInfo->getStep() << sep
          << setw(printwidth) << areaindex[a] << sep << setw(printwidth)
          << ageindex[age] << sep << setw(printwidth) << lenindex[len] << sep;

        //JMB crude filter to remove the 'silly' values from the output
        if (((*alptr)[a][age][len].N < rathersmall) || ((*alptr)[a][age][len].W < 0.0))
          outfile << setw(width) << 0 << sep << setw(width) << 0 << endl;
        else
          outfile << setprecision(precision) << setw(width) << (*alptr)[a][age][len].N
            << sep << setprecision(precision) << setw(width)
            << (*alptr)[a][age][len].W * (*alptr)[a][age][len].N << endl;

      }
    }
  }
  outfile.flush();
}

StockPreyPrinter::~StockPreyPrinter() {
  outfile.close();
  outfile.clear();
  delete LgrpDiv;
  delete aggregator;
  int i;
  for (i = 0; i < preynames.Size(); i++)
    delete[] preynames[i];
  for (i = 0; i < lenindex.Size(); i++)
    delete[] lenindex[i];
  for (i = 0; i < ageindex.Size(); i++)
    delete[] ageindex[i];
  for (i = 0; i < areaindex.Size(); i++)
    delete[] areaindex[i];
}
