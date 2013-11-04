#include "predatoroverprinter.h"
#include "conversionindex.h"
#include "predatoroveraggregator.h"
#include "areatime.h"
#include "readfunc.h"
#include "readword.h"
#include "readaggregation.h"
#include "errorhandler.h"
#include "predator.h"
#include "gadget.h"
#include "runid.h"
#include "global.h"

PredatorOverPrinter::PredatorOverPrinter(CommentStream& infile, const TimeClass* const TimeInfo)
  : Printer(PREDATOROVERPRINTER), predLgrpDiv(0), aggregator(0), dptr(0) {

  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  int i, j;

  //read in the predator names
  i = 0;
  infile >> text >> ws;
  if ((strcasecmp(text, "predators") != 0) && (strcasecmp(text, "predatornames") != 0))
    handle.logFileUnexpected(LOGFAIL, "predatornames", text);
  infile >> text >> ws;
  while (!infile.eof() && (strcasecmp(text, "areaaggfile") != 0)) {
    predatornames.resize(new char[strlen(text) + 1]);
    strcpy(predatornames[i++], text);
    infile >> text >> ws;
  }
  if (predatornames.Size() == 0)
    handle.logFileMessage(LOGFAIL, "\nError in predatoroverprinter - failed to read predators");
  handle.logMessage(LOGMESSAGE, "Read predator data - number of predators", predatornames.Size());

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
  predLgrpDiv = new LengthGroupDivision(lengths);
  if (predLgrpDiv->Error())
    handle.logFileMessage(LOGFAIL, "\nError in predatoroverprinter - failed to create length group");

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
    handle.logFileMessage(LOGFAIL, "\nError in predatoroverprinter - invalid value of precision");

  if (strcasecmp(text, "printatstart") == 0)
    infile >> printtimeid >> ws >> text >> ws;
  else
    printtimeid = 0;

  if (printtimeid != 0 && printtimeid != 1)
    handle.logFileMessage(LOGFAIL, "\nError in predatoroverprinter - invalid value of printatstart");

  if (strcasecmp(text, "yearsandsteps") != 0)
    handle.logFileUnexpected(LOGFAIL, "yearsandsteps", text);
  if (!AAT.readFromFile(infile, TimeInfo))
    handle.logFileMessage(LOGFAIL, "\nError in predatoroverprinter - wrong format for yearsandsteps");

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
  outfile << "; Predator overconsumption output file for the following predators";
  for (i = 0; i < predatornames.Size(); i++)
    outfile << sep << predatornames[i];

  if (printtimeid == 0)
    outfile << "\n; Printing the following information at the end of each timestep";
  else
    outfile << "\n; Printing the following information at the start of each timestep";

  outfile << "\n; year-step-area-length-overconsumption biomass\n";
  outfile.flush();
}

void PredatorOverPrinter::setPredator(PredatorPtrVector& predatorvec, const AreaClass* const Area) {
  PredatorPtrVector predators;
  delete aggregator;
  int i, j, k, found;

  for (i = 0; i < predatorvec.Size(); i++)
    for (j = 0; j < predatornames.Size(); j++)
      if (strcasecmp(predatorvec[i]->getName(), predatornames[j]) == 0)
        predators.resize(predatorvec[i]);

  if (predators.Size() != predatornames.Size()) {
    handle.logMessage(LOGWARN, "Error in predatoroverprinter - failed to match predators");
    for (i = 0; i < predatorvec.Size(); i++)
      handle.logMessage(LOGWARN, "Error in predatoroverprinter - found predator", predatorvec[i]->getName());
    for (i = 0; i < predatornames.Size(); i++)
      handle.logMessage(LOGWARN, "Error in predatoroverprinter - looking for predator", predatornames[i]);
    handle.logMessage(LOGFAIL, ""); //JMB this will exit gadget
  }

  for (i = 0; i < predators.Size(); i++)
    for (j = 0; j < predators.Size(); j++)
      if ((strcasecmp(predators[i]->getName(), predators[j]->getName()) == 0) && (i != j))
        handle.logMessage(LOGFAIL, "Error in predatoroverprinter - repeated predator", predators[i]->getName());

  //change from outer areas to inner areas.
  for (i = 0; i < areas.Nrow(); i++)
    for (j = 0; j < areas.Ncol(i); j++)
      areas[i][j] = Area->getInnerArea(areas[i][j]);

  //check predator areas and lengths
  if (handle.getLogLevel() >= LOGWARN) {
    for (j = 0; j < areas.Nrow(); j++) {
      found = 0;
      for (i = 0; i < predators.Size(); i++)
        for (k = 0; k < areas.Ncol(j); k++)
          if (predators[i]->isInArea(areas[j][k]))
            found++;
      if (found == 0)
        handle.logMessage(LOGWARN, "Warning in predatoroverprinter - predator not defined on all areas");
    }

    found = 0;
    for (i = 0; i < predators.Size(); i++)
      if (predLgrpDiv->maxLength(0) > predators[i]->getLengthGroupDiv()->minLength())
        found++;
    if (found == 0)
      handle.logMessage(LOGWARN, "Warning in predatoroverprinter - minimum length group less than predator length");

    found = 0;
    for (i = 0; i < predators.Size(); i++)
      if (predLgrpDiv->minLength(predLgrpDiv->numLengthGroups()) < predators[i]->getLengthGroupDiv()->maxLength())
        found++;
    if (found == 0)
      handle.logMessage(LOGWARN, "Warning in predatoroverprinter - maximum length group greater than predator length");
  }

  aggregator = new PredatorOverAggregator(predators, areas, predLgrpDiv);
}

void PredatorOverPrinter::Print(const TimeClass* const TimeInfo, int printtime) {

  if ((!AAT.atCurrentTime(TimeInfo)) || (printtime != printtimeid))
    return;

  aggregator->Sum();
  dptr = &aggregator->getSum();
  int a, len;

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

PredatorOverPrinter::~PredatorOverPrinter() {
  outfile.close();
  outfile.clear();
  delete predLgrpDiv;
  delete aggregator;
  int i;
  for (i = 0; i < predatornames.Size(); i++)
    delete[] predatornames[i];
  for (i = 0; i < areaindex.Size(); i++)
    delete[] areaindex[i];
  for (i = 0; i < lenindex.Size(); i++)
    delete[] lenindex[i];
}
