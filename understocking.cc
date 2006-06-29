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

  infile >> ws;
  if (infile.eof())
    return;  //JMB - OK, we're done here

  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  infile >> text >> ws;
  //JMB - removed the need to read in the area aggregation file
  if (strcasecmp(text, "areaaggfile") == 0) {
    infile >> text >> ws;
    handle.logMessage(LOGWARN, "Warning in understocking - area aggreagtion file ignored");
    infile >> text >> ws;
  }

  if (strcasecmp(text, "powercoeff") == 0)
    infile >> powercoeff >> ws >> text >> ws;
  if (isZero(powercoeff))
    handle.logMessage(LOGWARN, "Warning in understocking - power coefficient set to zero");

  //JMB - removed the need to read in the predator names and yearsandsteps
  if ((strcasecmp(text, "predatornames") == 0) || (strcasecmp(text, "fleetnames") == 0)) {
    handle.logMessage(LOGWARN, "Warning in understocking - predator names ignored");
    infile >> text >> ws;
    while (!infile.eof() && (strcasecmp(text, "[component]") != 0)) {
      infile >> text >> ws;
      if (strcasecmp(text, "yearsandsteps") == 0)
        handle.logMessage(LOGWARN, "Warning in understocking - yearsandsteps data ignored");
    }
  } else if (strcasecmp(text, "yearsandsteps") == 0) {
    handle.logMessage(LOGWARN, "Warning in understocking - yearsandsteps data ignored");
    infile >> text >> ws;
    while (!infile.eof() && (strcasecmp(text, "[component]") != 0))
      infile >> text >> ws;
  }

  //prepare for next likelihood component
  if (!infile.eof() && (strcasecmp(text, "[component]") != 0))
    handle.logFileUnexpected(LOGFAIL, "[component]", text);
}

void UnderStocking::setPreys(PreyPtrVector& preyvec, const AreaClass* const Area) {
  int i;
  //store the entries in the preyvec vector
  for (i = 0; i < preyvec.Size(); i++)
    preys.resize(preyvec[i]);

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
    for (i = 0; i < preys.Size(); i++)
      if (preys[i]->isOverConsumption(areas[j]))
        err += preys[i]->getTotalOverConsumption(areas[j]);

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
