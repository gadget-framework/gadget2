#include "understocking.h"
#include "readword.h"
#include "readaggregation.h"
#include "errorhandler.h"
#include "areatime.h"
#include "fleet.h"
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

  //read in the fleetnames
  i = 0;
  if (!(strcasecmp(text, "fleetnames") == 0))
    handle.Unexpected("fleetnames", text);
  infile >> text;
  while (!infile.eof() && !(strcasecmp(text, "yearsandsteps") == 0)) {
    fleetnames.resize(1);
    fleetnames[i] = new char[strlen(text) + 1];
    strcpy(fleetnames[i++], text);
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
  for (i = 0; i < fleetnames.Size(); i++)
    delete[] fleetnames[i];
  for (i = 0; i < areaindex.Size(); i++)
    delete[] areaindex[i];
}

void UnderStocking::setFleets(FleetPtrVector& Fleets) {
  int i, j;
  int found;
  for (i = 0; i < fleetnames.Size(); i++) {
    found = 0;
    for (j = 0; j < Fleets.Size(); j++)
      if (strcasecmp(fleetnames[i], Fleets[j]->Name()) == 0) {
        found++;
        fleets.resize(1, Fleets[j]);
      }

    if (found == 0)
      handle.logFailure("Error in understocking - unknown fleet", fleetnames[i]);

  }
}

void UnderStocking::Reset(const Keeper* const keeper) {
  Likelihood::Reset(keeper);
  handle.logMessage("Reset understocking component");
}

void UnderStocking::addLikelihood(const TimeClass* const TimeInfo) {
  int i, j, k;
  double err;
  if (AAT.AtCurrentTime(TimeInfo)) {
    handle.logMessage("Checking likelihood components for understocking");
    for (k = 0; k < areas.Nrow(); k++) {
      err = 0.0;
      for (i = 0; i < fleets.Size(); i++)
        for (j = 0; j < areas[k].Size(); j++)
          if (fleets[i]->IsInArea(areas[k][j]))
            err += fleets[i]->OverConsumption(areas[k][j]);
      likelihood += pow(err, powercoeff);
    }
    if (!(isZero(likelihood)))
      handle.logMessage("The understocking likelihood score has increased to", likelihood);
  }
}
