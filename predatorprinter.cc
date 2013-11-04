#include "predatorprinter.h"
#include "conversionindex.h"
#include "predatoraggregator.h"
#include "areatime.h"
#include "readfunc.h"
#include "readword.h"
#include "readaggregation.h"
#include "errorhandler.h"
#include "predator.h"
#include "prey.h"
#include "gadget.h"
#include "runid.h"
#include "global.h"

PredatorPrinter::PredatorPrinter(CommentStream& infile, const TimeClass* const TimeInfo)
  : Printer(PREDATORPRINTER), predLgrpDiv(0), preyLgrpDiv(0), aggregator(0), dptr(0) {

  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  int i, j;

  //default value is to print biomass
  biomass = 1;

  //read in the predator names
  i = 0;
  infile >> text >> ws;
  if ((strcasecmp(text, "predators") != 0) && (strcasecmp(text, "predatornames") != 0))
    handle.logFileUnexpected(LOGFAIL, "predatornames", text);
  infile >> text >> ws;
  while (!infile.eof() && ((strcasecmp(text, "preys") != 0) && (strcasecmp(text, "preynames") != 0))) {
    predatornames.resize(new char[strlen(text) + 1]);
    strcpy(predatornames[i++], text);
    infile >> text >> ws;
  }
  if (predatornames.Size() == 0)
    handle.logFileMessage(LOGFAIL, "\nError in predatorprinter - failed to read predators");
  handle.logMessage(LOGMESSAGE, "Read predator data - number of predators", predatornames.Size());

  //read in the prey names
  i = 0;
  infile >> text >> ws;
  while (!infile.eof() && (strcasecmp(text, "areaaggfile") != 0)) {
    preynames.resize(new char[strlen(text) + 1]);
    strcpy(preynames[i++], text);
    infile >> text >> ws;
  }
  if (preynames.Size() == 0)
    handle.logFileMessage(LOGFAIL, "\nError in predatorprinter - failed to read preys");
  handle.logMessage(LOGMESSAGE, "Read prey data - number of preys", preynames.Size());

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

  //read in predator length aggregation from file
  DoubleVector predlengths;
  readWordAndValue(infile, "predlenaggfile", filename);
  datafile.open(filename, ios::in);
  handle.checkIfFailure(datafile, filename);
  handle.Open(filename);
  i = readLengthAggregation(subdata, predlengths, predlenindex);
  handle.Close();
  datafile.close();
  datafile.clear();

  //read in prey length aggregation from file
  DoubleVector preylengths;
  readWordAndValue(infile, "preylenaggfile", filename);
  datafile.open(filename, ios::in);
  handle.checkIfFailure(datafile, filename);
  handle.Open(filename);
  i = readLengthAggregation(subdata, preylengths, preylenindex);
  handle.Close();
  datafile.close();
  datafile.clear();

  //finished reading from infile
  predLgrpDiv = new LengthGroupDivision(predlengths);
  if (predLgrpDiv->Error())
    handle.logFileMessage(LOGFAIL, "\nError in predatorprinter - failed to create predator length group");
  preyLgrpDiv = new LengthGroupDivision(preylengths);
  if (preyLgrpDiv->Error())
    handle.logFileMessage(LOGFAIL, "\nError in predatorprinter - failed to create prey length group");

  char c = infile.peek();
  if ((c == 'b') || (c == 'B'))
    readWordAndVariable(infile, "biomass", biomass);
  if (biomass != 0 && biomass != 1)
    handle.logFileMessage(LOGFAIL, "\nError in predatorprinter - biomass must be 0 or 1");

  //open the printfile
  readWordAndValue(infile, "printfile", filename);
  outfile.open(filename, ios::out);
  handle.checkIfFailure(outfile, filename);

  infile >> text >> ws;
  if (strcasecmp(text, "precision") == 0) {
    infile >> precision >> ws >> text >> ws;
    width = precision + 4;
  } else {
    //use default values
    precision = largeprecision;
    width = largewidth;
  }

  if (precision < 0)
    handle.logFileMessage(LOGFAIL, "\nError in predatorprinter - invalid value of precision");

  if (strcasecmp(text, "printatstart") == 0)
    infile >> printtimeid >> ws >> text >> ws;
  else
    printtimeid = 0;

  if (printtimeid != 0 && printtimeid != 1)
    handle.logFileMessage(LOGFAIL, "\nError in predatorprinter - invalid value of printatstart");

  if (strcasecmp(text, "yearsandsteps") != 0)
    handle.logFileUnexpected(LOGFAIL, "yearsandsteps", text);
  if (!AAT.readFromFile(infile, TimeInfo))
    handle.logFileMessage(LOGFAIL, "\nError in predatorprinter - wrong format for yearsandsteps");

  //prepare for next printfile component
  infile >> ws;
  if (!infile.eof()) {
    infile >> text >> ws;
    if (strcasecmp(text, "[component]") != 0)
      handle.logFileUnexpected(LOGFAIL, "[component]", text);
  }

  //finished initializing - now print first lines
  outfile << "; ";
  RUNID.Print(outfile);
  outfile << "; Predation output file for the following predators";
  for (i = 0; i < predatornames.Size(); i++)
    outfile << sep << predatornames[i];
  outfile << "\n; Consuming the following preys";
  for (i = 0; i < preynames.Size(); i++)
    outfile << sep << preynames[i];

  if (printtimeid == 0)
    outfile << "\n; Printing the following information at the end of each timestep";
  else
    outfile << "\n; Printing the following information at the start of each timestep";

  if (biomass)
    outfile << "\n; year-step-area-pred length-prey length-biomass consumed\n";
  else
    outfile << "\n; year-step-area-pred length-prey length-number consumed\n";

  outfile.flush();
}

void PredatorPrinter::setPredAndPrey(PredatorPtrVector& predatorvec,
  PreyPtrVector& preyvec, const AreaClass* const Area) {

  PredatorPtrVector predators;
  PreyPtrVector preys;
  int i, j, k, found;
  delete aggregator;

  for (i = 0; i < predatorvec.Size(); i++)
    for (j = 0; j < predatornames.Size(); j++)
      if (strcasecmp(predatorvec[i]->getName(), predatornames[j]) == 0)
        predators.resize(predatorvec[i]);

  for (i = 0; i < preyvec.Size(); i++)
    for (j = 0; j < preynames.Size(); j++)
      if (strcasecmp(preyvec[i]->getName(), preynames[j]) == 0)
        preys.resize(preyvec[i]);

  //change from outer areas to inner areas
  for (i = 0; i < areas.Nrow(); i++)
    for (j = 0; j < areas.Ncol(i); j++)
      areas[i][j] = Area->getInnerArea(areas[i][j]);

  //check predator and prey areas and length groups
  if (handle.getLogLevel() >= LOGWARN) {
    for (j = 0; j < areas.Nrow(); j++) {
      found = 0;
      for (i = 0; i < predators.Size(); i++)
        for (k = 0; k < areas.Ncol(j); k++)
          if (predators[i]->isInArea(areas[j][k]))
            found++;
      if (found == 0)
        handle.logMessage(LOGWARN, "Warning in predatorprinter - predators not defined on all areas");
    }

    found = 0;
    for (i = 0; i < predators.Size(); i++)
      if (predLgrpDiv->maxLength(0) > predators[i]->getLengthGroupDiv()->minLength())
        found++;
    if (found == 0)
      handle.logMessage(LOGWARN, "Warning in predatorprinter - minimum length group less than predator length");

    found = 0;
    for (i = 0; i < predators.Size(); i++)
      if (predLgrpDiv->minLength(predLgrpDiv->numLengthGroups()) < predators[i]->getLengthGroupDiv()->maxLength())
        found++;
    if (found == 0)
      handle.logMessage(LOGWARN, "Warning in predatorprinter - maximum length group greater than predator length");

    for (j = 0; j < areas.Nrow(); j++) {
      found = 0;
      for (i = 0; i < preys.Size(); i++)
        for (k = 0; k < areas.Ncol(j); k++)
          if (preys[i]->isInArea(areas[j][k]))
            found++;
      if (found == 0)
        handle.logMessage(LOGWARN, "Warning in predatorprinter - preys not defined on all areas");
    }

    found = 0;
    for (i = 0; i < preys.Size(); i++)
      if (preyLgrpDiv->maxLength(0) > preys[i]->getLengthGroupDiv()->minLength())
        found++;
    if (found == 0)
      handle.logMessage(LOGWARN, "Warning in predatorprinter - minimum length group less than prey length");

    found = 0;
    for (i = 0; i < preys.Size(); i++)
      if (preyLgrpDiv->minLength(preyLgrpDiv->numLengthGroups()) < preys[i]->getLengthGroupDiv()->maxLength())
        found++;
    if (found == 0)
      handle.logMessage(LOGWARN, "Warning in predatorprinter - maximum length group greater than prey length");
  }

  if (predators.Size() != predatornames.Size()) {
    handle.logMessage(LOGWARN, "Error in predatorprinter - failed to match predators");
    for (i = 0; i < predatorvec.Size(); i++)
      handle.logMessage(LOGWARN, "Error in predatorprinter - found predator", predatorvec[i]->getName());
    for (i = 0; i < predatornames.Size(); i++)
      handle.logMessage(LOGWARN, "Error in predatorprinter - looking for predator", predatornames[i]);
    handle.logMessage(LOGFAIL, ""); //JMB this will exit gadget
  }

  if (preys.Size() != preynames.Size()) {
    handle.logMessage(LOGWARN, "Error in predatorprinter - failed to match preys");
    for (i = 0; i < preyvec.Size(); i++)
      handle.logMessage(LOGWARN, "Error in predatorprinter - found prey", preyvec[i]->getName());
    for (i = 0; i < preynames.Size(); i++)
      handle.logMessage(LOGWARN, "Error in predatorprinter - looking for prey", preynames[i]);
    handle.logMessage(LOGFAIL, ""); //JMB this will exit gadget
  }

  for (i = 0; i < predators.Size(); i++)
    for (j = 0; j < predators.Size(); j++)
      if ((strcasecmp(predators[i]->getName(), predators[j]->getName()) == 0) && (i != j))
        handle.logMessage(LOGFAIL, "Error in predatorprinter - repeated predator", predators[i]->getName());

  for (i = 0; i < preys.Size(); i++)
    for (j = 0; j < preys.Size(); j++)
      if ((strcasecmp(preys[i]->getName(), preys[j]->getName()) == 0) && (i != j))
        handle.logMessage(LOGFAIL, "Error in predatorprinter - repeated prey", preys[i]->getName());

  aggregator = new PredatorAggregator(predators, preys, areas, predLgrpDiv, preyLgrpDiv);
}

void PredatorPrinter::Print(const TimeClass* const TimeInfo, int printtime) {

  if ((!AAT.atCurrentTime(TimeInfo)) || (printtime != printtimeid))
    return;

  if (biomass)
    aggregator->Sum();
  else
    aggregator->NumberSum();

  int a, predl, preyl;
  for (a = 0; a < areas.Nrow(); a++) {
    dptr = aggregator->getSum()[a];
    for (predl = 0; predl < dptr->Nrow(); predl++) {
      for (preyl = 0; preyl < dptr->Ncol(predl); preyl++) {
        outfile << setw(lowwidth) << TimeInfo->getYear() << sep
          << setw(lowwidth) << TimeInfo->getStep() << sep
          << setw(printwidth) << areaindex[a] << sep
          << setw(printwidth) << predlenindex[predl] << sep
          << setw(printwidth) << preylenindex[preyl] << sep;

        //JMB crude filter to remove the 'silly' values from the output
        if ((*dptr)[predl][preyl] < rathersmall)
          outfile << setw(width) << 0 << endl;
        else
          outfile << setprecision(precision) << setw(width) << (*dptr)[predl][preyl] << endl;
      }
    }
  }
  outfile.flush();
}

PredatorPrinter::~PredatorPrinter() {
  outfile.close();
  outfile.clear();
  delete predLgrpDiv;
  delete preyLgrpDiv;
  delete aggregator;
  int i;
  for (i = 0; i < predatornames.Size(); i++)
    delete[] predatornames[i];
  for (i = 0; i < preynames.Size(); i++)
    delete[] preynames[i];
  for (i = 0; i < areaindex.Size(); i++)
    delete[] areaindex[i];
  for (i = 0; i < preylenindex.Size(); i++)
    delete[] preylenindex[i];
  for (i = 0; i < predlenindex.Size(); i++)
    delete[] predlenindex[i];
}
