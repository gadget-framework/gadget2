#include "conversion.h"
#include "predatorprinter.h"
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
extern RunID RUNID;

PredatorPrinter::PredatorPrinter(CommentStream& infile,
  const AreaClass* const Area, const TimeClass* const TimeInfo)
  : Printer(PREDATORPRINTER), predLgrpDiv(0), preyLgrpDiv(0), aggregator(0) {

  ErrorHandler handle;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  int i, j;

  //Read in the predator names
  i = 0;
  infile >> text >> ws;
  if (!(strcasecmp(text, "predators") == 0))
    handle.Unexpected("predators", text);
  infile >> text >> ws;
  while (!infile.eof() && !(strcasecmp(text, "preys") == 0)) {
    predatornames.resize(1);
    predatornames[i] = new char[strlen(text) + 1];
    strcpy(predatornames[i++], text);
    infile >> text >> ws;
  }

  //Read in the prey names
  i = 0;
  infile >> text >> ws;
  while (!infile.eof() && !(strcasecmp(text, "areaaggfile") == 0)) {
    preynames.resize(1);
    preynames[i] = new char[strlen(text) + 1];
    strcpy(preynames[i++], text);
    infile >> text >> ws;
  }

  //Read in area aggregation from file
  char filename[MaxStrLength];
  strncpy(filename, "", MaxStrLength);
  ifstream datafile;
  CommentStream subdata(datafile);

  infile >> filename >> ws;
  datafile.open(filename);
  CheckIfFailure(datafile, filename);
  handle.Open(filename);
  i = ReadAggregation(subdata, areas, areaindex);
  handle.Close();
  datafile.close();
  datafile.clear();

  //Read in predator length aggregation from file
  DoubleVector predlengths;
  ReadWordAndValue(infile, "predlenaggfile", filename);
  datafile.open(filename);
  CheckIfFailure(datafile, filename);
  handle.Open(filename);
  i = ReadLengthAggregation(subdata, predlengths, predlenindex);
  handle.Close();
  datafile.close();
  datafile.clear();

  //Read in prey length aggregation from file
  DoubleVector preylengths;
  ReadWordAndValue(infile, "preylenaggfile", filename);
  datafile.open(filename);
  CheckIfFailure(datafile, filename);
  handle.Open(filename);
  i = ReadLengthAggregation(subdata, preylengths, preylenindex);
  handle.Close();
  datafile.close();
  datafile.clear();

  //Must change from outer areas to inner areas.
  for (i = 0; i < areas.Nrow(); i++)
    for (j = 0; j < areas.Ncol(i); j++)
      if ((areas[i][j] = Area->InnerArea(areas[i][j])) == -1)
        handle.UndefinedArea(areas[i][j]);

  //Finished reading from infile.
  predLgrpDiv = new LengthGroupDivision(predlengths);
  if (predLgrpDiv->Error())
    LengthGroupPrintError(predlengths, "predator lengths in predatorprinter");
  preyLgrpDiv = new LengthGroupDivision(preylengths);
  if (predLgrpDiv->Error())
    LengthGroupPrintError(preylengths, "prey lengths in predatorprinter");

  //Open the printfile
  ReadWordAndValue(infile, "printfile", filename);
  outfile.open(filename, ios::out);
  CheckIfFailure(outfile, filename);

  infile >> text >> ws;
  if (!(strcasecmp(text, "yearsandsteps") == 0))
    handle.Unexpected("yearsandsteps", text);
  if (!aat.ReadFromFile(infile, TimeInfo))
    handle.Message("Error in predatorprinter - wrong format for yearsandsteps");

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
  outfile << "; Predation output file for the following predators";
  for (i = 0; i < predatornames.Size(); i++)
    outfile << sep << predatornames[i];

  outfile << "\n; Consuming the following preys";
  for (i = 0; i < preynames.Size(); i++)
    outfile << sep << preynames[i];

  outfile << "\n; year-step-area-pred length-prey length-biomass consumed\n";
  outfile.flush();
}

void PredatorPrinter::SetPredAndPrey(PredatorPtrVector& predatorvec, PreyPtrVector& preyvec) {
  PredatorPtrVector predators;
  PreyPtrVector preys;
  int index = 0;
  int i, j;
  delete aggregator;

  for (i = 0; i < predatorvec.Size(); i++)
    for (j = 0; j < predatornames.Size(); j++)
      if (strcasecmp(predatorvec[i]->Name(), predatornames[j]) == 0) {
        predators.resize(1);
        predators[index++] = predatorvec[i];
      }

  index = 0;
  for (i = 0; i < preyvec.Size(); i++)
    for (j = 0; j < preynames.Size(); j++)
      if (strcasecmp(preyvec[i]->Name(), preynames[j]) == 0) {
        preys.resize(1);
        preys[index++] = preyvec[i];
      }

  if (predators.Size() != predatornames.Size()) {
    cerr << "Error in printer when searching for predator(s) with name matching:\n";
    for (i = 0; i < predatornames.Size(); i++)
      cerr << (const char*)predatornames[i] << sep;
    cerr << "\nDid only find the predator(s):\n";
    for (i = 0; i < predatorvec.Size(); i++)
      cerr << (const char*)predatorvec[i]->Name() << sep;
    cerr << endl;
    exit(EXIT_FAILURE);
  }
  if (preys.Size() != preynames.Size()) {
    cerr << "Error in printer when searching for prey(s) with name matching:\n";
    for (i = 0; i < preynames.Size(); i++)
      cerr << (const char*)preynames[i] << sep;
    cerr << "\nDid only find the prey(s):\n";
    for (i = 0; i < preyvec.Size(); i++)
      cerr << (const char*)preyvec[i]->Name() << sep;
    cerr << endl;
    exit(EXIT_FAILURE);
  }
  aggregator = new PredatorAggregator(predators, preys, areas, predLgrpDiv, preyLgrpDiv);
}

void PredatorPrinter::Print(const TimeClass* const TimeInfo) {
  if (!aat.AtCurrentTime(TimeInfo))
    return;
  aggregator->Sum();
  int i, j, k;

  for (i = 0; i < areas.Nrow(); i++) {
    const BandMatrix* bptr = &aggregator->ReturnSum()[i];
    for (j = 0; j < bptr->Nrow(); j++) {
      for (k = 0; k < bptr->Ncol(j); k++) {
        outfile << setw(smallwidth) << TimeInfo->CurrentYear() << sep
          << setw(smallwidth) << TimeInfo->CurrentStep() << sep
          << setw(printwidth) << areaindex[i] << sep
          << setw(printwidth) << predlenindex[j] << sep
          << setw(printwidth) << preylenindex[k] << sep;

        //JMB crude filter to remove the 'silly' values from the output
        if ((*bptr)[j][k] < rathersmall)
          outfile << setw(fullwidth) << 0 << endl;
        else
          outfile << setprecision(largeprecision) << setw(fullwidth) << (*bptr)[j][k] << endl;
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
