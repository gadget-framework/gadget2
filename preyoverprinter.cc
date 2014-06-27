#include "preyoverprinter.h"
#include "conversionindex.h"
#include "preyoveraggregator.h"
#include "areatime.h"
#include "readfunc.h"
#include "readword.h"
#include "readaggregation.h"
#include "errorhandler.h"
#include "prey.h"
#include "gadget.h"
#include "runid.h"
#include "global.h"

PreyOverPrinter::PreyOverPrinter(CommentStream& infile, const TimeClass* const TimeInfo)
  : Printer(PREYOVERPRINTER), preyLgrpDiv(0), aggregator(0), dptr(0) {

  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  int i, j;

  //read in the prey names
  i = 0;
  infile >> text >> ws;
  if ((strcasecmp(text, "preys") != 0) && (strcasecmp(text, "preynames") != 0))
    handle.logFileUnexpected(LOGFAIL, "preynames", text);
  infile >> text >> ws;
  while (!infile.eof() && (strcasecmp(text, "areaaggfile") != 0)) {
    preynames.resize(new char[strlen(text) + 1]);
    strcpy(preynames[i++], text);
    infile >> text >> ws;
  }
  if (preynames.Size() == 0)
    handle.logFileMessage(LOGFAIL, "\nError in preyoverprinter - failed to read preys");
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
  preyLgrpDiv = new LengthGroupDivision(lengths);
  if (preyLgrpDiv->Error())
    handle.logFileMessage(LOGFAIL, "\nError in preyoverprinter - failed to create length group");

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
    handle.logFileMessage(LOGFAIL, "\nError in preyoverprinter - invalid value of precision");

  if (strcasecmp(text, "printatstart") == 0)
    infile >> printtimeid >> ws >> text >> ws;
  else
    printtimeid = 0;

  if (printtimeid != 0 && printtimeid != 1)
    handle.logFileMessage(LOGFAIL, "\nError in preyoverprinter - invalid value of printatstart");

  if (strcasecmp(text, "yearsandsteps") != 0)
    handle.logFileUnexpected(LOGFAIL, "yearsandsteps", text);
  if (!AAT.readFromFile(infile, TimeInfo))
    handle.logFileMessage(LOGFAIL, "\nError in preyoverprinter - wrong format for yearsandsteps");

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
  outfile << "; Prey overconsumption output file for the following preys";
  for (i = 0; i < preynames.Size(); i++)
    outfile << sep << preynames[i];

  if (printtimeid == 0)
    outfile << "\n; Printing the following information at the end of each timestep";
  else
    outfile << "\n; Printing the following information at the start of each timestep";

  outfile << "\n; year-step-area-length-overconsumption biomass\n";
  outfile.flush();
}

void PreyOverPrinter::setPrey(PreyPtrVector& preyvec, const AreaClass* const Area) {
  PreyPtrVector preys;
  delete aggregator;
  int i, j, k, found;

  for (i = 0; i < preyvec.Size(); i++)
    for (j = 0; j < preynames.Size(); j++)
      if (strcasecmp(preyvec[i]->getName(), preynames[j]) == 0)
        preys.resize(preyvec[i]);

  if (preys.Size() != preynames.Size()) {
    handle.logMessage(LOGWARN, "Error in preyoverprinter - failed to match preys");
    for (i = 0; i < preyvec.Size(); i++)
      handle.logMessage(LOGWARN, "Error in preyoverprinter - found prey", preyvec[i]->getName());
    for (i = 0; i < preynames.Size(); i++)
      handle.logMessage(LOGWARN, "Error in preyoverprinter - looking for prey", preynames[i]);
    handle.logMessage(LOGFAIL, ""); //JMB this will exit gadget
  }

  for (i = 0; i < preys.Size(); i++)
    for (j = 0; j < preys.Size(); j++)
      if ((strcasecmp(preys[i]->getName(), preys[j]->getName()) == 0) && (i != j))
        handle.logMessage(LOGFAIL, "Error in preyoverprinter - repeated prey", preys[i]->getName());

  //change from outer areas to inner areas.
  for (i = 0; i < areas.Nrow(); i++)
    for (j = 0; j < areas.Ncol(i); j++)
      areas[i][j] = Area->getInnerArea(areas[i][j]);

  //check prey areas and lengths
  if (handle.getLogLevel() >= LOGWARN) {
    for (j = 0; j < areas.Nrow(); j++) {
      found = 0;
      for (i = 0; i < preys.Size(); i++)
        for (k = 0; k < areas.Ncol(j); k++)
          if (preys[i]->isInArea(areas[j][k]))
            found++;
      if (found == 0)
        handle.logMessage(LOGWARN, "Warning in preyoverprinter - prey not defined on all areas");
    }

    found = 0;
    for (i = 0; i < preys.Size(); i++)
      if (preyLgrpDiv->maxLength(0) > preys[i]->getLengthGroupDiv()->minLength())
        found++;
    if (found == 0)
      handle.logMessage(LOGWARN, "Warning in preyoverprinter - minimum length group less than prey length");

    found = 0;
    for (i = 0; i < preys.Size(); i++)
      if (preyLgrpDiv->minLength(preyLgrpDiv->numLengthGroups()) < preys[i]->getLengthGroupDiv()->maxLength())
        found++;
    if (found == 0)
      handle.logMessage(LOGWARN, "Warning in preyoverprinter - maximum length group greater than prey length");
  }

  aggregator = new PreyOverAggregator(preys, areas, preyLgrpDiv);
}

void PreyOverPrinter::Print(const TimeClass* const TimeInfo, int printtime) {

  if ((!AAT.atCurrentTime(TimeInfo)) || (printtime != printtimeid))
    return;

  aggregator->Sum();
  dptr = &aggregator->getSum();
  int a, len, p, w;

  for (a = 0; a < areas.Nrow(); a++) {
    for (len = 0; len < dptr->Ncol(a); len++) {
      outfile << setw(lowwidth) << TimeInfo->getYear() << sep
        << setw(lowwidth) << TimeInfo->getStep() << sep
        << setw(printwidth) << areaindex[a] << sep
        << setw(printwidth) << lenindex[len] << sep;

      //JMB crude filter to remove the 'silly' values from the output
      if ((*dptr)[a][len] < rathersmall)
        outfile << setw(width) << 0 << endl;
      else
        outfile << setprecision(precision) << setw(width) << (*dptr)[a][len] << endl;
    }
  }
  outfile.flush();
}

PreyOverPrinter::~PreyOverPrinter() {
  outfile.close();
  outfile.clear();
  delete preyLgrpDiv;
  delete aggregator;
  int i;
  for (i = 0; i < preynames.Size(); i++)
    delete[] preynames[i];
  for (i = 0; i < areaindex.Size(); i++)
    delete[] areaindex[i];
  for (i = 0; i < lenindex.Size(); i++)
    delete[] lenindex[i];
}
