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

  char datafilename[MaxStrLength];
  char aggfilename[MaxStrLength];
  strncpy(datafilename, "", MaxStrLength);
  strncpy(aggfilename, "", MaxStrLength);
  ifstream datafile;
  CommentStream subdata(datafile);

  functionname = new char[MaxStrLength];
  strncpy(functionname, "", MaxStrLength);
  tagname = new char[strlen(name) + 1];
  strcpy(tagname, name);

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
  lgrpdiv = new LengthGroupDivision(lengths);

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
  readRecaptureData(subdata, TimeInfo);
  handle.Close();
  datafile.close();
  datafile.clear();

  for (j = 0; j < tagid.Size(); j++) {
    check = 0;
    for (i = 0; i < Tag.Size(); i++) {
      if (strcasecmp(tagid[j], Tag[i]->TagName()) == 0) {
        check++;
        tagvec.resize(1, Tag[i]);
      }
    }
    if (check == 0) {
      handle.LogWarning("Error in recaptures - failed to match tag", tagid[j]);
      exit(EXIT_FAILURE);
    }
  }
}

void Recaptures::readRecaptureData(CommentStream& infile, const TimeClass* const TimeInfo) {

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
      for (i = 0; i < tagid.Size(); i++)
        if (strcasecmp(tagid[i], tmptagid) == 0)
          tid = i;

      if (tid == -1) {
        tagName = new char[strlen(tmptagid) + 1];
        strcpy(tagName, tmptagid);
        tagid.resize(1, tagName);
        tid = tagid.Size() - 1;
        Years.AddRows(1, 1, year);
        Steps.AddRows(1, 1, step);
        timeid = 0;
        obsRecaptures.AddRows(1, 1);
        obsRecaptures[tid][timeid] = new DoubleMatrix(numarea, numlen, 0.0);
        modelRecaptures.AddRows(1, 1);
        modelRecaptures[tid][timeid] = new DoubleMatrix(numarea, numlen, 0.0);

      } else {
        //if this is a new timestep, resize to store the data
        for (i = 0; i < Years[tid].Size(); i++)
          if ((Years[tid][i] == year) && (Steps[tid][i] == step))
            timeid = i;

        if (timeid == -1) {
          Years[tid].resize(1, year);
          Steps[tid].resize(1, step);
          timeid = Years.Ncol(tid) - 1;
          obsRecaptures[tid].resize(1);
          obsRecaptures[tid][timeid] = new DoubleMatrix(numarea, numlen, 0.0);
          modelRecaptures[tid].resize(1);
          modelRecaptures[tid][timeid] = new DoubleMatrix(numarea, numlen, 0.0);
        }
      }

      //store the number of the obsRecaptures
      count++;
      (*obsRecaptures[tid][timeid])[areaid][lenid] = tmpnumber;
    }
  }
  if (count == 0)
    handle.LogWarning("Warning in recaptures - found no data in the data file for", tagname);
  handle.LogMessage("Read recaptures data file - number of entries", count);
}

Recaptures::~Recaptures() {
  int i, j;
  for (i = 0; i < fleetnames.Size(); i++)
    delete[] fleetnames[i];
  for (i = 0; i < areaindex.Size(); i++)
    delete[] areaindex[i];
  for (i = 0; i < lenindex.Size(); i++)
    delete[] lenindex[i];
  for (i = 0; i < tagid.Size(); i++)
    delete[] tagid[i];
  for (i = 0; i < obsRecaptures.Nrow(); i++)
    for (j = 0; j < obsRecaptures.Ncol(i); j++) {
      delete obsRecaptures[i][j];
      delete modelRecaptures[i][j];
    }
  if (aggregator != 0)  {
    for (i = 0; i < tagvec.Size(); i++)
      delete aggregator[i];
    delete[] aggregator;
    aggregator = 0;
  }
  delete[] tagname;
  delete[] functionname;
  delete lgrpdiv;
}

void Recaptures::Reset(const Keeper* const keeper) {
  Likelihood::Reset(keeper);
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
        found = 1;
        fleets.resize(1, Fleets[j]);
      }

    if (found == 0) {
      handle.LogWarning("Error in recaptures - failed to match fleet", fleetnames[i]);
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
          if (strcasecmp(stocknames->operator[](i), Stocks[j]->returnPrey()->Name()) == 0) {
            found = 1;
            stocks.resize(1, Stocks[j]);
          }
        }
      }
      if (found == 0) {
        handle.LogWarning("Error in recaptures - failed to match stock", stocknames->operator[](j));
        exit(EXIT_FAILURE);
      }
    }

    //Check if the stock lives on all the areas that were read in
    for (i = 0; i < areas.Nrow(); i++)
      for (l = 0; l < areas.Ncol(i); l++)
        for (j = 0; j < stocks.Size(); j++)
          if (!stocks[j]->IsInArea(areas[i][l])) {
            handle.LogWarning("Error in recaptures - stocks arent defined on all areas");
            exit(EXIT_FAILURE);
          }

    IntMatrix agematrix(1, 0);
    minage = 999;
    maxage = 0;
    for (i = 0; i < stocks.Size(); i++) {
      minage = (minage < stocks[i]->Minage() ? minage : stocks[i]->Minage());
      maxage = (maxage > stocks[0]->Maxage() ? maxage : stocks[i]->Maxage());
    }
    for (i = 0; i <= maxage - minage; i++)
      agematrix[0].resize(1, minage + i);

    LengthGroupDivision* LgrpDiv = new LengthGroupDivision(lengths);
    aggregator[k] = new RecAggregator(fleets, stocks, LgrpDiv, areas, agematrix, tagvec[k]);

    delete LgrpDiv;
    while (stocks.Size() > 0)
      stocks.Delete(0);
  }
}

void Recaptures::AddToLikelihood(const TimeClass* const TimeInfo) {

  switch(functionnumber) {
    case 1:
      likelihood += LikPoisson(TimeInfo);
      break;
    default:
      handle.LogWarning("Warning in recaptures - unknown function", functionname);
      break;
  }
}

double Recaptures::LikPoisson(const TimeClass* const TimeInfo) {
  double x, n, lik = 0.0;
  int t, i, ti, len, timeid;
  int year = TimeInfo->CurrentYear();
  int step = TimeInfo->CurrentStep();

  for (t = 0; t < tagvec.Size(); t++) {
    if (tagvec[t]->IsWithinPeriod(year, step)) {
      aggregator[t]->Sum(TimeInfo);
      const AgeBandMatrixPtrVector& alptr = aggregator[t]->returnSum();

      for (i = 0; i < alptr.Size(); i++) {
        for (len = 0; len < numlen; len++) {
          n = 0;
          x = alptr[i][0][len].N;

          timeid = -1;
          for (ti = 0; ti < Years.Ncol(t); ti++)
            if (Years[t][ti] == year && Steps[t][ti] == step)
              timeid = ti;

          if (timeid > -1) {
            n = (*obsRecaptures[t][timeid])[i][len];
            (*modelRecaptures[t][timeid])[i][len] = x;
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
  outfile << "\nRecaptures Data " << tagname << " - likelihood value " << likelihood
    << "\n\tFunction " << functionname << endl;
  for (t = 0; t < tagvec.Size(); t++) {
    outfile << "\tTagging experiment:\t" << tagid[t];
    for (ti = 0; ti < Years.Ncol(t); ti++) {
      outfile << "\n\tyear " << Years[t][ti] << " and step " << Steps[t][ti] << "\n\tobserved recaptures";
      for (i = 0; i < (*obsRecaptures[t][ti]).Nrow(); i++)
        for (j = 0; j < (*obsRecaptures[t][ti]).Ncol(i); j++)
          outfile << TAB << (*obsRecaptures[t][ti])[i][j];

      outfile << "\n\tmodelled recaptures";
      for (i = 0; i < (*modelRecaptures[t][ti]).Nrow(); i++)
        for (j = 0; j < (*modelRecaptures[t][ti]).Ncol(i); j++)
          outfile << TAB << (*modelRecaptures[t][ti])[i][j];
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
    outfile << "\tTagging experiment:\t" << tagid[t];
    for (ti = 0; ti < Years.Ncol(t); ti++) {
      outfile << "\n\tyear " << Years[t][ti] << " and step " << Steps[t][ti] << "\n\tobserved recaptures";
      for (i = 0; i < (*obsRecaptures[t][ti]).Nrow(); i++)
        for (j = 0; j < (*obsRecaptures[t][ti]).Ncol(i); j++)
          outfile << TAB << (*obsRecaptures[t][ti])[i][j];

      outfile << "\n\tmodelled recaptures";
      for (i = 0; i < (*modelRecaptures[t][ti]).Nrow(); i++)
        for (j = 0; j < (*modelRecaptures[t][ti]).Ncol(i); j++)
          outfile << TAB << (*modelRecaptures[t][ti])[i][j];
    }
    outfile << endl;
  }
  outfile.flush();
}
