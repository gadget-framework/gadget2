#include "areatime.h"
#include "recapture.h"
#include "fleet.h"
#include "stock.h"
#include "stockprey.h"
#include "errorhandler.h"
#include "readfunc.h"
#include "readword.h"
#include "readaggregation.h"
#include "gadget.h"

extern ErrorHandler handle;

Recaptures::Recaptures(CommentStream& infile, const AreaClass* const Area,
  const TimeClass* const TimeInfo, double weight, TagPtrVector Tag, const char* name)
  : Likelihood(TAGLIKELIHOOD, weight) {

  aggregator = 0;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  int i, j, check;
  int numarea = 0, numlen = 0;

  char datafilename[MaxStrLength];
  char aggfilename[MaxStrLength];
  strncpy(datafilename, "", MaxStrLength);
  strncpy(aggfilename, "", MaxStrLength);
  ifstream datafile;
  CommentStream subdata(datafile);

  functionname = new char[MaxStrLength];
  strncpy(functionname, "", MaxStrLength);
  recname = new char[strlen(name) + 1];
  strcpy(recname, name);

  readWordAndValue(infile, "datafile", datafilename);
  readWordAndValue(infile, "function", functionname);
  if (strcasecmp(functionname, "poisson") == 0)
    functionnumber = 1;
  else
    handle.Message("Error in recaptures - unrecognised function", functionname);

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

  //read in length aggregation from file
  readWordAndValue(infile, "lenaggfile", aggfilename);
  datafile.open(aggfilename, ios::in);
  handle.checkIfFailure(datafile, aggfilename);
  handle.Open(aggfilename);
  numlen = readLengthAggregation(subdata, lengths, lenindex);
  handle.Close();
  datafile.close();
  datafile.clear();

  //read in the fleetnames
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
  datafile.open(datafilename, ios::in);
  handle.checkIfFailure(datafile, datafilename);
  handle.Open(datafilename);
  readRecaptureData(subdata, TimeInfo, numarea, numlen);
  handle.Close();
  datafile.close();
  datafile.clear();

  for (j = 0; j < tagnames.Size(); j++) {
    check = 0;
    for (i = 0; i < Tag.Size(); i++) {
      if (strcasecmp(tagnames[j], Tag[i]->TagName()) == 0) {
        check++;
        tagvec.resize(1, Tag[i]);
      }
    }
    if (check == 0)
      handle.logFailure("Error in recaptures - failed to match tag", tagnames[j]);

  }
}

void Recaptures::readRecaptureData(CommentStream& infile,
  const TimeClass* const TimeInfo, int numarea, int numlen) {

  int i, j, k;
  int year, step;
  double tmpnumber;
  char tmparea[MaxStrLength], tmplength[MaxStrLength], tmptagid[MaxStrLength];
  strncpy(tmparea, "", MaxStrLength);
  strncpy(tmplength, "", MaxStrLength);
  strncpy(tmptagid, "", MaxStrLength);
  int keepdata, timeid, areaid, lenid, tid;
  int count = 0;
  char* tagName;

  infile >> ws;
  //Check the number of columns in the inputfile
  if (countColumns(infile) != 6)
      handle.Message("Wrong number of columns in inputfile - should be 6");

  while (!infile.eof()) {
    keepdata = 0;
    infile >> tmptagid >> year >> step >> tmparea >> tmplength >> tmpnumber >> ws;

    //if tmparea is in areaindex find areaid, else dont keep the data
    areaid = -1;
    for (i = 0; i < areaindex.Size(); i++)
      if (strcasecmp(areaindex[i], tmparea) == 0)
        areaid = i;

    if (areaid == -1)
      keepdata = 1;

    //if tmplength is in lenindex find lenid, else dont keep the data
    lenid = -1;
    for (i = 0; i < lenindex.Size(); i++)
      if (strcasecmp(lenindex[i], tmplength) == 0)
        lenid = i;

    if (lenid == -1)
      keepdata = 1;

    timeid = -1;
    tid = -1;
    if ((TimeInfo->IsWithinPeriod(year, step)) && (keepdata == 0)) {
      //if this is a new tagging exp. then resize
      for (i = 0; i < tagnames.Size(); i++)
        if (strcasecmp(tagnames[i], tmptagid) == 0)
          tid = i;

      if (tid == -1) {
        tagName = new char[strlen(tmptagid) + 1];
        strcpy(tagName, tmptagid);
        tagnames.resize(1, tagName);
        tid = tagnames.Size() - 1;
        Years.AddRows(1, 1, year);
        Steps.AddRows(1, 1, step);
        timeid = 0;
        obsDistribution.AddRows(1, 1);
        obsDistribution[tid][timeid] = new DoubleMatrix(numarea, numlen, 0.0);
        modelDistribution.AddRows(1, 1);
        modelDistribution[tid][timeid] = new DoubleMatrix(numarea, numlen, 0.0);

      } else {
        //if this is a new timestep, resize to store the data
        for (i = 0; i < Years[tid].Size(); i++)
          if ((Years[tid][i] == year) && (Steps[tid][i] == step))
            timeid = i;

        if (timeid == -1) {
          Years[tid].resize(1, year);
          Steps[tid].resize(1, step);
          timeid = Years.Ncol(tid) - 1;
          obsDistribution[tid].resize(1);
          obsDistribution[tid][timeid] = new DoubleMatrix(numarea, numlen, 0.0);
          modelDistribution[tid].resize(1);
          modelDistribution[tid][timeid] = new DoubleMatrix(numarea, numlen, 0.0);
        }
      }

      //store the number of the obsDistribution
      count++;
      (*obsDistribution[tid][timeid])[areaid][lenid] = tmpnumber;
    }
  }
  if (count == 0)
    handle.logWarning("Warning in recaptures - found no data in the data file for", recname);
  handle.logMessage("Read recaptures data file - number of entries", count);
}

Recaptures::~Recaptures() {
  int i, j;
  for (i = 0; i < fleetnames.Size(); i++)
    delete[] fleetnames[i];
  for (i = 0; i < areaindex.Size(); i++)
    delete[] areaindex[i];
  for (i = 0; i < lenindex.Size(); i++)
    delete[] lenindex[i];
  for (i = 0; i < tagnames.Size(); i++)
    delete[] tagnames[i];
  for (i = 0; i < obsDistribution.Nrow(); i++)
    for (j = 0; j < obsDistribution.Ncol(i); j++) {
      delete obsDistribution[i][j];
      delete modelDistribution[i][j];
    }
  if (aggregator != 0)  {
    for (i = 0; i < tagvec.Size(); i++)
      delete aggregator[i];
    delete[] aggregator;
    aggregator = 0;
  }
  delete[] recname;
  delete[] functionname;
}

void Recaptures::Reset(const Keeper* const keeper) {
  Likelihood::Reset(keeper);
  handle.logMessage("Reset recaptures component", recname);
}

void Recaptures::setFleetsAndStocks(FleetPtrVector& Fleets, StockPtrVector& Stocks) {
  int i, j, k, l, found;
  FleetPtrVector fleets;
  StockPtrVector stocks;
  const CharPtrVector* stocknames;

  for (i = 0; i < fleetnames.Size(); i++) {
    found = 0;
    for (j = 0; j < Fleets.Size(); j++)
      if (strcasecmp(fleetnames[i], Fleets[j]->Name()) == 0) {
        found++;
        fleets.resize(1, Fleets[j]);
      }

    if (found == 0)
      handle.logFailure("Error in recaptures - failed to match fleet", fleetnames[i]);

  }

  double minlen, maxlen;
  int minage, maxage, size;

  aggregator = new RecAggregator*[tagvec.Size()];
  for (k = 0; k < tagvec.Size(); k++) {
    stocknames = tagvec[k]->getStockNames();
    for (i = 0; i < stocknames->Size(); i++)  {
      found = 0;
      for (j = 0; j < Stocks.Size(); j++) {
        if (Stocks[j]->IsEaten()) {
          if (strcasecmp(stocknames->operator[](i), Stocks[j]->returnPrey()->Name()) == 0) {
            found++;
            stocks.resize(1, Stocks[j]);
          }
        }
      }
      if (found == 0)
        handle.logFailure("Error in recaptures - failed to match stock", stocknames->operator[](j));

    }

    //Check if the stock lives on all the areas that were read in
    for (i = 0; i < areas.Nrow(); i++)
      for (l = 0; l < areas.Ncol(i); l++)
        for (j = 0; j < stocks.Size(); j++)
          if (!stocks[j]->IsInArea(areas[i][l]))
            handle.logFailure("Error in recaptures - stocks arent defined on all areas");

    IntMatrix agematrix(1, 0);
    minage = 999;
    maxage = 0;
    for (i = 0; i < stocks.Size(); i++) {
      minage = (minage < stocks[i]->minAge() ? minage : stocks[i]->minAge());
      maxage = (maxage > stocks[0]->maxAge() ? maxage : stocks[i]->maxAge());
    }
    for (i = 0; i <= maxage - minage; i++)
      agematrix[0].resize(1, minage + i);

    LengthGroupDivision* lgrpdiv = new LengthGroupDivision(lengths);
    aggregator[k] = new RecAggregator(fleets, stocks, lgrpdiv, areas, agematrix, tagvec[k]);

    delete lgrpdiv;
    while (stocks.Size() > 0)
      stocks.Delete(0);
  }
}

void Recaptures::addLikelihood(const TimeClass* const TimeInfo) {

  double l = 0.0;
  switch(functionnumber) {
    case 1:
      l = calcLikPoisson(TimeInfo);
      break;
    default:
      handle.logWarning("Warning in recaptures - unknown function", functionname);
      break;
  }

  if (!(isZero(l))) {
    likelihood += l;
    handle.logMessage("The likelihood score for this component on this timestep is", l);
 }
}

double Recaptures::calcLikPoisson(const TimeClass* const TimeInfo) {
  double x, n, lik = 0.0;
  int t, i, ti, len, timeid, check;
  int year = TimeInfo->CurrentYear();
  int step = TimeInfo->CurrentStep();

  check = 0;
  for (t = 0; t < tagvec.Size(); t++) {
    if (tagvec[t]->IsWithinPeriod(year, step)) {

      if (check == 0) {
        handle.logMessage("Calculating likelihood score for recaptures component", recname);
        check++;
      }
     
      aggregator[t]->Sum(TimeInfo);
      const AgeBandMatrixPtrVector& alptr = aggregator[t]->returnSum();

      for (i = 0; i < alptr.Size(); i++) {
        for (len = 0; len < lengths.Size(); len++) {
          n = 0;
          x = alptr[i][0][len].N;

          timeid = -1;
          for (ti = 0; ti < Years.Ncol(t); ti++)
            if (Years[t][ti] == year && Steps[t][ti] == step)
              timeid = ti;

          if (timeid > -1) {
            n = (*obsDistribution[t][timeid])[i][len];
            (*modelDistribution[t][timeid])[i][len] = x;
          }

          if (isZero(n))
            lik += x;
          else if (x < verysmall)
            lik += verybig;
          else
            lik -= -x + (n * log(x)) - logFactorial(n);

        }
      }
    }
  }
  return lik;
}

void Recaptures::Print(ofstream& outfile) const {
  int t, ti, i, j;
  outfile << "\nRecaptures Data " << recname << " - likelihood value " << likelihood
    << "\n\tFunction " << functionname << endl;
  for (t = 0; t < tagvec.Size(); t++) {
    outfile << "\tTagging experiment:\t" << tagnames[t];
    for (ti = 0; ti < Years.Ncol(t); ti++) {
      outfile << "\n\tyear " << Years[t][ti] << " and step " << Steps[t][ti] << "\n\tobserved recaptures";
      for (i = 0; i < (*obsDistribution[t][ti]).Nrow(); i++)
        for (j = 0; j < (*obsDistribution[t][ti]).Ncol(i); j++)
          outfile << TAB << (*obsDistribution[t][ti])[i][j];

      outfile << "\n\tmodelled recaptures";
      for (i = 0; i < (*modelDistribution[t][ti]).Nrow(); i++)
        for (j = 0; j < (*modelDistribution[t][ti]).Ncol(i); j++)
          outfile << TAB << (*modelDistribution[t][ti])[i][j];
    }
    outfile << endl;
  }
  outfile.flush();
}

void Recaptures::LikelihoodPrint(ofstream& outfile) {
  int t, ti, i, j;
  outfile << "\nRecaptures Data\n\nLikelihood " << likelihood
    << "\nWeight " << weight << endl;
  for (t = 0; t < tagvec.Size(); t++) {
    outfile << "\tTagging experiment:\t" << tagnames[t];
    for (ti = 0; ti < Years.Ncol(t); ti++) {
      outfile << "\n\tyear " << Years[t][ti] << " and step " << Steps[t][ti] << "\n\tobserved recaptures";
      for (i = 0; i < (*obsDistribution[t][ti]).Nrow(); i++)
        for (j = 0; j < (*obsDistribution[t][ti]).Ncol(i); j++)
          outfile << TAB << (*obsDistribution[t][ti])[i][j];

      outfile << "\n\tmodelled recaptures";
      for (i = 0; i < (*modelDistribution[t][ti]).Nrow(); i++)
        for (j = 0; j < (*modelDistribution[t][ti]).Ncol(i); j++)
          outfile << TAB << (*modelDistribution[t][ti])[i][j];
    }
    outfile << endl;
  }
  outfile.flush();
}
