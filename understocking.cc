#include "understocking.h"
#include "readword.h"
#include "readaggregation.h"
#include "errorhandler.h"
#include "areatime.h"
#include "predator.h"
#include "gadget.h"

extern ErrorHandler handle;

UnderStocking::UnderStocking(CommentStream& infile, const AreaClass* const Area,
  const TimeClass* const TimeInfo, double weight)
  : Likelihood(UNDERSTOCKINGLIKELIHOOD, weight), powercoeff(2) {

  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  int i, j;
  int numarea = 0;

  char aggfilename[MaxStrLength];
  strncpy(aggfilename, "", MaxStrLength);
  ifstream datafile;
  CommentStream subdata(datafile);

  //read in area aggregation from file
  readWordAndValue(infile, "areaaggfile", aggfilename);
  datafile.open(aggfilename, ios::in);
  handle.checkIfFailure(datafile, aggfilename);
  handle.Open(aggfilename);
  numarea = readAggregation(subdata, areas, areaindex);
  handle.Close();
  datafile.close();
  datafile.clear();

  //Must change from outer areas to inner areas.
  for (i = 0; i < areas.Nrow(); i++)
    for (j = 0; j < areas.Ncol(i); j++)
      if ((areas[i][j] = Area->InnerArea(areas[i][j])) == -1)
        handle.UndefinedArea(areas[i][j]);

  infile >> text >> ws;
  if (strcasecmp(text, "powercoeff") == 0)
    infile >> powercoeff >> ws >> text >> ws;

  //read in the predator names
  i = 0;
  if (!((strcasecmp(text, "predatornames") == 0) || (strcasecmp(text, "fleetnames") == 0)))
    handle.Unexpected("predatornames", text);
  infile >> text;
  while (!infile.eof() && !(strcasecmp(text, "yearsandsteps") == 0)) {
    prednames.resize(1);
    prednames[i] = new char[strlen(text) + 1];
    strcpy(prednames[i++], text);
    infile >> text >> ws;
  }

  if (!AAT.readFromFile(infile, TimeInfo))
    handle.Message("Error in understocking - wrong format for yearsandsteps");

  //prepare for next likelihood component
  infile >> ws;
  if (!infile.eof()) {
    infile >> text >> ws;
    if (!(strcasecmp(text, "[component]") == 0))
      handle.Unexpected("[component]", text);
  }
}

UnderStocking::~UnderStocking() {
  int i;
  for (i = 0; i < prednames.Size(); i++)
    delete[] prednames[i];
  for (i = 0; i < areaindex.Size(); i++)
    delete[] areaindex[i];
}

void UnderStocking::setPredators(PredatorPtrVector& Predators) {
  int i, j;
  int found;
  for (i = 0; i < prednames.Size(); i++) {
    found = 0;
    for (j = 0; j < Predators.Size(); j++)
      if (strcasecmp(prednames[i], Predators[j]->Name()) == 0) {
        found++;
        predators.resize(1, Predators[j]);
      }

    if (found == 0)
      handle.logFailure("Error in understocking - unrecognised predator", prednames[i]);

  }
}

void UnderStocking::Reset(const Keeper* const keeper) {
  Likelihood::Reset(keeper);
  handle.logMessage("Reset understocking component");
}

void UnderStocking::addLikelihood(const TimeClass* const TimeInfo) {
  int i, j, k;
  double err, l;
  if (AAT.AtCurrentTime(TimeInfo)) {
    handle.logMessage("Checking understocking likelihood component");
    DoubleVector store(areas.Nrow(), 0.0);
    l = 0.0;
    for (k = 0; k < areas.Nrow(); k++) {
      err = 0.0;
      for (i = 0; i < predators.Size(); i++)
        for (j = 0; j < areas[k].Size(); j++)
          if (predators[i]->IsInArea(areas[k][j]))
            err += predators[i]->getTotalOverConsumption(areas[k][j]);

      store[k] += pow(err, powercoeff);
      l += store[k];
    }
    if (!(isZero(l))) {
      Years.resize(1, TimeInfo->CurrentYear());
      Steps.resize(1, TimeInfo->CurrentStep());
      likelihoodValues.AddRows(1, areas.Nrow(), 0.0);
      for (k = 0; k < areas.Nrow(); k++)
        likelihoodValues[likelihoodValues.Nrow() - 1][k] = store[k];

      handle.logMessage("The likelihood score for this component on this timestep is", l);
      likelihood += l;
    }
  }
}

void UnderStocking::LikelihoodPrint(ofstream& outfile) {
  int i, j, year, area;

  outfile << "\nUnderstocking\n\nLikelihood " << likelihood
    << "\nWeight " << weight << "\nPredator names:";
  for (i = 0; i < prednames.Size(); i++)
    outfile << sep << prednames[i];
  outfile << "\nInternal areas:";
  for (i  = 0; i < areas.Nrow(); i++) {
    outfile << endl;
    for (j = 0; j < areas.Ncol(i); j++)
      outfile << areas[i][j] << sep;
  }
  outfile << endl;

  for (year = 0; year < likelihoodValues.Nrow(); year++) {
    outfile << "\nYear " << Years[year] << " and step " << Steps[year] << "\nLikelihood values:";
    for (area = 0; area < likelihoodValues.Ncol(year); area++) {
      outfile.width(smallwidth);
      outfile.precision(smallprecision);
      outfile << sep << likelihoodValues[year][area];
    }
    outfile << endl;
  }

  outfile.flush();
}

void UnderStocking::SummaryPrint(ofstream& outfile) {
  int year, area;

  for (year = 0; year < likelihoodValues.Nrow(); year++)
    for (area = 0; area < likelihoodValues.Ncol(year); area++)
      outfile << setw(lowwidth) << Years[year] << sep << setw(lowwidth)
        << Steps[year] << sep << setw(printwidth) << areaindex[area]
        << " understocking"  << setw(smallwidth) << weight << sep
        << setprecision(largeprecision) << setw(largewidth)
        << likelihoodValues[year][area] << endl;

  outfile.flush();
}
