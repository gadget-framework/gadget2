#include "areatime.h"
#include "recapture.h"
#include "fleet.h"
#include "stock.h"
#include "stockprey.h"
#include "errorhandler.h"
#include "mathfunc.h"
#include "readfunc.h"
#include "readword.h"
#include "readaggregation.h"
#include "gadget.h"
#include "global.h"

Recaptures::Recaptures(CommentStream& infile, const AreaClass* const Area,
  const TimeClass* const TimeInfo, double weight, TagPtrVector Tag, const char* name)
  : Likelihood(TAGLIKELIHOOD, weight, name), alptr(0) {

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

  readWordAndValue(infile, "datafile", datafilename);
  readWordAndValue(infile, "function", functionname);
  if (strcasecmp(functionname, "poisson") == 0)
    functionnumber = 1;
  else
    handle.logFileMessage(LOGFAIL, "\nError in recaptures - unrecognised function", functionname);

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
      areas[i][j] = Area->getInnerArea(areas[i][j]);

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
  if (strcasecmp(text, "fleetnames") != 0)
    handle.logFileUnexpected(LOGFAIL, "fleetnames", text);
  infile >> text;
  while (!infile.eof() && (strcasecmp(text, "[component]") != 0)) {
    fleetnames.resize(new char[strlen(text) + 1]);
    strcpy(fleetnames[i++], text);
    infile >> text >> ws;
  }
  if (fleetnames.Size() == 0)
    handle.logFileMessage(LOGFAIL, "\nError in recaptures - failed to read fleets");
  handle.logMessage(LOGMESSAGE, "Read fleet data - number of fleets", fleetnames.Size());

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
      if (strcasecmp(tagnames[j], Tag[i]->getName()) == 0) {
        check++;
        tagvec.resize(Tag[i]);
        //likelihoodValues.AddRows(1, Tag[i]->getNumTagTimeSteps(), 0.0);
      }
    }
    if (check == 0)
      handle.logMessage(LOGFAIL, "Error in recaptures - failed to match tag", tagnames[j]);
  }
}

void Recaptures::readRecaptureData(CommentStream& infile,
  const TimeClass* const TimeInfo, int numarea, int numlen) {

  int i, j, k;
  double tmpnumber;
  char tmparea[MaxStrLength], tmplength[MaxStrLength], tmptagid[MaxStrLength];
  strncpy(tmparea, "", MaxStrLength);
  strncpy(tmplength, "", MaxStrLength);
  strncpy(tmptagid, "", MaxStrLength);
  int keepdata, timeid, areaid, lenid, tid;
  int year, step, count, reject;
  char* tagName;

  infile >> ws;
  //Check the number of columns in the inputfile
  if (countColumns(infile) != 6)
      handle.logFileMessage(LOGFAIL, "wrong number of columns in inputfile - should be 6");

  year = step = count = reject = 0;
  while (!infile.eof()) {
    keepdata = 1;
    infile >> tmptagid >> year >> step >> tmparea >> tmplength >> tmpnumber >> ws;

    //crude check to see if something has gone wrong and avoid infinite loops
    if (strlen(tmparea) == 0)
      handle.logFileMessage(LOGFAIL, "failed to read data from file");

    //if tmparea is in areaindex find areaid, else dont keep the data
    areaid = -1;
    for (i = 0; i < areaindex.Size(); i++)
      if (strcasecmp(areaindex[i], tmparea) == 0)
        areaid = i;

    if (areaid == -1)
      keepdata = 0;

    //if tmplength is in lenindex find lenid, else dont keep the data
    lenid = -1;
    for (i = 0; i < lenindex.Size(); i++)
      if (strcasecmp(lenindex[i], tmplength) == 0)
        lenid = i;

    if (lenid == -1)
      keepdata = 0;

    timeid = -1;
    tid = -1;
    if ((TimeInfo->isWithinPeriod(year, step)) && (keepdata == 1)) {
      //if this is a new tagging exp. then resize
      for (i = 0; i < tagnames.Size(); i++)
        if (strcasecmp(tagnames[i], tmptagid) == 0)
          tid = i;

      if (tid == -1) {
        //if this is a new tagging experiment, resize to store the data
        tagName = new char[strlen(tmptagid) + 1];
        strcpy(tagName, tmptagid);
        tagnames.resize(tagName);
        tid = tagnames.Size() - 1;
        obsYears.AddRows(1, 1, year);
        obsSteps.AddRows(1, 1, step);
        timeid = 0;
        obsDistribution.resize();
        obsDistribution[tid].resize(new DoubleMatrix(numarea, numlen, 0.0));
        modelDistribution.resize();
        modelDistribution[tid].resize(new DoubleMatrix(numarea, numlen, 0.0));
        //JMB - add objects to allow for timesteps when no recpatures are found
        modYears.AddRows(1, 0, 0);
        modSteps.AddRows(1, 0, 0);
        newDistribution.resize();

      } else {
        for (i = 0; i < obsYears[tid].Size(); i++)
          if ((obsYears[tid][i] == year) && (obsSteps[tid][i] == step))
            timeid = i;

        //if this is a new timestep, resize to store the data
        if (timeid == -1) {
          obsYears[tid].resize(1, year);
          obsSteps[tid].resize(1, step);
          timeid = obsYears.Ncol(tid) - 1;
          obsDistribution[tid].resize(new DoubleMatrix(numarea, numlen, 0.0));
          modelDistribution[tid].resize(new DoubleMatrix(numarea, numlen, 0.0));
        }
      }

      //finally store the number of the obsDistribution
      count++;
      (*obsDistribution[tid][timeid])[areaid][lenid] = tmpnumber;

    } else
      reject++;  //count number of rejected data points read from file
  }

  if (count == 0)
    handle.logMessage(LOGWARN, "Warning in recaptures - found no data in the data file for", this->getName());
  if (reject != 0)
    handle.logMessage(LOGMESSAGE, "Discarded invalid recaptures data - number of invalid entries", reject);
  handle.logMessage(LOGMESSAGE, "Read recaptures data file - number of entries", count);
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
  for (i = 0; i < obsDistribution.Nrow(); i++) {
    for (j = 0; j < obsDistribution.Ncol(i); j++) {
      delete obsDistribution[i][j];
      delete modelDistribution[i][j];
    }
  }
  for (i = 0; i < newDistribution.Nrow(); i++)
    for (j = 0; j < newDistribution.Ncol(i); j++)
      delete newDistribution[i][j];
  if (aggregator != 0)  {
    for (i = 0; i < tagvec.Size(); i++)
      delete aggregator[i];
    delete[] aggregator;
    aggregator = 0;
  }
  delete[] functionname;
}

void Recaptures::Reset(const Keeper* const keeper) {
  int i, j;
  Likelihood::Reset(keeper);
  for (i = 0; i < newDistribution.Nrow(); i++)
    for (j = 0; j < newDistribution.Ncol(i); j++)
      delete newDistribution[i][j];
  for (i = 0; i < modelDistribution.Nrow(); i++)
    for (j = 0; j < modelDistribution.Ncol(i); j++)
      (*modelDistribution[i][j]).setToZero();
  if (handle.getLogLevel() >= LOGMESSAGE)
    handle.logMessage(LOGMESSAGE, "Reset recaptures component", this->getName());
}

void Recaptures::setFleetsAndStocks(FleetPtrVector& Fleets, StockPtrVector& Stocks) {
  int i, j, k, l, found;
  FleetPtrVector fleets;
  StockPtrVector stocks;
  CharPtrVector stocknames;

  for (i = 0; i < fleetnames.Size(); i++) {
    found = 0;
    for (j = 0; j < Fleets.Size(); j++)
      if (strcasecmp(fleetnames[i], Fleets[j]->getName()) == 0) {
        found++;
        fleets.resize(Fleets[j]);
      }

    if (found == 0)
      handle.logMessage(LOGFAIL, "Error in recaptures - failed to match fleet", fleetnames[i]);
  }

  for (i = 0; i < fleets.Size(); i++)
    for (j = 0; j < fleets.Size(); j++)
      if ((strcasecmp(fleets[i]->getName(), fleets[j]->getName()) == 0) && (i != j))
        handle.logMessage(LOGFAIL, "Error in recaptures - repeated fleet", fleets[i]->getName());

  double minlen, maxlen;
  int minage, maxage, size;

  aggregator = new RecAggregator*[tagvec.Size()];
  for (k = 0; k < tagvec.Size(); k++) {
    stocknames = tagvec[k]->getStockNames();
    for (i = 0; i < stocknames.Size(); i++)  {
      found = 0;
      for (j = 0; j < Stocks.Size(); j++) {
        if (Stocks[j]->isEaten()) {
          if (strcasecmp(stocknames[i], Stocks[j]->getName()) == 0) {
            found++;
            stocks.resize(Stocks[j]);
          }
        }
      }
      if (found == 0)
        handle.logMessage(LOGFAIL, "Error in recaptures - failed to match stock", stocknames[i]);
    }

    //Check the stock has been tagged
    for (j = 0; j < stocks.Size(); j++)
      if (!stocks[j]->isTagged())
        handle.logMessage(LOGFAIL, "Error in recaptures - stocks hasnt been tagged", stocks[j]->getName());

    //Check if the stock lives on all the areas that were read in
    for (i = 0; i < areas.Nrow(); i++)
      for (l = 0; l < areas.Ncol(i); l++)
        for (j = 0; j < stocks.Size(); j++)
          if (!stocks[j]->isInArea(areas[i][l]))
            handle.logMessage(LOGFAIL, "Error in recaptures - stocks arent defined on all areas");

    IntMatrix agematrix(1, 0, 0);
    minage = 999;
    maxage = 0;
    for (i = 0; i < stocks.Size(); i++) {
      minage = min(stocks[i]->minAge(), minage);
      maxage = max(stocks[i]->maxAge(), maxage);
    }

    for (i = 0; i <= maxage - minage; i++)
      agematrix[0].resize(1, minage + i);

    LengthGroupDivision* lgrpdiv = new LengthGroupDivision(lengths);
    if (lgrpdiv->Error())
      handle.logMessage(LOGFAIL, "Error in recaptures - failed to create length group");
    aggregator[k] = new RecAggregator(fleets, stocks, lgrpdiv, areas, agematrix, tagvec[k]);

    delete lgrpdiv;
    stocks.Reset();
  }
}

void Recaptures::addLikelihood(const TimeClass* const TimeInfo) {
  double l = 0.0;
  switch (functionnumber) {
    case 1:
      l = calcLikPoisson(TimeInfo);
      break;
    default:
      handle.logMessage(LOGWARN, "Warning in recaptures - unrecognised function", functionname);
      break;
  }

  if (!(isZero(l))) {
    likelihood += l;
    if (handle.getLogLevel() >= LOGMESSAGE)
      handle.logMessage(LOGMESSAGE, "The likelihood score for this component on this timestep is", l);
 }
}

double Recaptures::calcLikPoisson(const TimeClass* const TimeInfo) {
  double x, n, lik, total;
  int t, a, ti, len, timeid, checktag, checktime;
  int year = TimeInfo->getYear();
  int step = TimeInfo->getStep();

  total = 0.0;
  for (t = 0; t < tagvec.Size(); t++) {
    checktag = 0;
    lik = 0.0;
    if (tagvec[t]->isWithinPeriod(year, step)) {

      if (!checktag) {
        if (handle.getLogLevel() >= LOGMESSAGE)
          handle.logMessage(LOGMESSAGE, "Calculating likelihood score for recaptures component", this->getName());
        checktag++;
      }

      aggregator[t]->Sum();
      alptr = &aggregator[t]->getSum();

      checktime = 0;
      timeid = -1;
      for (ti = 0; ti < obsYears.Ncol(t); ti++)
        if ((obsYears[t][ti] == year) && (obsSteps[t][ti] == step))
          timeid = ti;

      if (timeid == -1) {
        // this is a modelled return that doesnt have a corresponding observed return
        checktime++;
        for (ti = 0; ti < modYears.Ncol(t); ti++)
          if ((modYears[t][ti] == year) && (modSteps[t][ti] == step))
            timeid = ti;

        // resize objects if needed
        if (timeid == -1) {
          modYears[t].resize(1, year);
          modSteps[t].resize(1, step);
          timeid = modYears.Ncol(t) - 1;
          newDistribution[t].resize(new DoubleMatrix(areaindex.Size(), lenindex.Size(), 0.0));
        }
      }

      for (a = 0; a < areas.Nrow(); a++) {
        for (len = 0; len < lengths.Size() - 1; len++) {
          x = ((*alptr)[a][0][len]).N;

          if (!checktime) {
            // this is a modelled return that has a non-zero observed return
            n = (*obsDistribution[t][timeid])[a][len];
            (*modelDistribution[t][timeid])[a][len] = x;
          } else {
            // this is a modelled return that doesnt have a corresponding observed return
            n = 0.0;
            (*newDistribution[t][timeid])[a][len] = x;
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
    total += lik;
  }
  return total;
}

void Recaptures::Print(ofstream& outfile) const {
  int t, ti, area, len;
  outfile << "\nRecaptures Data " << this->getName() << " - likelihood value " << likelihood
    << "\n\tFunction " << functionname << endl;
  for (t = 0; t < tagvec.Size(); t++) {
    outfile << "\tTagging experiment:\t" << tagnames[t];
    for (ti = 0; ti < obsYears.Ncol(t); ti++) {
      outfile << "\n\tyear " << obsYears[t][ti] << " and step " << obsSteps[t][ti] << "\n\tobserved recaptures";
      for (area = 0; area < (*obsDistribution[t][ti]).Nrow(); area++)
        for (len = 0; len < (*obsDistribution[t][ti]).Ncol(area); len++)
          outfile << TAB << (*obsDistribution[t][ti])[area][len];

      outfile << "\n\tmodelled recaptures";
      for (area = 0; area < (*modelDistribution[t][ti]).Nrow(); area++)
        for (len = 0; len < (*modelDistribution[t][ti]).Ncol(area); len++)
          outfile << TAB << (*modelDistribution[t][ti])[area][len];
    }
    outfile << endl;
  }
  outfile.flush();
}

void Recaptures::printSummary(ofstream& outfile) {
  //JMB to start with just print a summary of all likelihood scores
  //This needs to be changed to print something for each timestep and area
  //but we need to sort out how to get the information in the same format as other components
  if (!(isZero(likelihood))) {
    outfile << "all   all        all" << sep << setw(largewidth) << this->getName() << sep
      << setprecision(smallprecision) << setw(smallwidth) << weight << sep
      << setprecision(largeprecision) << setw(largewidth) << likelihood << endl;
    outfile.flush();
  }
}

void Recaptures::printLikelihood(ofstream& outfile, const TimeClass* const TimeInfo) {
  int year = TimeInfo->getYear();
  int step = TimeInfo->getStep();
  int t, ti, timeid, area, len;

  for (t = 0; t < tagvec.Size(); t++) {
    if (tagvec[t]->isWithinPeriod(year, step)) {
      timeid = -1;
      for (ti = 0; ti < obsYears.Ncol(t); ti++)
        if (obsYears[t][ti] == year && obsSteps[t][ti] == step)
          timeid = ti;

      if (timeid > -1) {
        for (area = 0; area < modelDistribution[t][timeid]->Nrow(); area++) {
          for (len = 0; len < modelDistribution[t][timeid]->Ncol(area); len++) {
            outfile << setw(printwidth) << tagnames[t] << sep << setw(lowwidth)
              << obsYears[t][timeid] << sep << setw(lowwidth) << obsSteps[t][timeid] << sep
              << setw(printwidth) << areaindex[area] << sep << setw(printwidth)
              << lenindex[len] << sep << setprecision(largeprecision) << setw(largewidth);

            //JMB crude filter to remove the 'silly' values from the output
            if ((*modelDistribution[t][timeid])[area][len] < rathersmall)
              outfile << 0 << endl;
            else
              outfile << (*modelDistribution[t][timeid])[area][len] << endl;
          }
        }

      } else {
        for (ti = 0; ti < modYears.Ncol(t); ti++)
          if ((modYears[t][ti] == year) && (modSteps[t][ti] == step))
            timeid = ti;

        if (timeid > -1) {
          for (area = 0; area < newDistribution[t][timeid]->Nrow(); area++) {
            for (len = 0; len < newDistribution[t][timeid]->Ncol(area); len++) {
              outfile << setw(printwidth) << tagnames[t] << sep << setw(lowwidth)
                << modYears[t][timeid] << sep << setw(lowwidth) << modSteps[t][timeid] << sep
                << setw(printwidth) << areaindex[area] << sep << setw(printwidth)
                << lenindex[len] << sep << setprecision(largeprecision) << setw(largewidth);

            //JMB crude filter to remove the 'silly' values from the output
            if ((*newDistribution[t][timeid])[area][len] < rathersmall)
              outfile << 0 << endl;
            else
              outfile << (*newDistribution[t][timeid])[area][len] << endl;
            }
          }
        }
      }
    }
  }
}
