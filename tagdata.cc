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
  const TimeClass* const TimeInfo, double w, TagPtrVector Tag)
  : Likelihood(TAGLIKELIHOOD, w) {

  ErrorHandler handle;
  aggregator = 0;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  int i, numarea = 0;

  char datafilename[MaxStrLength];
  char aggfilename[MaxStrLength];
  strncpy(datafilename, "", MaxStrLength);
  strncpy(aggfilename, "", MaxStrLength);
  ifstream datafile;
  CommentStream subdata(datafile);

  readWordAndValue(infile, "datafile", datafilename);

  //Read in area aggregation from file
  readWordAndValue(infile, "areaaggfile", aggfilename);
  datafile.open(aggfilename);
  checkIfFailure(datafile, aggfilename);
  handle.Open(aggfilename);
  CharPtrVector areaindex;
  numarea = readAggregation(subdata, areas, areaindex);
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
  checkIfFailure(datafile, datafilename);
  handle.Open(datafilename);
  ReadRecaptureData(subdata, TimeInfo, areaindex);
  handle.Close();
  datafile.close();
  datafile.clear();

  for (i = 0; i < areaindex.Size(); i++)
    delete[] areaindex[i];

  int j, k, check = 0;
  for (i = 0; i < tagid.Size(); i++) {
    j = 0;
    p.AddRows(1, 1, 0);
    for (k = 0; k < recTime[i].Size(); k++) {
      check = 1;
      while (check) {
        if (recTime[i][k] == recTime[i][p[i][j]] && recAreas[i][k] == recAreas[i][p[i][j]]) {
          recaptures[i][j]++;
          k++;
          if (k >= recTime[i].Size()) {
            check = 0;
          }
        } else {
          check = 0;
        }
      }
      j++;
      if (k < recTime[i].Size()) {
        p[i].resize(1, k);
        recaptures[i].resize(1, 1);
      }
    }
  }

  for (j = 0; j < tagid.Size(); j++) {
    check = 0;
    for (i = 0; i < Tag.Size(); i++) {
      if (strcasecmp(tagid[j], Tag[i]->TagName()) == 0) {
        check = 1;
        tagvec.resize(1, Tag[i]);
      }
    }
    if (check == 0) {
      cerr << "Error: when searching for names of tags for tagdata.\n"
       << "Did not find any name matching " << tagid[j] << endl;
      exit(EXIT_FAILURE);
    }
  }
  index.resize(tagvec.Size(), 0);
}

void TagData::ReadRecaptureData(CommentStream& infile,
  const TimeClass* TimeInfo, CharPtrVector areaindex) {

  ErrorHandler handle;
  int i, j, k;
  int year, step, age, maturity;
  double reclength, taglength;
  char tmparea[MaxStrLength], tmpfleet[MaxStrLength], tmptagid[MaxStrLength];
  strncpy(tmparea, "", MaxStrLength);
  strncpy(tmpfleet, "", MaxStrLength);
  strncpy(tmptagid, "", MaxStrLength);
  int keepdata, timeid, areaid, fleetid, tid;
  int FirstYear = TimeInfo->FirstYear();
  int StepsInYear = TimeInfo->StepsInYear();
  char* tagName;

  infile >> ws;
  //Check the number of columns in the inputfile
  if (countColumns(infile) != 9)
      handle.Message("Wrong number of columns in inputfile - should be 9");

  while (!infile.eof()) {
    keepdata = 0;
    infile >> tmptagid >> year >> step >> tmparea >> reclength >> tmpfleet >> age >> maturity >> taglength >> ws;

    if (year == -1 || step == -1)
      keepdata = 1;

    //if tmparea is in areaindex find areaid, else dont keep the data
    areaid = -1;
    for (i = 0; i < areaindex.Size(); i++)
      if (strcasecmp(areaindex[i], tmparea) == 0)
        areaid = i;

    if (areaid == -1)
      keepdata = 1;

    //if tmpfleet is in fleetnames find fleetid, else dont keep the data
    fleetid = -1;
    for (i = 0; i < fleetnames.Size(); i++)
      if (strcasecmp(fleetnames[i], tmpfleet) == 0)
        fleetid = i;

    if (fleetid == -1)
      keepdata = 1;

    timeid = -1;
    tid = -1;
    if ((keepdata == 0) && (TimeInfo->IsWithinPeriod(year, step))) {
      //if this is a new tagging exp. then resize
      for (i = 0; i < tagid.Size(); i++)
        if (strcasecmp(tagid[i], tmptagid) == 0)
          tid = i;

      if (tid == -1) {
        tagName = new char[strlen(tmptagid) + 1];
        strcpy(tagName, tmptagid);
        tagid.resize(1, tagName);
        Years.AddRows(1, 1, year);
        Steps.AddRows(1, 1, step);
        timeid = ((year - FirstYear) * StepsInYear) + step;
        recTime.AddRows(1, 1, timeid);
        recAreas.AddRows(1, 1, areaid);
        recAge.AddRows(1, 1, age);
        recFleet.AddRows(1, 1, fleetid);
        recMaturity.AddRows(1, 1, maturity);
        tagLength.AddRows(1, 1, taglength);
        recLength.AddRows(1, 1, reclength);
        recaptures.AddRows(1, 1, 0);

      } else {
        //if this is a new timestep, resize to store the data
        for (i = 0; i < Years[tid].Size(); i++)
          if ((Years[tid][i] == year) && (Steps[tid][i] == step))
            timeid = ((year - FirstYear) * StepsInYear) + step;

        if (timeid == -1) {
          Years[tid].resize(1, year);
          Steps[tid].resize(1, step);
          timeid = ((year - FirstYear) * StepsInYear) + step;
        }

        //store this new recapture data
        recTime[tid].resize(1, timeid);
        recAreas[tid].resize(1, areaid);
        recAge[tid].resize(1, age);
        recFleet[tid].resize(1, fleetid);
        recMaturity[tid].resize(1, maturity);
        tagLength[tid].resize(1, taglength);
        recLength[tid].resize(1, reclength);
      }
    }
  }
}

TagData::~TagData() {
  int i;
  for (i = 0; i < fleetnames.Size(); i++)
    delete[] fleetnames[i];
  for (i = 0; i < stocknames.Size(); i++)
    delete[] stocknames[i];
  for (i = 0; i < tagid.Size(); i++)
    delete[] tagid[i];
  if (aggregator != 0)  {
    for (i = 0; i < tagvec.Size(); i++)
      delete aggregator[i];
    delete[] aggregator;
    aggregator = 0;
  }
}

void TagData::Reset(const Keeper* const keeper) {
  Likelihood::Reset(keeper);
  int i;
  for (i = 0; i < index.Size(); i++)
    index[i] = 0;
}

void TagData::SetFleetsAndStocks(FleetPtrVector& Fleets, StockPtrVector& Stocks) {
  int i, j, k, found;
  FleetPtrVector fleets;
  StockPtrVector stocks;
  const CharPtrVector* stocknames;

  for (i = 0; i < fleetnames.Size(); i++) {
    found = 0;
    for (j = 0; j < Fleets.Size(); j++)
      if (strcasecmp(fleetnames[i], Fleets[j]->Name()) == 0) {
        found = 1;
        fleets.resize(1, Fleets[j]);
      }

    if (found == 0) {
      cerr << "Error: when searching for names of fleets for tagdata.\n"
        << "Did not find any name matching " << fleetnames[i] << endl;
      exit(EXIT_FAILURE);
    }
  }

  double minlen, maxlen;
  int minage, maxage, size;

  aggregator = new RecAggregator*[tagvec.Size()];
  for (k = 0; k < tagvec.Size(); k++) {
    stocknames = tagvec[k]->getStocknames();
    for (i = 0; i < stocknames->Size(); i++)  {
      found = 0;
      for (j = 0; j < Stocks.Size(); j++) {
        if (Stocks[j]->IsEaten()) {
          if (strcasecmp(stocknames->operator[](i), Stocks[j]->ReturnPrey()->Name()) == 0) {
            found = 1;
            stocks.resize(1, Stocks[j]);
          }
        }
      }
      if (found == 0) {
        cerr << "Error: when searching for names of stocks for tagdata.\n"
          << "Did not find any name matching " << stocknames->operator[](j) << endl;
        exit(EXIT_FAILURE);
      }
    }

    //Check if the stock lives on all the areas that were read in
    for (i = 0; i < recAreas.Ncol(k); i++)
      for (j = 0; j < stocks.Size(); j++)
        if (!stocks[j]->IsInArea(recAreas[k][i])) {
          cerr << "Error: when reading recapture information on the area "
            << recAreas[k][i] << "\nfor the stock " << stocks[j]->Name()
            << " which does not live on that area\n";
          exit(EXIT_FAILURE);
        }

    /* Now, pass the information on to the RecAggregator
     * aggregator which does most of the work.
     * We have to prepare the data in the following manner
     * 1. Get the maximum and minimum length of the stock and create a
     * length group division with one length group
     * 2. Get the maximum and minimum age and create a (one row) matrix
     * with all the ages from min to max (inclusive)
     * 3. Put the areas in a matrix with one column, since recaggregator
     * aggregates over the lines in the areamatrix. */

    IntMatrix agematrix(1, 0);
    IntMatrix areamatrix(areas.Size(), 1, 0);

    const LengthGroupDivision *tmpLgrpDiv;
    minlen = 9999.9;
    maxlen = 0.0;
    minage = 999;
    maxage = 0;

    for (i = 0; i < stocks.Size(); i++) {
      tmpLgrpDiv = stocks[i]->ReturnLengthGroupDiv();
      size = tmpLgrpDiv->NoLengthGroups() - 1;
      minlen = (minlen < tmpLgrpDiv->Minlength(0) ? minlen : tmpLgrpDiv->Minlength(0));
      maxlen = (maxlen > tmpLgrpDiv->Maxlength(size) ? maxlen : tmpLgrpDiv->Maxlength(size));
      minage = (minage < stocks[i]->Minage() ? minage : stocks[i]->Minage());
      maxage = (maxage > stocks[0]->Maxage() ? maxage : stocks[i]->Maxage());
    }

    LengthGroupDivision* LgrpDiv = new LengthGroupDivision(minlen, maxlen, maxlen - minlen);
    for (i = 0; i <= maxage - minage; i++)
      agematrix[0].resize(1, minage + i);
    for (i = 0; i < areas.Size(); i++)
      areamatrix[i][0] = areas[i];
    aggregator[k] = new RecAggregator(fleets, stocks, LgrpDiv, areamatrix, agematrix, tagvec[k]);

    delete LgrpDiv;
    while (stocks.Size() > 0)
      stocks.Delete(0);
  }
}

//Poisson likelihood
void TagData::AddToLikelihood(const TimeClass* const TimeInfo) {
  double lik, x;
  int n, t, i;

  lik = 0.0;
  for (t = 0; t < tagvec.Size(); t++) {
    if (((tagvec[t]->getTagYear() < TimeInfo->CurrentYear()) || ((tagvec[t]->getTagYear() == TimeInfo->CurrentYear())
        && (tagvec[t]->getTagStep() <= TimeInfo->CurrentStep()))) && ((tagvec[t]->getEndYear() > TimeInfo->CurrentYear())
        || ((tagvec[t]->getEndYear() == TimeInfo->CurrentYear()) && (tagvec[t]->getEndStep() >= TimeInfo->CurrentStep())))) {

      aggregator[t]->Sum(TimeInfo);
      const AgeBandMatrixPtrVector& alptr = aggregator[t]->AgeLengthDist();
      for (i = 0; i < areas.Size(); i++) {
        x = alptr[i][0][0].N;

        if (index[t] < p.Ncol(t) && recTime[t][p[t][index[t]]] == TimeInfo->CurrentTime()
            && recAreas[t][p[t][index[t]]] == areas[i]) {

          n = recaptures[t][index[t]];
          if (x < 0 || isZero(x))
            lik += HUGE_VALUE;
          else
            lik -= -x + (n * log(x)) - logFactorial(n);

cout << "recaptures likelihood data - x " << x << " n " << n << " score " << lik << endl;

          index[t]++;
        } else
          lik += x;
      }
    }
  }
  likelihood += lik;
}
