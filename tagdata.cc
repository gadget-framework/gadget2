#include "areatime.h"
#include "tagdata.h"
#include "fleet.h"
#include "stock.h"
#include "stockprey.h"
#include "errorhandler.h"
#include "readfunc.h"
#include "readword.h"
#include "readaggregation.h"
#include "gadget.h"

TagData::TagData(CommentStream& infile, const AreaClass* const Area,
  const TimeClass* const TimeInfo, double likweight)
  :Likelihood(TAGLIKELIHOOD, likweight) {

  ErrorHandler handle;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  int i, numarea = 0;

  char datafilename[MaxStrLength];
  char aggfilename[MaxStrLength];
  strncpy(datafilename, "", MaxStrLength);
  strncpy(aggfilename, "", MaxStrLength);
  ifstream datafile;
  CommentStream subdata(datafile);

  /*stockname = new char[MaxStrLength];
  strncpy(stockname, "", MaxStrLength);
  ReadWordAndVariable(infile, "ngreaterthanx", n_gt_x);
  ReadWordAndValue(infile, "stockname", stockname);*/
  ReadWordAndValue(infile, "datafile", datafilename);

  //Read in area aggregation from file
  ReadWordAndValue(infile, "areaaggfile", aggfilename);
  datafile.open(aggfilename);
  CheckIfFailure(datafile, aggfilename);
  handle.Open(aggfilename);
  charptrvector areaindex;
  numarea = ReadAggregation(subdata, areas, areaindex);
  handle.Close();
  datafile.close();
  datafile.clear();

  //Must change from outer areas to inner areas.
  for (i = 0; i < areas.Size(); i++)
    if ((areas[i] = Area->InnerArea(areas[i])) == -1)
      handle.UndefinedArea(areas[i]);

  //Read in the fleetnames
  i = 0;
  infile >> text >> ws;
  if (!(strcasecmp(text, "fleetnames") == 0))
    handle.Unexpected("fleetnames", text);
  infile >> text;
  while (!infile.eof() && !(strcasecmp(text, "[component]") == 0)) {
    fleetnames.resize(1);
    fleetnames[i] = new char[strlen(text) + 1];
    strcpy(fleetnames[i++], text);
    infile >> text >> ws;
  }

  //We have now read in all the data from the main likelihood file
  //But we have to read in the recapture data from datafilename
  datafile.open(datafilename);
  CheckIfFailure(datafile, datafilename);
  handle.Open(datafilename);
  ReadRecaptureData(subdata, TimeInfo, areaindex);
  handle.Close();
  datafile.close();
  datafile.clear();
}

void TagData::ReadRecaptureData(CommentStream& infile,
  const TimeClass* TimeInfo, charptrvector areaindex) {

  ErrorHandler handle;
  int i;
  int year, step;
  double tmpnumber;
  char tmparea[MaxStrLength];
  strncpy(tmparea, "", MaxStrLength);
  int keepdata, timeid, areaid;

  //Find start of recapture data in datafile
  infile >> ws;
  char c;
  c = infile.peek();
  if (!isdigit(c)) {
    infile.get(c);
    while (c != '\n' && !infile.eof())
      infile.get(c);
  }

  //Check the number of columns in the inputfile
  if (!(CheckColumns(infile, 4)))
    handle.Message("Wrong number of columns in inputfile - should be 4");

  while (!infile.eof()) {
    keepdata = 0;
    infile >> year >> step >> tmparea >> tmpnumber >> ws;

    //if tmparea is in areaindex find areaid, else dont keep the data
    areaid = -1;
    for (i = 0; i < areaindex.Size(); i++)
      if (strcasecmp(areaindex[i], tmparea) == 0)
        areaid = i;

    if (areaid == -1)
      keepdata = 1;

    //check if the year and step are in the simulation
    timeid = -1;
    if ((keepdata == 0 ) && (TimeInfo->IsWithinPeriod(year, step))) {
      //if this is a new timestep, resize to store the data
      for (i = 0; i < Years.Size(); i++)
        if ((Years[i] == year) && (Steps[i] == step))
          timeid = i;

      if (timeid == -1) {
        Years.resize(1, year);
        Steps.resize(1, step);
        Areas.resize(1, areaid);
        recaptures.resize(1, tmpnumber);
      }
    }
  }
}

TagData::~TagData() {
  delete[] stockname;
  int i;
  for (i = 0; i < fleetnames.Size(); i++)
    delete[] fleetnames[i];
  delete aggregator;
}

void TagData::Reset(const Keeper* const keeper) {
  Likelihood::Reset(keeper);
  index = 0;
}

void TagData::Print(ofstream& outfile) const {
}

void TagData::SetFleetsAndStocks(Fleetptrvector& Fleets, Stockptrvector& Stocks) {
  int i, j, found;
  Fleetptrvector fleets;
  Stockptrvector stock;

  for (i = 0; i < fleetnames.Size(); i++) {
    found = 0;
    for (j = 0; j < Fleets.Size(); j++)
      if (strcasecmp(fleetnames[i], Fleets[j]->Name()) == 0) {
        found = 1;
        fleets.resize(1, Fleets[j]);
      }

    if (found == 0) {
      cerr << "Error: when searching for names of fleets for TagData.\n"
        << "Did not find any name matching " << fleetnames[i] << endl;
      exit(EXIT_FAILURE);
    }
  }

  found = 0;
  for (i = 0; i < Stocks.Size(); j++) {
    if (Stocks[i]->IsEaten())
      if (strcasecmp(stockname, Stocks[i]->ReturnPrey()->Name()) == 0) {
        found = 1;
        stock.resize(1, Stocks[i]);
      }
  }

  //only possible if two stocks have the same name
  if (found == 0) {
    cerr << "Error: when searching for names of stocks for TagData.\n"
      << "Did not find any name matching " << stockname << endl;
    exit(EXIT_FAILURE);
  }

  //Check if the stock lives on all the areas that were read in
  for (i = 0; i < Areas.Size(); i++)
    if (!stock[0]->IsInArea(Areas[i])) {
      cerr << "Warning: Read recapture information on the area "
        << Areas[i] << " but the stock " << stock[0]->Name()
        << " does not live on that area\n";
      exit(EXIT_FAILURE);
    }

  /* JMB code removed from here - see RemovedCode.txt for details*/

  /* Now, pass the information on to the FleetPreyAggregator
   * aggregator which does most of the work.
   * We have to prepare the data in the following manner
   * 1. Get the maximum and minimum length of the stock and create a
   * length group division with one length group
   * 2. Get the maximum and minimum age and create a (one row) matrix
   * with all the ages from min to max (inclusive)
   * 3. Put the areas in a matrix with one column, since fleetpreyagg.
   * aggregates over the lines in the areamatrix. */

  double min_l, max_l;
  int min_age, max_age;
  intmatrix agematrix(1, 0);
  intmatrix areamatrix(areas.Size(), 1, 0);

  const LengthGroupDivision *tmpLgrpDiv = stock[0]->ReturnLengthGroupDiv();
  min_l = tmpLgrpDiv->Minlength(0);
  max_l = tmpLgrpDiv->Maxlength(tmpLgrpDiv->NoLengthGroups() - 1);
  LengthGroupDivision* LgrpDiv = new LengthGroupDivision(min_l, max_l, max_l - min_l);
  min_age = stock[0]->Minage();
  max_age = stock[0]->Maxage();
  for (i = 0; i <= max_age - min_age; i++)
    agematrix[0].resize(1, min_age + i);
  for (i = 0; i < areas.Size(); i++)
    areamatrix[i][0] = areas[i];
  aggregator = new FleetPreyAggregator(fleets, stock, LgrpDiv, areamatrix, agematrix, 1);
}

void TagData::AddToLikelihood(const TimeClass* const TimeInfo) {
  double lik, p, x;
  int i, n;

  lik = 0.0;
  //Calculate "catch" and get information from aggregator
  aggregator->Sum(TimeInfo);
  const Agebandmatrixvector& alptr = aggregator->AgeLengthDist();
  const bandmatrixvector& catchratios = aggregator->CatchRatios();

  for (i = 0; i < areas.Size(); i++) {
    p = catchratios[i][0][0];
    x = alptr[i][0][0].N;
    if (index < Years.Size() && Years[index] == TimeInfo->CurrentYear() &&
        Steps[index] == TimeInfo->CurrentStep() && Areas[index] == areas[i]) {
      n = recaptures[index];
      if (n > x)
        lik += (n - x) * n_gt_x;
      else if (p != 0.0 && p != 1.0) {
        lik -= logGamma(x + 1.0) - logGamma(x - (double)n + 1.0) - logfactorial(n) + n * log(p) + (x - (double)n) * log(1 - p);
      } else if (p == 1.0 && n == (int)x); //no addition to the likelihood value
      else if (iszero(p) && n == 0);
      else
        lik += HUGE_VALUE;

      index++;

    } else {
      if (p != 1)
        lik -= x * log(1 - p);
      else
        lik += HUGE_VALUE;
    }
  }
  likelihood += lik;
}
