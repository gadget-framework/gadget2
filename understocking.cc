#include "understocking.h"
#include "readword.h"
#include "readaggregation.h"
#include "errorhandler.h"
#include "areatime.h"
#include "fleet.h"
#include "gadget.h"

UnderStocking::UnderStocking(CommentStream& infile, const AreaClass* const Area,
  const TimeClass* const TimeInfo, double weight)
  : Likelihood(UNDERSTOCKINGLIKELIHOOD, weight), powercoeff(2) {

  ErrorHandler handle;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  int i, j;
  int numarea = 0;

  char aggfilename[MaxStrLength];
  strncpy(aggfilename, "", MaxStrLength);
  ifstream datafile;
  CommentStream subdata(datafile);

  //Read in area aggregation from file
  readWordAndValue(infile, "areaaggfile", aggfilename);
  datafile.open(aggfilename, ios::in);
  checkIfFailure(datafile, aggfilename);
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

  //Read in the fleetnames
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

  if (!aat.ReadFromFile(infile, TimeInfo))
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

void UnderStocking::SetFleets(FleetPtrVector& Fleets) {
  int i, j;
  int found;
  for (i = 0; i < fleetnames.Size(); i++) {
    found = 0;
    for (j = 0; j < Fleets.Size(); j++)
      if (strcasecmp(fleetnames[i], Fleets[j]->Name()) == 0) {
        found = 1;
        fleets.resize(1, Fleets[j]);
      }
    if (found == 0) {
      cerr << "Error when searching for names of fleets for understocking.\n"
        << "Did not find any name matching " << fleetnames[i] << endl;
      exit(EXIT_FAILURE);
    }
  }
}

void UnderStocking::Reset(const Keeper* const keeper) {
  Likelihood::Reset(keeper);
}

void UnderStocking::AddToLikelihood(const TimeClass* const TimeInfo) {
  int i, j, k;
  double err;
  if (aat.AtCurrentTime(TimeInfo))
    for (k = 0; k < areas.Nrow(); k++) {
      err = 0.0;
      for (i = 0; i < fleets.Size(); i++)
        for (j = 0; j < areas[k].Size(); j++)
          if (fleets[i]->IsInArea(areas[k][j]))
            err += fleets[i]->OverConsumption(areas[k][j]);
      likelihood += pow(err, powercoeff);
    }
}
