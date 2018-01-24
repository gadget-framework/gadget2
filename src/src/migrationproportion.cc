#include "migrationproportion.h"
#include "readfunc.h"
#include "readword.h"
#include "readaggregation.h"
#include "errorhandler.h"
#include "areatime.h"
#include "fleet.h"
#include "stock.h"
#include "mathfunc.h"
#include "stockprey.h"
#include "gadget.h"
#include "global.h"

MigrationProportion::MigrationProportion(CommentStream& infile, const AreaClass* const Area,
  const TimeClass* const TimeInfo, double weight, const char* name)
  : Likelihood(MIGRATIONPROPORTIONLIKELIHOOD, weight, name) {

  int i, j;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  int numarea = 0, numage = 0, numlen = 0;

  char datafilename[MaxStrLength];
  char aggfilename[MaxStrLength];
  strncpy(datafilename, "", MaxStrLength);
  strncpy(aggfilename, "", MaxStrLength);
  ifstream datafile;
  CommentStream subdata(datafile);

  timeindex = 0;
  biomass = 1;  // default is to use the biomass to calculate the likelihood score
  functionname = new char[MaxStrLength];
  strncpy(functionname, "", MaxStrLength);

  readWordAndValue(infile, "datafile", datafilename);
  readWordAndValue(infile, "function", functionname);

  functionnumber = 0;
  if (strcasecmp(functionname, "sumofsquares") == 0) {
    functionnumber = 1;
  } else
    handle.logFileMessage(LOGFAIL, "\nError in migrationproportion - unrecognised function", functionname);

  char c = infile.peek();
  if ((c == 'b') || (c == 'B'))
    readWordAndVariable(infile, "biomass", biomass);
  if (biomass != 0 && biomass != 1)
    handle.logFileMessage(LOGFAIL, "\nError in migrationproportion - biomass must be 0 or 1");

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
  if (areaindex.Size() == 0)
    handle.logFileMessage(LOGFAIL, "\nError in migrationproportion - failed to read areas");
  if (areaindex.Size() == 1)
    handle.logFileMessage(LOGWARN, "\nWarning in migrationproportion - only read one area");
  handle.logMessage(LOGMESSAGE, "Read area data - number of areas", areaindex.Size());

  //read in the stocknames
  i = 0;
  infile >> text >> ws;
  if (strcasecmp(text, "stocknames") != 0)
    handle.logFileUnexpected(LOGFAIL, "stocknames", text);
  infile >> text;
  while (!infile.eof() && (strcasecmp(text, "[component]") != 0)) {
    infile >> ws;
    stocknames.resize(new char[strlen(text) + 1]);
    strcpy(stocknames[i++], text);
    infile >> text;
  }
  if (stocknames.Size() == 0)
    handle.logFileMessage(LOGFAIL, "\nError in migrationproportion - failed to read stocks");
  handle.logMessage(LOGMESSAGE, "Read stock data - number of stocks", stocknames.Size());

  //We have now read in all the data from the main likelihood file
  //But we have to read in the migration proportion data from datafilename
  datafile.open(datafilename, ios::in);
  handle.checkIfFailure(datafile, datafilename);
  handle.Open(datafilename);
  readProportionData(subdata, TimeInfo, numarea);
  handle.Close();
  datafile.close();
  datafile.clear();
}

void MigrationProportion::readProportionData(CommentStream& infile,
  const TimeClass* TimeInfo, int numarea) {

  int i, year, step;
  double tmpnumber;
  char tmparea[MaxStrLength];
  strncpy(tmparea, "", MaxStrLength);
  int keepdata, timeid, areaid, count, reject;

  //Check the number of columns in the inputfile
  infile >> ws;
  if (countColumns(infile) != 4)
    handle.logFileMessage(LOGFAIL, "wrong number of columns in inputfile - should be 4");

  year = step = count = reject = 0;
  while (!infile.eof()) {
    keepdata = 1;
    infile >> year >> step >> tmparea >> tmpnumber >> ws;

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
    if ((TimeInfo->isWithinPeriod(year, step)) && (keepdata == 1)) {
      //if this is a new timestep, resize to store the data
      for (i = 0; i < Years.Size(); i++)
        if ((Years[i] == year) && (Steps[i] == step))
          timeid = i;

      if (timeid == -1) {
        Years.resize(1, year);
        Steps.resize(1, step);
        timeid = (Years.Size() - 1);

        obsDistribution.AddRows(1, numarea, 0.0);
        modelDistribution.AddRows(1, numarea, 0.0);
        likelihoodValues.resize(1, 0.0);
      }

    } else
      keepdata = 0;

    if (keepdata == 1) {
      //distribution data is required, so store it
      count++;
      obsDistribution[timeid][areaid] = tmpnumber;
    } else
      reject++;  //count number of rejected data points read from file
  }

  AAT.addActions(Years, Steps, TimeInfo);
  if (count == 0)
    handle.logMessage(LOGWARN, "Warning in migrationproportion - found no data in the data file for", this->getName());
  if (reject != 0)
    handle.logMessage(LOGMESSAGE, "Discarded invalid migrationproportion data - number of invalid entries", reject);
  handle.logMessage(LOGMESSAGE, "Read migrationproportion data file - number of entries", count);
}

MigrationProportion::~MigrationProportion() {
  int i, j;
  for (i = 0; i < stocknames.Size(); i++)
    delete[] stocknames[i];
  for (i = 0; i < areaindex.Size(); i++)
    delete[] areaindex[i];
  delete[] functionname;
}

void MigrationProportion::Reset(const Keeper* const keeper) {
  Likelihood::Reset(keeper);
  if (isZero(weight))
    handle.logMessage(LOGWARN, "Warning in migrationproportion - zero weight for", this->getName());

  modelDistribution.setToZero();
  if (handle.getLogLevel() >= LOGMESSAGE)
    handle.logMessage(LOGMESSAGE, "Reset migrationproportion component", this->getName());
}

void MigrationProportion::Print(ofstream& outfile) const {
  int i;
  outfile << "\nMigration Proportion " << this->getName() << " - likelihood value " << likelihood
    << "\n\tFunction " << functionname << "\n\tStock names:";
  for (i = 0; i < stocknames.Size(); i++)
    outfile << sep << stocknames[i];
  outfile << endl;
  outfile.flush();
}

void MigrationProportion::printLikelihood(ofstream& outfile, const TimeClass* const TimeInfo) {

  if (!AAT.atCurrentTime(TimeInfo))
    return;

  int i, area;
  timeindex = -1;
  for (i = 0; i < Years.Size(); i++)
    if ((Years[i] == TimeInfo->getYear()) && (Steps[i] == TimeInfo->getStep()))
      timeindex = i;
  if (timeindex == -1)
    handle.logMessage(LOGFAIL, "Error in migrationproportion - invalid timestep");

  for (area = 0; area < modelDistribution.Ncol(timeindex); area++) {
    outfile << setw(lowwidth) << Years[timeindex] << sep << setw(lowwidth)
      << Steps[timeindex] << sep << setw(printwidth) << areaindex[area] << sep;

    //JMB crude filter to remove the 'silly' values from the output
    if (modelDistribution[timeindex][area] < rathersmall)
      outfile << setw(largewidth) << 0 << endl;
    else
      outfile << setprecision(largeprecision) << setw(largewidth)
        << modelDistribution[timeindex][area] << endl;
  }
}

void MigrationProportion::setFleetsAndStocks(FleetPtrVector& Fleets, StockPtrVector& Stocks) {
  int i, j, k, found;

  for (i = 0; i < stocknames.Size(); i++) {
    found = 0;
    for (j = 0; j < Stocks.Size(); j++) {
      if (strcasecmp(stocknames[i], Stocks[j]->getName()) == 0) {
        found++;
        stocks.resize(Stocks[j]);
      }
    }
    if (found == 0)
      handle.logMessage(LOGFAIL, "Error in migrationproportion - unrecognised stock", stocknames[i]);
  }

  for (i = 0; i < stocks.Size(); i++)
    for (j = 0; j < stocks.Size(); j++)
      if ((strcasecmp(stocks[i]->getName(), stocks[j]->getName()) == 0) && (i != j))
        handle.logMessage(LOGFAIL, "Error in migrationproportion - repeated stock", stocks[i]->getName());

  if (handle.getLogLevel() >= LOGWARN) {
    for (j = 0; j < areas.Nrow(); j++) {
      found = 0;
      for (i = 0; i < stocks.Size(); i++)
        for (k = 0; k < areas.Ncol(j); k++)
          if (stocks[i]->isInArea(areas[j][k]))
            found++;
      if (found == 0)
        handle.logMessage(LOGWARN, "Warning in migrationproportion - stock not defined on all areas");
    }
  }
}

void MigrationProportion::addLikelihood(const TimeClass* const TimeInfo) {

  if ((!(AAT.atCurrentTime(TimeInfo))) || (isZero(weight)))
    return;

  if (handle.getLogLevel() >= LOGMESSAGE)
    handle.logMessage(LOGMESSAGE, "Calculating likelihood score for migrationproportion component", this->getName());

  int i;
  timeindex = -1;
  for (i = 0; i < Years.Size(); i++)
    if ((Years[i] == TimeInfo->getYear()) && (Steps[i] == TimeInfo->getStep()))
      timeindex = i;
  if (timeindex == -1)
    handle.logMessage(LOGFAIL, "Error in migrationproportion - invalid timestep");

  double l = 0.0;
  switch (functionnumber) {
    case 1:
      l = calcLikSumSquares(TimeInfo);
      break;
    default:
      handle.logMessage(LOGWARN, "Warning in migrationproportion - unrecognised function", functionname);
      break;
  }

  likelihood += l;
  if (handle.getLogLevel() >= LOGMESSAGE)
    handle.logMessage(LOGMESSAGE, "The likelihood score for this component on this timestep is", l);
}

double MigrationProportion::calcLikSumSquares(const TimeClass* const TimeInfo) {

  double temp, totalmodel, totaldata;
  int a, r, s;

  if (biomass) {
    for (a = 0; a < areas.Nrow(); a++)
      for (r = 0; r < areas.Ncol(a); r++)
        for (s = 0; s < stocks.Size(); s++)
          modelDistribution[timeindex][a] += stocks[s]->getTotalStockBiomass(areas[a][r]);
  } else {
    for (a = 0; a < areas.Nrow(); a++)
      for (r = 0; r < areas.Ncol(a); r++)
        for (s = 0; s < stocks.Size(); s++)
          modelDistribution[timeindex][a] += stocks[s]->getTotalStockNumber(areas[a][r]);
  }

  totalmodel = 0.0;
  totaldata = 0.0;
  for (a = 0; a < areas.Nrow(); a++) {
    totalmodel += modelDistribution[timeindex][a];
    totaldata += obsDistribution[timeindex][a];
  }
  if (!(isZero(totalmodel)))
    totalmodel = 1.0 / totalmodel;
  if (!(isZero(totaldata)))
    totaldata = 1.0 / totaldata;

  likelihoodValues[timeindex] = 0.0;
  for (a = 0; a < areas.Nrow(); a++) {
    temp = ((obsDistribution[timeindex][a] * totaldata)
          - (modelDistribution[timeindex][a] * totalmodel));
    likelihoodValues[timeindex] += (temp * temp);
  }

  return likelihoodValues[timeindex];
}

void MigrationProportion::printSummary(ofstream& outfile) {
  int year;

  for (year = 0; year < likelihoodValues.Size(); year++) {
    outfile << setw(lowwidth) << Years[year] << sep << setw(lowwidth) << Steps[year] << "   all     "
      << setw(largewidth) << this->getName() << sep << setprecision(smallprecision)
      << setw(smallwidth) << weight << sep << setprecision(largeprecision)
      << setw(largewidth) << likelihoodValues[year] << endl;
  }
  outfile.flush();
}
