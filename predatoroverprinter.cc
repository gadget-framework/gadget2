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
extern ErrorHandler handle;

PredatorOverPrinter::PredatorOverPrinter(CommentStream& infile,
  const AreaClass* const Area, const TimeClass* const TimeInfo)
  : Printer(PREDATOROVERPRINTER), predLgrpDiv(0), aggregator(0) {

  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  int i, j;

  //read in the predator names
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

  //read in area aggregation from file
  char filename[MaxStrLength];
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

  //Must change from outer areas to inner areas.
  for (i = 0; i < areas.Nrow(); i++)
    for (j = 0; j < areas.Ncol(i); j++)
      if ((areas[i][j] = Area->InnerArea(areas[i][j])) == -1)
        handle.UndefinedArea(areas[i][j]);

  //Finished reading from infile.
  predLgrpDiv = new LengthGroupDivision(lengths);
  if (predLgrpDiv->Error())
    handle.Message("Error in predatoroverprinter - failed to create length group");

  //Open the printfile
  readWordAndValue(infile, "printfile", filename);
  outfile.open(filename, ios::out);
  handle.checkIfFailure(outfile, filename);

  infile >> text >> ws;
  if (!(strcasecmp(text, "yearsandsteps") == 0))
    handle.Unexpected("yearsandsteps", text);
  if (!AAT.readFromFile(infile, TimeInfo))
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

void PredatorOverPrinter::setPredator(PredatorPtrVector& predatorvec) {
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
    handle.logWarning("Error in predatoroverprinter - failed to match predators");
    for (i = 0; i < predatorvec.Size(); i++)
      handle.logWarning("Error in predatoroverprinter - found predator", predatorvec[i]->Name());
    for (i = 0; i < predatornames.Size(); i++)
      handle.logWarning("Error in predatoroverprinter - looking for predator", predatornames[i]);
    exit(EXIT_FAILURE);
  }
  aggregator = new PredatorOverAggregator(predators, areas, predLgrpDiv);
}

void PredatorOverPrinter::Print(const TimeClass* const TimeInfo) {
  if (!AAT.AtCurrentTime(TimeInfo))
    return;
  aggregator->Sum();
  const DoubleMatrix* dptr = &aggregator->returnSum();
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
