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
extern RunID RUNID;

PredatorOverPrinter::PredatorOverPrinter(CommentStream& infile,
  const AreaClass* const Area, const TimeClass* const TimeInfo)
  : Printer(PREDATOROVERPRINTER), predLgrpDiv(0), aggregator(0) {

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
  while (!infile.eof() && !(strcasecmp(text, "areaaggfile") == 0)) {
    predatornames.resize(1);
    predatornames[i] = new char[strlen(text) + 1];
    strcpy(predatornames[i++], text);
    infile >> text >> ws;
  }

  //Read in area aggregation from file
  char filename[MaxStrLength];
  strncpy(filename, "", MaxStrLength);
  ifstream datafile;
  CommentStream subdata(datafile);

  infile >> filename >> ws;
  datafile.open(filename, ios::in);
  checkIfFailure(datafile, filename);
  handle.Open(filename);
  i = readAggregation(subdata, areas, areaindex);
  handle.Close();
  datafile.close();
  datafile.clear();

  //Read in length aggregation from file
  DoubleVector lengths;
  readWordAndValue(infile, "lenaggfile", filename);
  datafile.open(filename, ios::in);
  checkIfFailure(datafile, filename);
  handle.Open(filename);
  i = readLengthAggregation(subdata, lengths, lenindex);
  handle.Close();
  datafile.close();
  datafile.clear();

  //Must change from outer areas to inner areas.
  for (i = 0; i < areas.Nrow(); i++)
    for (j = 0; j < areas.Ncol(i); j++)
      if ((areas[i][j] = Area->InnerArea(areas[i][j])) == -1)
        handle.UndefinedArea(areas[i][j]);

  //Finished reading from infile.
  predLgrpDiv = new LengthGroupDivision(lengths);
  if (predLgrpDiv->Error())
    printLengthGroupError(lengths, "predoverprinter");

  //Open the printfile
  readWordAndValue(infile, "printfile", filename);
  outfile.open(filename, ios::out);
  checkIfFailure(outfile, filename);

  infile >> text >> ws;
  if (!(strcasecmp(text, "yearsandsteps") == 0))
    handle.Unexpected("yearsandsteps", text);
  if (!aat.ReadFromFile(infile, TimeInfo))
    handle.Message("Error in predatoroverprinter - wrong format for yearsandsteps");

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
  outfile << "; Predator overconsumption output file for the following predators";
  for (i = 0; i < predatornames.Size(); i++)
    outfile << sep << predatornames[i];

  outfile << "\n; year-step-area-length-overconsumption biomass\n";
  outfile.flush();
}

void PredatorOverPrinter::SetPredator(PredatorPtrVector& predatorvec) {
  PredatorPtrVector predators;
  int index = 0;
  int i, j;

  for (i = 0; i < predatorvec.Size(); i++) {
    for (j = 0; j < predatornames.Size(); j++)
      if (strcasecmp(predatorvec[i]->Name(), predatornames[j]) == 0) {
        predators.resize(1);
        predators[index++] = predatorvec[i];
      }
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
  aggregator = new PredatorOverAggregator(predators, areas, predLgrpDiv);
}

void PredatorOverPrinter::Print(const TimeClass* const TimeInfo) {
  if (!aat.AtCurrentTime(TimeInfo))
    return;
  aggregator->Sum();
  const DoubleMatrix* dptr = &aggregator->ReturnSum();
  int i, j;
  for (i = 0; i < areas.Nrow(); i++) {
    for (j = 0; j < dptr->Ncol(i); j++) {
      outfile << setw(lowwidth) << TimeInfo->CurrentYear() << sep
        << setw(lowwidth) << TimeInfo->CurrentStep() << sep
        << setw(printwidth) << areaindex[i] << sep
        << setw(printwidth) << lenindex[j] << sep;

      //JMB crude filter to remove the 'silly' values from the output
      if ((*dptr)[i][j] < rathersmall)
        outfile << setw(largewidth) << 0 << endl;
      else
        outfile << setprecision(largeprecision) << setw(largewidth) << (*dptr)[i][i] << endl;
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
