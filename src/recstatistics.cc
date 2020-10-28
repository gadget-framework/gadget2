#include "recstatistics.h"
#include "commentstream.h"
#include "readfunc.h"
#include "readword.h"
#include "errorhandler.h"
#include "areatime.h"
#include "fleet.h"
#include "stock.h"
#include "stockprey.h"
#include "mathfunc.h"
#include "readaggregation.h"
#include "gadget.h"
#include "global.h"

RecStatistics::RecStatistics(CommentStream& infile, const AreaClass* const Area,
  const TimeClass* const TimeInfo, double weight, TagPtrVector Tag, const char* name)
  : Likelihood(RECSTATISTICSLIKELIHOOD, weight, name), aggregator(0), alptr(0) {

  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  int i, j, check, numarea = 0;

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

  if ((strcasecmp(functionname, "lengthcalcvar") == 0) || (strcasecmp(functionname, "lengthcalcstddev") == 0))
    functionnumber = 1;
  else if ((strcasecmp(functionname, "lengthgivenvar") == 0) || (strcasecmp(functionname, "lengthgivenstddev") == 0))
    functionnumber = 2;
  else if ((strcasecmp(functionname, "lengthnovar") == 0) || (strcasecmp(functionname, "lengthnostddev") == 0))
    functionnumber = 3;
  else
    handle.logFileMessage(LOGFAIL, "\nError in recstatistics - unrecognised function", functionname);

  //read in area aggregation from file
  readWordAndValue(infile, "areaaggfile", aggfilename);
  datafile.open(aggfilename, ios::binary);
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

  //read in the fleetnames
  i = 0;
  infile >> text >> ws;
  if (strcasecmp(text, "fleetnames") != 0)
    handle.logFileUnexpected(LOGFAIL, "fleetnames", text);
  infile >> text;
  while (!infile.eof() && (strcasecmp(text, "[component]") != 0)) {
    infile >> ws;
    fleetnames.resize(new char[strlen(text) + 1]);
    strcpy(fleetnames[i++], text);
    infile >> text;
  }
  if (fleetnames.Size() == 0)
    handle.logFileMessage(LOGFAIL, "\nError in recstatistics - failed to read fleets");
  handle.logMessage(LOGMESSAGE, "Read fleet data - number of fleets", fleetnames.Size());

  //We have now read in all the data from the main likelihood file
  //But we have to read in the statistics data from datafilename
  datafile.open(datafilename, ios::binary);
  handle.checkIfFailure(datafile, datafilename);
  handle.Open(datafilename);
  readStatisticsData(subdata, TimeInfo, numarea, Tag);
  handle.Close();
  datafile.close();
  datafile.clear();

  for (j = 0; j < tagnames.Size(); j++) {
    check = 0;
    for (i = 0; i < Tag.Size(); i++) {
      if (strcasecmp(tagnames[j], Tag[i]->getName()) == 0) {
        check++;
        tagvec.resize(Tag[i]);
      }
    }
    if (check == 0)
      handle.logMessage(LOGFAIL, "Error in recstatistics - failed to match tag", tagnames[j]);
  }
}

void RecStatistics::readStatisticsData(CommentStream& infile,
  const TimeClass* TimeInfo, int numarea, TagPtrVector Tag) {

  double tmpnumber, tmpmean, tmpstddev;
  char tmparea[MaxStrLength], tmptag[MaxStrLength];
  strncpy(tmparea, "", MaxStrLength);
  strncpy(tmptag, "", MaxStrLength);
  int keepdata, needvar, readvar;
  int i, timeid, tagid, areaid, tmpindex;
  int year, step, count, reject;
  char* tagName;

  readvar = 0;
  if (functionnumber == 2)
    readvar = 1;
  needvar = 0;
  if (functionnumber == 1)
    needvar = 1;

  //Check the number of columns in the inputfile
  infile >> ws;
  if ((readvar) && (countColumns(infile) != 7))
    handle.logFileMessage(LOGFAIL, "wrong number of columns in inputfile - should be 7");
  else if ((!readvar) && (countColumns(infile) != 6))
    handle.logFileMessage(LOGFAIL, "wrong number of columns in inputfile - should be 6");

  year = step = count = reject = 0;
  while (!infile.eof()) {
    keepdata = 1;
    if (readvar)
      infile >> tmptag >> year >> step >> tmparea >> tmpnumber >> tmpmean >> tmpstddev >> ws;
    else
      infile >> tmptag >> year >> step >> tmparea >> tmpnumber >> tmpmean >> ws;

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

    //check if the year and step are in the simulation
    timeid = -1;
    tagid = -1;
    if ((TimeInfo->isWithinPeriod(year, step)) && (keepdata == 1)) {
      for (i = 0; i < tagvec.Size(); i++)
        if (strcasecmp(tagvec[i]->getName(), tmptag) == 0)
          tagid = i;

      if (tagid == -1) {
        tmpindex = -1;
        for (i = 0; i < Tag.Size(); i++)
          if (strcasecmp(Tag[i]->getName(), tmptag) == 0)
            tmpindex = i;

        if (tmpindex == -1) {
          keepdata = 0;
        } else {
          tagName = new char[strlen(tmptag) + 1];
          strcpy(tagName, tmptag);
          tagnames.resize(tagName);
          tagid = tagnames.Size() - 1;
          Years.AddRows(1, 1, year);
          Steps.AddRows(1, 1, step);
          timeid = 0;
          numbers.resize(new DoubleMatrix(1, numarea, 0.0));
          obsMean.resize(new DoubleMatrix(1, numarea, 0.0));
          modelMean.resize(new DoubleMatrix(1, numarea, 0.0));
          if (needvar)
            modelStdDev.resize(new DoubleMatrix(1, numarea, 0.0));
          if (readvar)
            obsStdDev.resize(new DoubleMatrix(1, numarea, 0.0));
        }

      } else {
        for (i = 0; i < Years.Ncol(tagid); i++)
          if ((Years[tagid][i] == year) && (Steps[tagid][i] == step))
            timeid = i;

        //if this is a new timestep, resize to store the data
        if (timeid == -1) {
          Years[tagid].resize(1, year);
          Steps[tagid].resize(1, step);
          (*numbers[tagid]).AddRows(1, numarea, 0.0);
          (*obsMean[tagid]).AddRows(1, numarea, 0.0);
          if (readvar)
            (*obsStdDev[tagid]).AddRows(1, numarea, 0.0);
          timeid = Years.Ncol(tagid) - 1;
        }
      }

    } else
      keepdata = 0;

    if (keepdata == 1) {
      //statistics data is required, so store it
      count++;
      (*numbers[tagid])[timeid][areaid] = tmpnumber;
      (*obsMean[tagid])[timeid][areaid] = tmpmean;
      if (readvar)
        (*obsStdDev[tagid])[timeid][areaid] = tmpstddev;
    } else
      reject++;  //count number of rejected data points read from file
  }

  timeindex.resize(tagvec.Size(), -1);
  if (count == 0)
    handle.logMessage(LOGWARN, "Warning in recstatistics - found no data in the data file for", this->getName());
  if (reject != 0)
    handle.logMessage(LOGMESSAGE, "Discarded invalid recstatistics data - number of invalid entries", reject);
  handle.logMessage(LOGMESSAGE, "Read recstatistics data file - number of entries", count);
}

RecStatistics::~RecStatistics() {
  int i;
  for (i = 0; i < fleetnames.Size(); i++)
    delete[] fleetnames[i];
  for (i = 0; i < tagnames.Size(); i++)
    delete[] tagnames[i];
  for (i = 0; i < areaindex.Size(); i++)
    delete[] areaindex[i];
  for (i = 0; i < numbers.Size(); i++) {
    delete numbers[i];
    delete obsMean[i];
    delete modelMean[i];
  }
  for (i = 0; i < obsStdDev.Size(); i++)
    delete obsStdDev[i];
  for (i = 0; i < modelStdDev.Size(); i++)
    delete modelStdDev[i];
  delete[] functionname;
  if (aggregator != 0)  {
    for (i = 0; i < tagvec.Size(); i++)
      delete aggregator[i];
    delete[] aggregator;
    aggregator = 0;
  }
  delete LgrpDiv;
}

void RecStatistics::Reset(const Keeper* const keeper) {
  int i;
  Likelihood::Reset(keeper);
  for (i = 0; i < timeindex.Size(); i++)
    timeindex[i] = -1;
  for (i = 0; i < modelMean.Size(); i++)
    (*modelMean[i]).setToZero();
  if (handle.getLogLevel() >= LOGMESSAGE)
    handle.logMessage(LOGMESSAGE, "Reset recstatistics component", this->getName());
}

void RecStatistics::Print(ofstream& outfile) const {
  int i;

  outfile << "\nRecapture Statistics " << this->getName() << " - likelihood value " << likelihood
    << "\n\tFunction " << functionname;
  outfile << "\n\tFleet names:";
  for (i = 0; i < fleetnames.Size(); i++)
    outfile << sep << fleetnames[i];
  outfile << endl;
  for (i = 0; i < tagnames.Size(); i++) {
    outfile << "\tTagging experiment:\t" << tagnames[i] << endl;
    aggregator[i]->Print(outfile);
    outfile << endl;
  }
  outfile.flush();
}

void RecStatistics::printSummary(ofstream& outfile) {
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

void RecStatistics::setFleetsAndStocks(FleetPtrVector& Fleets, StockPtrVector& Stocks) {
  int i, j, t, found, minage, maxage;
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
      handle.logMessage(LOGFAIL, "Error in recstatistics - unrecognised fleet", fleetnames[i]);
  }

  for (i = 0; i < fleets.Size(); i++)
    for (j = 0; j < fleets.Size(); j++)
      if ((strcasecmp(fleets[i]->getName(), fleets[j]->getName()) == 0) && (i != j))
        handle.logMessage(LOGFAIL, "Error in recstatistics - repeated fleet", fleets[i]->getName());

  aggregator = new RecAggregator*[tagvec.Size()];
  for (t = 0; t < tagvec.Size(); t++) {
    stocks.Reset();
    stocknames = tagvec[t]->getStockNames();
    for (i = 0; i < stocknames.Size(); i++) {
      found = 0;
      for (j = 0; j < Stocks.Size(); j++)
        if (Stocks[j]->isEaten())
          if (strcasecmp(stocknames[i], Stocks[j]->getName()) == 0) {
            found++;
            stocks.resize(Stocks[j]);
          }

      if (found == 0)
        handle.logMessage(LOGFAIL, "Error in recstatistics - unrecognised stock", stocknames[i]);
    }

    //Check the stock has been tagged
    for (j = 0; j < stocks.Size(); j++)
      if (!stocks[j]->isTagged())
        handle.logMessage(LOGFAIL, "Error in recstatistics - stocks hasnt been tagged", stocks[j]->getName());

    LgrpDiv = new LengthGroupDivision(*(stocks[0]->getPrey()->getLengthGroupDiv()));
    for (i = 1; i < stocks.Size(); i++)
      if (!LgrpDiv->Combine(stocks[i]->getPrey()->getLengthGroupDiv()))
        handle.logMessage(LOGFAIL, "Error in recstatistics - length groups not compatible");

    if (LgrpDiv->Error())
      handle.logMessage(LOGFAIL, "Error in recstatistics - failed to create length group");

    minage = 999;
    maxage = 0;
    for (i = 0; i < stocks.Size(); i++) {
      minage = min(stocks[i]->minAge(), minage);
      maxage = max(stocks[i]->maxAge(), maxage);
    }

    IntMatrix ages(1, 0, 0);
    for (i = 0; i <= maxage - minage; i++)
      ages[0].resize(1, minage + i);
    aggregator[t] = new RecAggregator(fleets, stocks, LgrpDiv, areas, ages, tagvec[t]);
  }
}

void RecStatistics::addLikelihood(const TimeClass* const TimeInfo) {
  int t, i, check;
  double l = 0.0;

  check = 0;
  for (t = 0; t < tagvec.Size(); t++) {
    timeindex[t] = -1;
    if (tagvec[t]->isWithinPeriod(TimeInfo->getYear(), TimeInfo->getStep())) {
      for (i = 0; i < Years.Ncol(t); i++) {
        if (Years[t][i] == TimeInfo->getYear() && Steps[t][i] == TimeInfo->getStep()) {
          if (check == 0)
            if (handle.getLogLevel() >= LOGMESSAGE)
              handle.logMessage(LOGMESSAGE, "Calculating likelihood score for recstatistics component", this->getName());
          timeindex[t] = i;
          aggregator[t]->Sum();
          check++;
        }
      }
    }
  }

  if (check > 0) {
    l = calcLikSumSquares();
    likelihood += l;
    if (handle.getLogLevel() >= LOGMESSAGE)
      handle.logMessage(LOGMESSAGE, "The likelihood score for this component on this timestep is", l);
  }
}

double RecStatistics::calcLikSumSquares() {
  double lik = 0.0;
  int t, area;
  double simvar, simdiff;

  for (t = 0; t < tagvec.Size(); t++) {
    if (timeindex[t] > -1) {
      alptr = &aggregator[t]->getSum();
      for (area = 0; area < alptr->Size(); area++) {
        simvar = 0.0;
        ps.calcStatistics((*alptr)[area][0], aggregator[t]->getLengthGroupDiv(), 0);
        (*modelMean[t])[timeindex[t]][area] = ps.meanLength();

        switch (functionnumber) {
          case 1:
            (*modelStdDev[t])[timeindex[t]][area] = ps.sdevLength();
            simvar = (*modelStdDev[t])[timeindex[t]][area] * (*modelStdDev[t])[timeindex[t]][area];
            break;
          case 2:
            simvar = (*obsStdDev[t])[timeindex[t]][area] * (*obsStdDev[t])[timeindex[t]][area];
            break;
          case 3:
            simvar = 1.0;
            break;
          default:
            handle.logMessage(LOGWARN, "Warning in recstatistics - unrecognised function", functionname);
            break;
        }

        if (!(isZero(simvar))) {
          simdiff = (*modelMean[t])[timeindex[t]][area] - (*obsMean[t])[timeindex[t]][area];
          lik += simdiff * simdiff * (*numbers[t])[timeindex[t]][area] / simvar;
        }
      }
    }
  }
  return lik;
}

void RecStatistics::printLikelihood(ofstream& outfile, const TimeClass* const TimeInfo) {
  int year = TimeInfo->getYear();
  int step = TimeInfo->getStep();
  int t, ti, timeid, area;

  for (t = 0; t < tagvec.Size(); t++) {
    if (tagvec[t]->isWithinPeriod(year, step)) {
      timeid = -1;
      for (ti = 0; ti < Years.Ncol(t); ti++)
        if (Years[t][ti] == year && Steps[t][ti] == step)
          timeid = ti;

      if (timeid > -1) {
        for (area = 0; area < areaindex.Size(); area++) {
          outfile << setw(printwidth) << tagnames[t] << sep << setw(lowwidth)
            << year << sep << setw(lowwidth) << step << sep << setw(printwidth)
            << areaindex[area] << sep << setprecision(printprecision)
            << setw(printwidth) << (*numbers[t])[timeid][area] << sep
            << setprecision(largeprecision) << setw(largewidth);

          //JMB crude filter to remove the 'silly' values from the output
          if ((*modelMean[t])[timeid][area] < rathersmall)
            outfile << 0;
          else
            outfile << (*modelMean[t])[timeid][area];

          switch (functionnumber) {
            case 1:
              outfile << sep << setprecision(printprecision) << setw(printwidth)
                << (*modelStdDev[t])[timeid][area] << endl;
              break;
            case 2:
              outfile << sep << setprecision(printprecision) << setw(printwidth)
                << (*obsStdDev[t])[timeid][area] << endl;
              break;
            case 3:
              outfile << endl;
              break;
            default:
              handle.logMessage(LOGWARN, "Warning in recstatistics - unrecognised function", functionname);
              break;
          }
        }
      }
    }
  }
}
