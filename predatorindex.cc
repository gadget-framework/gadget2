#include "predatorindex.h"
#include "pionstep.h"
#include "readfunc.h"
#include "readword.h"
#include "readaggregation.h"
#include "predator.h"
#include "prey.h"
#include "errorhandler.h"
#include "predatoraggregator.h"
#include "stock.h"
#include "gadget.h"

PredatorIndices::PredatorIndices(CommentStream& infile, const AreaClass* const Area,
  const TimeClass* const TimeInfo, double w, const char* name)
  : Likelihood(PREDATORINDICESLIKELIHOOD, w) {

  ErrorHandler handle;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  int i, j;

  int numarea = 0;
  CharPtrVector areaindex;
  DoubleVector predatorlengths;
  DoubleVector preylengths;
  CharPtrVector predlenindex;
  CharPtrVector preylenindex;

  char datafilename[MaxStrLength];
  char aggfilename[MaxStrLength];
  strncpy(datafilename, "", MaxStrLength);
  strncpy(aggfilename, "", MaxStrLength);

  ifstream datafile;
  CommentStream subdata(datafile);
  piname = new char[strlen(name) + 1];
  strcpy(piname, name);
  readWordAndValue(infile, "datafile", datafilename);

  //read in the predator names and lengths
  i = 0;
  infile >> text >> ws;
  if (!(strcasecmp(text, "predatornames") == 0))
    handle.Unexpected("predatornames", text);
  infile >> text >> ws;
  while (!infile.eof() && !(strcasecmp(text, "predlenaggfile") == 0)) {
    predatornames.resize(1);
    predatornames[i] = new char[strlen(text) + 1];
    strcpy(predatornames[i++], text);
    infile >> text >> ws;
  }

  if (!(strcasecmp(text, "predlenaggfile") == 0))
    handle.Unexpected("predlenaggfile", text);
  infile >> aggfilename >> ws;
  datafile.open(aggfilename, ios::in);
  checkIfFailure(datafile, aggfilename);
  handle.Open(aggfilename);
  i = readLengthAggregation(subdata, predatorlengths, predlenindex);
  handle.Close();
  datafile.close();
  datafile.clear();

  //read in the prey names and lengths
  i = 0;
  infile >> text >> ws;
  if (!(strcasecmp(text, "preynames") == 0))
    handle.Unexpected("preynames", text);
  infile >> text >> ws;
  while (!infile.eof() && !(strcasecmp(text, "preylenaggfile") == 0)) {
    preynames.resize(1);
    preynames[i] = new char[strlen(text) + 1];
    strcpy(preynames[i++], text);
    infile >> text >> ws;
  }

  if (!(strcasecmp(text, "preylenaggfile") == 0))
    handle.Unexpected("preylenaggfile", text);
  infile >> aggfilename >> ws;
  datafile.open(aggfilename, ios::in);
  checkIfFailure(datafile, aggfilename);
  handle.Open(aggfilename);
  i = readLengthAggregation(subdata, preylengths, preylenindex);
  handle.Close();
  datafile.close();
  datafile.clear();

  //read in area aggregation from file
  readWordAndValue(infile, "areaaggfile", aggfilename);
  datafile.open(aggfilename, ios::in);
  checkIfFailure(datafile, aggfilename);
  handle.Open(aggfilename);
  numarea = readAggregation(subdata, areas, areaindex);
  handle.Close();
  datafile.close();
  datafile.clear();

  //Check if we read correct input
  if (areas.Nrow() != 1)
    handle.Message("Error - there should be only one area for the predator indices");

  //Must change from outer areas to inner areas.
  for (i = 0; i < areas.Nrow(); i++)
    for (j = 0; j < areas.Ncol(i); j++)
      if ((areas[i][j] = Area->InnerArea(areas[i][j])) == -1)
        handle.UndefinedArea(areas[i][j]);

  int biomass;
  readWordAndVariable(infile, "biomass", biomass);
  infile >> text >> ws;
  if (!(strcasecmp(text, "fittype") == 0))
    handle.Unexpected("fittype", text);

  PI = new PIOnStep(infile, areas, predatorlengths, preylengths, TimeInfo, biomass,
    areaindex, preylenindex, predlenindex, datafilename, piname);

  //prepare for next likelihood component
  infile >> ws;
  if (!infile.eof()) {
    infile >> text >> ws;
    if (!(strcasecmp(text, "[component]") == 0))
      handle.Unexpected("[component]", text);
  }

  for (i = 0; i < areaindex.Size(); i++)
    delete[] areaindex[i];
  for (i = 0; i < preylenindex.Size(); i++)
    delete[] preylenindex[i];
  for (i = 0; i < predlenindex.Size(); i++)
    delete[] predlenindex[i];
}

void PredatorIndices::setPredatorsAndPreys(PredatorPtrVector& Predators, PreyPtrVector& Preys) {
  int i, j;
  int found = 0;
  PredatorPtrVector predators;
  for (i = 0; i < predatornames.Size(); i++) {
    found = 0;
    for (j = 0; j < Predators.Size(); j++)
      if (strcasecmp(predatornames[i], Predators[j]->Name()) == 0) {
        found = 1;
        predators.resize(1, Predators[j]);
      }
    if (found == 0) {
      cerr << "Error when searching for names of predators for predatorindex\n"
        << "Did not find any name matching " << predatornames[i] << endl;
      exit(EXIT_FAILURE);
    }
  }

  PreyPtrVector preys;
  for (i = 0; i < preynames.Size(); i++) {
    found = 0;
    for (j = 0; j < Preys.Size(); j++)
      if (strcasecmp(preynames[i], Preys[j]->Name()) == 0) {
        found = 1;
        preys.resize(1, Preys[j]);
      }
    if (found == 0) {
      cerr << "Error when searching for names of preys for predatorindex\n"
        << "Did not find any name matching " << preynames[i] << endl;
      exit(EXIT_FAILURE);
    }
  }
  PI->setPredatorsAndPreys(predators, preys);
}

void PredatorIndices::Reset(const Keeper* const keeper) {
  Likelihood::Reset(keeper);
  PI->Reset(keeper);
}

void PredatorIndices::Print(ofstream& outfile) const {
  int i, j;
  outfile << "\nPredator Indices " << piname << " - likelihood value " << likelihood << "\n\tPredator names: ";
  for (i = 0; i < predatornames.Size(); i++)
    outfile << predatornames[i] << sep;
  outfile << "\n\tPrey names: ";
  for (i = 0; i < preynames.Size(); i++)
    outfile << preynames[i] << sep;
  outfile << "\nInternal areas";
  for (i = 0; i < areas.Nrow(); i++) {
    outfile << endl;
    for (j = 0; j < areas.Ncol(i); j++)
      outfile << areas[i][j] << sep;
  }
  outfile << endl;
  PI->Print(outfile);
}

PredatorIndices::~PredatorIndices() {
  int i;
  for (i = 0; i < predatornames.Size(); i++)
    delete[] predatornames[i];
  for (i = 0; i < preynames.Size(); i++)
    delete[] preynames[i];
  delete[] piname;
  delete PI;
}

void PredatorIndices::AddToLikelihood(const TimeClass* const TimeInfo) {
  likelihood = 0;
  PI->Sum(TimeInfo);
  if (TimeInfo->CurrentTime() == TimeInfo->TotalNoSteps())
    likelihood += PI->Regression();
}
