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
extern RunID RUNID;

PreyOverPrinter::PreyOverPrinter(CommentStream& infile,
  const AreaClass* const Area, const TimeClass* const TimeInfo)
  : Printer(PREYOVERPRINTER), preyLgrpDiv(0), aggregator(0) {

  ErrorHandler handle;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  int i, j;

  //read in the prey names
  i = 0;
  infile >> text >> ws;
  if (!(strcasecmp(text, "preys") == 0))
    handle.Unexpected("preys", text);
  infile >> text >> ws;
  while (!infile.eof() && !(strcasecmp(text, "areaaggfile") == 0)) {
    preynames.resize(1);
    preynames[i] = new char[strlen(text) + 1];
    strcpy(preynames[i++], text);
    infile >> text >> ws;
  }

  //read in area aggregation from file
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

  //read in length aggregation from file
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
  preyLgrpDiv = new LengthGroupDivision(lengths);
  if (preyLgrpDiv->Error())
    printLengthGroupError(lengths, "preyoverprinter");

  //Open the printfile
  readWordAndValue(infile, "printfile", filename);
  outfile.open(filename, ios::out);
  checkIfFailure(outfile, filename);

  infile >> text >> ws;
  if (!(strcasecmp(text, "yearsandsteps") == 0))
    handle.Unexpected("yearsandsteps", text);
  if (!AAT.readFromFile(infile, TimeInfo))
    handle.Message("Error in preyoverprinter - wrong format for yearsandsteps");

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
  outfile << "; Prey overconsumption output file for the following preys";
  for (i = 0; i < preynames.Size(); i++)
    outfile << sep << preynames[i];

  outfile << "\n; year-step-area-length-overconsumption biomass\n";
  outfile.flush();
}

void PreyOverPrinter::setPrey(PreyPtrVector& preyvec) {
  PreyPtrVector preys;
  int index = 0;
  int i, j;
  for (i = 0; i < preyvec.Size(); i++)
    for (j = 0; j < preynames.Size(); j++)
      if (strcasecmp(preyvec[i]->Name(), preynames[j]) == 0) {
        preys.resize(1);
        preys[index++] = preyvec[i];
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
  aggregator = new PreyOverAggregator(preys, areas, preyLgrpDiv);
}

void PreyOverPrinter::Print(const TimeClass* const TimeInfo) {
  if (!AAT.AtCurrentTime(TimeInfo))
    return;
  aggregator->Sum();
  const DoubleMatrix *dptr = &aggregator->returnSum();
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
        outfile << setprecision(largeprecision) << setw(largewidth) << (*dptr)[i][j] << endl;
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
