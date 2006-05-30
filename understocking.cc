#include "understocking.h"
#include "readword.h"
#include "readaggregation.h"
#include "errorhandler.h"
#include "areatime.h"
#include "predator.h"
#include "gadget.h"

extern ErrorHandler handle;

UnderStocking::UnderStocking(CommentStream& infile, const AreaClass* const Area,
  const TimeClass* const TimeInfo, double weight, const char* name)
  : Likelihood(UNDERSTOCKINGLIKELIHOOD, weight, name), powercoeff(2) {

  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  int i, j;
  char aggfilename[MaxStrLength];
  strncpy(aggfilename, "", MaxStrLength);
  ifstream datafile;
  CommentStream subdata(datafile);

  //read in area aggregation from file
  readWordAndValue(infile, "areaaggfile", aggfilename);
  datafile.open(aggfilename, ios::in);
  handle.checkIfFailure(datafile, aggfilename);
  handle.Open(aggfilename);
  i = readAggregation(subdata, areas, areaindex);
  handle.Close();
  datafile.close();
  datafile.clear();

  //Must change from outer areas to inner areas.
  for (i = 0; i < areas.Nrow(); i++)
    for (j = 0; j < areas.Ncol(i); j++)
      areas[i][j] = Area->getInnerArea(areas[i][j]);

  infile >> text >> ws;
  if (strcasecmp(text, "powercoeff") == 0)
    infile >> powercoeff >> ws >> text >> ws;

  //read in the predator names
  i = 0;
  if ((strcasecmp(text, "predatornames") != 0) && (strcasecmp(text, "fleetnames") != 0))
    handle.logFileUnexpected(LOGFAIL, "predatornames", text);
  infile >> text;
  while (!infile.eof() && (strcasecmp(text, "yearsandsteps") != 0)) {
    prednames.resize(new char[strlen(text) + 1]);
    strcpy(prednames[i++], text);
    infile >> text >> ws;
  }
  if (prednames.Size() == 0)
    handle.logFileMessage(LOGFAIL, "\nError in understocking - failed to read predators");
  handle.logMessage(LOGMESSAGE, "Read predator data - number of predators", prednames.Size());

  if (!AAT.readFromFile(infile, TimeInfo))
    handle.logFileMessage(LOGFAIL, "\nError in understocking - wrong format for yearsandsteps");

  store.resize(areas.Nrow(), 0.0);
  //prepare for next likelihood component
  infile >> ws;
  if (!infile.eof()) {
    infile >> text >> ws;
    if (strcasecmp(text, "[component]") != 0)
      handle.logFileUnexpected(LOGFAIL, "[component]", text);
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
  int i, j, k;
  int found;
  for (i = 0; i < prednames.Size(); i++) {
    found = 0;
    for (j = 0; j < Predators.Size(); j++)
      if (strcasecmp(prednames[i], Predators[j]->getName()) == 0) {
        found++;
        predators.resize(Predators[j]);
      }

    if (found == 0)
      handle.logMessage(LOGFAIL, "Error in understocking - unrecognised predator", prednames[i]);
  }

  //check predator areas
  if (handle.getLogLevel() >= LOGWARN) {
    for (j = 0; j < areas.Nrow(); j++) {
      found = 0;
      for (i = 0; i < predators.Size(); i++)
        for (k = 0; k < areas.Ncol(j); k++)
          if (predators[i]->isInArea(areas[j][k]))
            found++;
      if (found == 0)
        handle.logMessage(LOGWARN, "Warning in understocking - predator not defined on all areas");
    }
  }
}

void UnderStocking::Reset(const Keeper* const keeper) {
  Likelihood::Reset(keeper);
  Years.Reset();
  Steps.Reset();
  likelihoodValues.Reset();
  if (handle.getLogLevel() >= LOGMESSAGE)
    handle.logMessage(LOGMESSAGE, "Reset understocking component", this->getName());
}

void UnderStocking::addLikelihood(const TimeClass* const TimeInfo) {

  if (!AAT.atCurrentTime(TimeInfo))
    return;

  int i, j, k;
  double err, l;

  if (handle.getLogLevel() >= LOGMESSAGE)
    handle.logMessage(LOGMESSAGE, "Checking understocking likelihood component", this->getName());

  l = 0.0;
  for (k = 0; k < areas.Nrow(); k++) {
    err = 0.0;
    store[k] = 0.0;
    for (i = 0; i < predators.Size(); i++)
      for (j = 0; j < areas[k].Size(); j++)
        if ((predators[i]->isInArea(areas[k][j])) && (predators[i]->hasOverConsumption(areas[k][j])))
          err += predators[i]->getTotalOverConsumption(areas[k][j]);

    if (!(isZero(err))) {
      store[k] = pow(err, powercoeff);
      l += store[k];
    }
  }

  if (!(isZero(l))) {
    likelihoodValues.AddRows(1, areas.Nrow(), 0.0);
    for (k = 0; k < areas.Nrow(); k++)
      likelihoodValues[Years.Size()][k] = store[k];
    Years.resize(1, TimeInfo->getYear());
    Steps.resize(1, TimeInfo->getStep());
    if (handle.getLogLevel() >= LOGMESSAGE)
      handle.logMessage(LOGMESSAGE, "The likelihood score for this component on this timestep is", l);
    likelihood += l;
  }
}

void UnderStocking::Print(ofstream& outfile) const {
  int i, j;

  outfile << "\nUnderstocking " << this->getName() << " - likelihood value "
    << likelihood << "\n\tPredator names:";
  for (i = 0; i < prednames.Size(); i++)
    outfile << sep << prednames[i];
  outfile << "\n\tInternal areas:";
  for (i  = 0; i < areas.Nrow(); i++) {
    outfile << endl << TAB;
    for (j = 0; j < areas.Ncol(i); j++)
      outfile << areas[i][j] << sep;
  }
  outfile << endl;
  for (i = 0; i < likelihoodValues.Nrow(); i++) {
    outfile << "\n\tYear " << Years[i] << " and step " << Steps[i] << "\n\tLikelihood values: ";
    for (j = 0; j < likelihoodValues.Ncol(i); j++)
      outfile << setw(smallwidth) << setprecision(smallprecision) << likelihoodValues[i][j] << sep;
    outfile << endl;
  }
  outfile.flush();
}

void UnderStocking::printSummary(ofstream& outfile) {
  int year, area;

  for (year = 0; year < likelihoodValues.Nrow(); year++)
    for (area = 0; area < likelihoodValues.Ncol(year); area++)
      outfile << setw(lowwidth) << Years[year] << sep << setw(lowwidth)
        << Steps[year] << sep << setw(printwidth) << areaindex[area]
        << sep << this->getName() << sep << setprecision(smallprecision)
        << setw(smallwidth) << weight << sep << setprecision(largeprecision)
        << setw(largewidth) << likelihoodValues[year][area] << endl;

  outfile.flush();
}
