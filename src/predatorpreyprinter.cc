#include "predatorpreyprinter.h"
#include "predatorpreyaggregator.h"
#include "areatime.h"
#include "readword.h"
#include "readaggregation.h"
#include "errorhandler.h"
#include "predator.h"
#include "stockprey.h"
#include "gadget.h"
#include "runid.h"
#include "global.h"

PredatorPreyPrinter::PredatorPreyPrinter(CommentStream& infile, const TimeClass* const TimeInfo)
  : Printer(PREDATORPREYPRINTER), aggregator(0), alptr(0), dptr(0) {

  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);

  //read in the predator names
  int i = 0;
  infile >> text >> ws;
  if (strcasecmp(text, "predatornames") != 0)
    handle.logFileUnexpected(LOGFAIL, "predatornames", text);
  infile >> text >> ws;
  while (!infile.eof() && (strcasecmp(text, "preynames") != 0)) {
    predatornames.resize(new char[strlen(text) + 1]);
    strcpy(predatornames[i++], text);
    infile >> text >> ws;
  }
  if (predatornames.Size() == 0)
    handle.logFileMessage(LOGFAIL, "\nError in predatorpreyprinter - failed to read predators");
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
    handle.logFileMessage(LOGFAIL, "\nError in predatorpreyprinter - failed to read preys");
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

  LgrpDiv = new LengthGroupDivision(lengths);
  if (LgrpDiv->Error())
    handle.logFileMessage(LOGFAIL, "\nError in predatorpreyprinter - failed to create length group");

  //open the printfile
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
    handle.logFileMessage(LOGFAIL, "\nError in predatorpreyprinter - invalid value of precision");

  if (strcasecmp(text, "printatstart") == 0)
    infile >> printtimeid >> ws >> text >> ws;
  else
    printtimeid = 0;

  if (printtimeid != 0 && printtimeid != 1)
    handle.logFileMessage(LOGFAIL, "\nError in predatorpreyprinter - invalid value of printatstart");

  if (strcasecmp(text, "yearsandsteps") != 0)
    handle.logFileUnexpected(LOGFAIL, "yearsandsteps", text);
  if (!AAT.readFromFile(infile, TimeInfo))
    handle.logFileMessage(LOGFAIL, "\nError in predatorpreyprinter - wrong format for yearsandsteps");

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
  outfile << "\n; -- data --";
  outfile << "\n; year step area age length number_consumed biomass_consumed mortality\n";
  outfile.flush();
}

void PredatorPreyPrinter::setPredAndPrey(PredatorPtrVector& predatorvec,
  PreyPtrVector& preyvec, const AreaClass* const Area) {

  PredatorPtrVector predators;
  PreyPtrVector preys;
  int i, j, k, found, minage, maxage;
  delete aggregator;

  for (i = 0; i < predatorvec.Size(); i++)
    for (j = 0; j < predatornames.Size(); j++)
      if (strcasecmp(predatorvec[i]->getName(), predatornames[j]) == 0)
        predators.resize(predatorvec[i]);

  if (predators.Size() != predatornames.Size()) {
    handle.logMessage(LOGWARN, "Error in predatorpreyprinter - failed to match predators");
    for (i = 0; i < predatorvec.Size(); i++)
      handle.logMessage(LOGWARN, "Error in predatorpreyprinter - found predator", predatorvec[i]->getName());
    for (i = 0; i < predatornames.Size(); i++)
      handle.logMessage(LOGWARN, "Error in predatorpreyprinter - looking for predator", predatornames[i]);
    handle.logMessage(LOGFAIL, ""); //JMB this will exit gadget
  }

  for (i = 0; i < predators.Size(); i++)
    for (j = 0; j < predators.Size(); j++)
      if ((strcasecmp(predators[i]->getName(), predators[j]->getName()) == 0) && (i != j))
        handle.logMessage(LOGFAIL, "Error in predatorpreyprinter - repeated predator", predators[i]->getName());

  for (i = 0; i < preyvec.Size(); i++)
    for (j = 0; j < preynames.Size(); j++)
      if (strcasecmp(preyvec[i]->getName(), preynames[j]) == 0)
        preys.resize(preyvec[i]);

  if (preys.Size() != preynames.Size()) {
    handle.logMessage(LOGWARN, "Error in predatorpreyprinter - failed to match preys");
    for (i = 0; i < preyvec.Size(); i++)
      handle.logMessage(LOGWARN, "Error in predatorpreyprinter - found prey", preyvec[i]->getName());
    for (i = 0; i < preynames.Size(); i++)
      handle.logMessage(LOGWARN, "Error in predatorpreyprinter - looking for prey", preynames[i]);
    handle.logMessage(LOGFAIL, ""); //JMB this will exit gadget
  }

  for (i = 0; i < preys.Size(); i++)
    for (j = 0; j < preys.Size(); j++)
      if ((strcasecmp(preys[i]->getName(), preys[j]->getName()) == 0) && (i != j))
        handle.logMessage(LOGFAIL, "Error in predatorpreyprinter - repeated prey", preys[i]->getName());

  //check that the preys are stocks and not otherfood
  for (i = 0; i < preys.Size(); i++)
    if (preys[i]->getType() == LENGTHPREY)
      handle.logMessage(LOGFAIL, "Error in predatorpreyprinter - cannot print prey", preys[i]->getName());

  //change from outer areas to inner areas.
  for (i = 0; i < areas.Nrow(); i++)
    for (j = 0; j < areas.Ncol(i); j++)
      areas[i][j] = Area->getInnerArea(areas[i][j]);

  //check predator and prey areas, ages and length groups
  if (handle.getLogLevel() >= LOGWARN) {
    for (j = 0; j < areas.Nrow(); j++) {
      found = 0;
      for (i = 0; i < predators.Size(); i++)
        for (k = 0; k < areas.Ncol(j); k++)
          if (predators[i]->isInArea(areas[j][k]))
            found++;
      if (found == 0)
        handle.logMessage(LOGWARN, "Warning in predatorpreyprinter - predators not defined on all areas");
    }

    for (j = 0; j < areas.Nrow(); j++) {
      found = 0;
      for (i = 0; i < preys.Size(); i++)
        for (k = 0; k < areas.Ncol(j); k++)
          if (preys[i]->isInArea(areas[j][k]))
            found++;
      if (found == 0)
        handle.logMessage(LOGWARN, "Warning in predatorpreyprinter - preys not defined on all areas");
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
      handle.logMessage(LOGWARN, "Warning in predatorpreyprinter - minimum age less than prey age");

    found = 0;
    for (i = 0; i < preys.Size(); i++)
      if (maxage <= ((StockPrey*)preys[i])->maxAge())
        found++;
    if (found == 0)
      handle.logMessage(LOGWARN, "Warning in predatorpreyprinter - maximum age greater than prey age");

    found = 0;
    for (i = 0; i < preys.Size(); i++)
      if (LgrpDiv->maxLength(0) > preys[i]->getLengthGroupDiv()->minLength())
        found++;
    if (found == 0)
      handle.logMessage(LOGWARN, "Warning in predatorpreyprinter - minimum length group less than prey length");

    found = 0;
    for (i = 0; i < preys.Size(); i++)
      if (LgrpDiv->minLength(LgrpDiv->numLengthGroups()) < preys[i]->getLengthGroupDiv()->maxLength())
        found++;
    if (found == 0)
      handle.logMessage(LOGWARN, "Warning in predatorpreyprinter - maximum length group greater than prey length");
  }

  aggregator = new PredatorPreyAggregator(predators, preys, LgrpDiv, areas, ages);
}

void PredatorPreyPrinter::Print(const TimeClass* const TimeInfo, int printtime) {

  if ((!AAT.atCurrentTime(TimeInfo)) || (printtime != printtimeid))
    return;

  int a, age, len;
  aggregator->Sum(TimeInfo);
  alptr = &aggregator->getConsumption();
  for (a = 0; a < areas.Nrow(); a++) {
    dptr = aggregator->getMortality()[a];
    for (age = (*alptr)[a].minAge(); age <= (*alptr)[a].maxAge(); age++) {
      for (len = (*alptr)[a].minLength(age); len < (*alptr)[a].maxLength(age); len++) {
        outfile << setw(lowwidth) << TimeInfo->getYear() << sep
          << setw(lowwidth) << TimeInfo->getStep() << sep
          << setw(printwidth) << areaindex[a] << sep
          << setw(printwidth) << ageindex[age] << sep
          << setw(printwidth) << lenindex[len] << sep;

        //JMB crude filter to remove the 'silly' values from the output
        if (((*alptr)[a][age][len].N < rathersmall) || ((*alptr)[a][age][len].W < 0.0))
          outfile << setw(width) << 0 << sep << setw(width) << 0 << sep << setw(width) << 0 << endl;
        else
          outfile << setprecision(precision) << setw(width) << (*alptr)[a][age][len].N
            << sep << setprecision(precision) << setw(width)
            << (*alptr)[a][age][len].N * (*alptr)[a][age][len].W
            << sep << setprecision(precision) << setw(width)
            << (*dptr)[age][len] << endl;
      }
    }
  }
  outfile.flush();
}

PredatorPreyPrinter::~PredatorPreyPrinter() {
  outfile.close();
  outfile.clear();
  delete LgrpDiv;
  delete aggregator;
  int i;
  for (i = 0; i < predatornames.Size(); i++)
    delete[] predatornames[i];
  for (i = 0; i < preynames.Size(); i++)
    delete[] preynames[i];
  for (i = 0; i < lenindex.Size(); i++)
    delete[] lenindex[i];
  for (i = 0; i < ageindex.Size(); i++)
    delete[] ageindex[i];
  for (i = 0; i < areaindex.Size(); i++)
    delete[] areaindex[i];
}
