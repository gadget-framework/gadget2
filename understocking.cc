#include "understocking.h"
#include "readword.h"
#include "readaggregation.h"
#include "errorhandler.h"
#include "areatime.h"
#include "predator.h"
#include "gadget.h"
#include "global.h"

UnderStocking::UnderStocking(CommentStream& infile, const AreaClass* const Area,
  const TimeClass* const TimeInfo, double weight, const char* name)
  : Likelihood(UNDERSTOCKINGLIKELIHOOD, weight, name) {

  //set the default values
  powercoeff = 2.0;
  allpredators = 1;

  infile >> ws;
  if (infile.eof())
    return;  //JMB - OK, we're done here

  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  infile >> text >> ws;
  //JMB - removed the need to read in the area aggregation file
  if (strcasecmp(text, "areaaggfile") == 0) {
    infile >> text >> ws;
    handle.logMessage(LOGWARN, "Warning in understocking - area aggregation file ignored");
    infile >> text >> ws;
  }

  if (strcasecmp(text, "powercoeff") == 0)
    infile >> powercoeff >> ws >> text >> ws;
  if (isZero(powercoeff))
    handle.logMessage(LOGWARN, "Warning in understocking - power coefficient set to zero");

  //reading the predator names is optional - default to all predators
  if ((strcasecmp(text, "predatornames") == 0) || (strcasecmp(text, "fleetnames") == 0)) {
    allpredators = 0;
    int i = 0;
    infile >> text >> ws;
    while (!infile.eof() && (strcasecmp(text, "[component]") != 0) && (strcasecmp(text, "yearsandsteps") != 0)) {
      prednames.resize(new char[strlen(text) + 1]);
      strcpy(prednames[i++], text);
      infile >> text >> ws;
    }

    if (prednames.Size() == 0)
      handle.logFileMessage(LOGFAIL, "\nError in understocking - failed to read predators");
    handle.logMessage(LOGMESSAGE, "Read predator data - number of predators", prednames.Size());
  }

  //JMB - removed the need to read in the yearsandsteps
  if (strcasecmp(text, "yearsandsteps") == 0) {
    handle.logMessage(LOGWARN, "Warning in understocking - yearsandsteps data ignored");
    infile >> text >> ws;
    while (!infile.eof() && (strcasecmp(text, "[component]") != 0))
      infile >> text >> ws;
  }

  //prepare for next likelihood component
  if (!infile.eof() && (strcasecmp(text, "[component]") != 0))
    handle.logFileUnexpected(LOGFAIL, "[component]", text);
}

UnderStocking::~UnderStocking() {
  int i;
  for (i = 0; i < prednames.Size(); i++)
    delete[] prednames[i];
}

void UnderStocking::setPredatorsAndPreys(PredatorPtrVector& predvec,
  PreyPtrVector& preyvec, const AreaClass* const Area) {

  int i, j, found;

  if (allpredators) {
    //store all the preys
    for (i = 0; i < preyvec.Size(); i++)
      preys.resize(preyvec[i]);

  } else {
    //store the predators that have been specified
    for (i = 0; i < prednames.Size(); i++) {
      found = 0;
      for (j = 0; j < predvec.Size(); j++) {
        if (strcasecmp(prednames[i], predvec[j]->getName()) == 0) {
          found ++;
          predators.resize(predvec[j]);
        }
      }
      if (found == 0)
        handle.logMessage(LOGFAIL, "Error in understocking - unrecognised predator", prednames[i]);
    }

    for (i = 0; i < predators.Size(); i++)
      for (j = 0; j < predators.Size(); j++)
        if ((strcasecmp(predators[i]->getName(), predators[j]->getName()) == 0) && (i != j))
          handle.logMessage(LOGFAIL, "Error in understocking - repeated predator", predators[i]->getName());
  }

  //store a list of the areas in the model
  areas = Area->getAllModelAreas();
  for (i = 0; i < areas.Size(); i++)
    areas[i] = Area->getInnerArea(areas[i]);
}

void UnderStocking::Reset(const Keeper* const keeper) {
  Likelihood::Reset(keeper);
  if (isZero(weight))
    handle.logMessage(LOGWARN, "Warning in understocking - zero weight for", this->getName());
  Years.Reset();
  Steps.Reset();
  likelihoodValues.Reset();
  if (handle.getLogLevel() >= LOGMESSAGE)
    handle.logMessage(LOGMESSAGE, "Reset understocking component", this->getName());
}

void UnderStocking::addLikelihood(const TimeClass* const TimeInfo) {

  if (isZero(weight))
    return;

  int i, j;
  double err, l;

  if (handle.getLogLevel() >= LOGMESSAGE)
    handle.logMessage(LOGMESSAGE, "Checking understocking likelihood component", this->getName());

  l = 0.0;
  for (j = 0; j < areas.Size(); j++) {
    err = 0.0;

    if (allpredators) {
      for (i = 0; i < preys.Size(); i++)
        if (preys[i]->isOverConsumption(areas[j]))
          err += preys[i]->getTotalOverConsumption(areas[j]);
    } else {
      for (i = 0; i < predators.Size(); i++)
        if (predators[i]->hasOverConsumption(areas[j]))
          err += predators[i]->getTotalOverConsumption(areas[j]);
    }

    if (!(isZero(err)))
      l += pow(err, powercoeff);
  }

  if (!(isZero(l))) {
    likelihoodValues.resize(1, l);
    Years.resize(1, TimeInfo->getYear());
    Steps.resize(1, TimeInfo->getStep());
    if (handle.getLogLevel() >= LOGMESSAGE)
      handle.logMessage(LOGMESSAGE, "The likelihood score for this component on this timestep is", l);
    likelihood += l;
  }
}

void UnderStocking::Print(ofstream& outfile) const {
  int i;
  outfile << "\nUnderstocking " << this->getName() << " - likelihood value "
    << likelihood << endl;

  if (!allpredators) {
    outfile << "Checking understocking caused by the following predators consumption of preys" << endl;
    for (i = 0; i < prednames.Size(); i++)
      outfile << TAB << prednames[i];
  }

  for (i = 0; i < likelihoodValues.Size(); i++)
    outfile << "\n\tYear " << Years[i] << " and step " << Steps[i] << " likelihood score "
      << setw(smallwidth) << setprecision(smallprecision) << likelihoodValues[i] << endl;
  outfile.flush();
}

void UnderStocking::printSummary(ofstream& outfile) {
  int i;
  for (i = 0; i < likelihoodValues.Size(); i++)
    outfile << setw(lowwidth) << Years[i] << sep << setw(lowwidth) << Steps[i]
      << "   all      " << this->getName() << sep << setprecision(smallprecision)
      << setw(smallwidth) << weight << sep << setprecision(largeprecision)
      << setw(largewidth) << likelihoodValues[i] << endl;
  outfile.flush();
}
