#include "catchinkilos.h"
#include "readfunc.h"
#include "readword.h"
#include "readaggregation.h"
#include "errorhandler.h"
#include "areatime.h"
#include "fleet.h"
#include "stock.h"
#include "ecosystem.h"
#include "gadget.h"

extern ErrorHandler handle;

CatchInKilos::CatchInKilos(CommentStream& infile, const AreaClass* const Area,
  const TimeClass* const TimeInfo, double weight, const char* name)
  : Likelihood(CATCHINKILOSLIKELIHOOD, weight, name) {

  int i, j;
  int numarea = 0;
  int readfile = 0;
  char text[MaxStrLength];
  char datafilename[MaxStrLength];
  char aggfilename[MaxStrLength];

  strncpy(text, "", MaxStrLength);
  strncpy(datafilename, "", MaxStrLength);
  strncpy(aggfilename, "", MaxStrLength);
  ifstream datafile;
  CommentStream subdata(datafile);

  functionname = new char[MaxStrLength];
  strncpy(functionname, "", MaxStrLength);
  readWordAndValue(infile, "datafile", datafilename);
  readWordAndValue(infile, "function", functionname);

  yearly = 0;
  epsilon = 10.0;
  timeindex = 0;
  functionnumber = 0;
  if (strcasecmp(functionname, "sumofsquares") == 0)
    functionnumber = 1;
  else
    handle.logFileMessage(LOGFAIL, "Error in catchinkilos - unrecognised function", functionname);

  infile >> ws;
  char c = infile.peek();
  if ((c == 'a') || (c == 'A')) {
    //we have found either aggregationlevel or areaaggfile ...
    streampos pos = infile.tellg();

    infile >> text >> ws;
    if ((strcasecmp(text, "aggregationlevel") == 0))
      infile >> yearly >> ws;
    else if (strcasecmp(text, "areaaggfile") == 0)
      infile.seekg(pos);
    else
      handle.logFileUnexpected(LOGFAIL, "areaaggfile", text);

    //JMB - peek at the next char
    c = infile.peek();

    if (yearly != 0 && yearly != 1)
      handle.logFileMessage(LOGFAIL, "Error in catchinkilos - aggregationlevel must be 0 or 1");
  }

  //JMB - changed to make the reading of epsilon optional
  c = infile.peek();
  if ((c == 'e') || (c == 'E')) {
    readWordAndVariable(infile, "epsilon", epsilon);
    if (epsilon < verysmall) {
      handle.logFileMessage(LOGWARN, "Epsilon should be a positive number - set to default value 10");
      epsilon = 10.0;
    }
  }

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
      areas[i][j] = Area->InnerArea(areas[i][j]);

  //read in the fleetnames
  i = 0;
  infile >> text >> ws;
  if (!(strcasecmp(text, "fleetnames") == 0))
    handle.logFileUnexpected(LOGFAIL, "fleetnames", text);
  infile >> text >> ws;
  while (!infile.eof() && !(strcasecmp(text, "stocknames") == 0)) {
    fleetnames.resize(1);
    fleetnames[i] = new char[strlen(text) + 1];
    strcpy(fleetnames[i++], text);
    infile >> text >> ws;
  }
  if (fleetnames.Size() == 0)
    handle.logFileMessage(LOGFAIL, "Error in catchinkilos - failed to read fleets");
  if (handle.getLogLevel() >= LOGMESSAGE)
    handle.logMessage(LOGMESSAGE, "Read fleet data - number of fleets", fleetnames.Size());

  //read in the stocknames
  i = 0;
  if (!(strcasecmp(text, "stocknames") == 0))
    handle.logFileUnexpected(LOGFAIL, "stocknames", text);
  infile >> text;
  while (!infile.eof() && !(strcasecmp(text, "[component]") == 0)) {
    infile >> ws;
    stocknames.resize(1);
    stocknames[i] = new char[strlen(text) + 1];
    strcpy(stocknames[i++], text);
    infile >> text;
  }
  if (stocknames.Size() == 0)
    handle.logFileMessage(LOGFAIL, "Error in catchinkilos - failed to read stocks");
  if (handle.getLogLevel() >= LOGMESSAGE)
    handle.logMessage(LOGMESSAGE, "Read stock data - number of stocks", stocknames.Size());

  //We have now read in all the data from the main likelihood file
  //But we have to read in the statistics data from datafilename
  datafile.open(datafilename, ios::in);
  handle.checkIfFailure(datafile, datafilename);
  handle.Open(datafilename);
  readCatchInKilosData(subdata, TimeInfo, numarea);
  handle.Close();
  datafile.close();
  datafile.clear();
}

void CatchInKilos::Reset(const Keeper* const keeper) {
  Likelihood::Reset(keeper);
  int i, j;
  for (i = 0; i < modelDistribution.Nrow(); i++)
    for (j = 0; j < modelDistribution.Ncol(i); j++)
      modelDistribution[i][j] = 0.0;
  if (handle.getLogLevel() >= LOGMESSAGE)
    handle.logMessage(LOGMESSAGE, "Reset catchinkilos component", this->getName());
}

void CatchInKilos::Print(ofstream& outfile) const {
  int i;
  outfile << "\nCatch in Kilos " << this->getName() << " - likelihood value " << likelihood
    << "\n\tFunction " << functionname;
  outfile << "\n\tStock names:";
  for (i = 0; i < stocknames.Size(); i++)
    outfile << sep << stocknames[i];
  outfile << "\n\tFleet names:";
  for (i = 0; i < fleetnames.Size(); i++)
    outfile << sep << fleetnames[i];
  outfile << endl;
  outfile.flush();
}

double CatchInKilos::calcLikSumSquares(const TimeClass* const TimeInfo) {
  int a, a2, f, p;
  double totallikelihood = 0.0;

  for (a = 0; a < areas.Nrow(); a++) {
    likelihoodValues[timeindex][a] = 0.0;
    for (a2 = 0; a2 < areas.Ncol(a); a2++)
      for (f = 0; f < preyindex.Nrow(); f++)
        for (p = 0; p < preyindex.Ncol(f); p++)
          modelDistribution[timeindex][a] += fleets[f]->getPredator()->getConsumptionBiomass(preyindex[f][p], a2);

    if ((yearly == 0) || (TimeInfo->getStep() == TimeInfo->numSteps())) {
      likelihoodValues[timeindex][a] +=
        (log(modelDistribution[timeindex][a] + epsilon) - log(obsDistribution[timeindex][a] + epsilon))
        * (log(modelDistribution[timeindex][a] + epsilon) - log(obsDistribution[timeindex][a] + epsilon));

      totallikelihood += likelihoodValues[timeindex][a];
    }
  }
  return totallikelihood;
}

void CatchInKilos::addLikelihood(const TimeClass* const TimeInfo) {

  if (!(AAT.atCurrentTime(TimeInfo)))
    return;

  int i;
  if (yearly == 1) {
    for (i = 0; i < Years.Size(); i++)
      if (Years[i] == TimeInfo->getYear())
        timeindex = i;  
  } else {
    for (i = 0; i < Years.Size(); i++)
      if ((Years[i] == TimeInfo->getYear()) && (Steps[i] == TimeInfo->getStep()))
        timeindex = i;
  }

  double l = 0.0;
  switch (functionnumber) {
    case 1:
      l = calcLikSumSquares(TimeInfo);
      break;
    default:
      handle.logMessage(LOGWARN, "Warning in catchinkilos - unrecognised function", functionname);
      break;
  }

  if ((yearly == 0) || (TimeInfo->getStep() == TimeInfo->numSteps())) {
    likelihood += l;
    if (handle.getLogLevel() >= LOGMESSAGE) {
      handle.logMessage(LOGMESSAGE, "Calculating likelihood score for catchinkilos component", this->getName());
      handle.logMessage(LOGMESSAGE, "The likelihood score for this component on this timestep is", l);
    }
  }
}

CatchInKilos::~CatchInKilos() {
  int i;
  for (i = 0; i < stocknames.Size(); i++)
    delete[] stocknames[i];
  for (i = 0; i < fleetnames.Size(); i++)
    delete[] fleetnames[i];
  for (i = 0; i < areaindex.Size(); i++)
    delete[] areaindex[i];
  delete[] functionname;
}

void CatchInKilos::setFleetsAndStocks(FleetPtrVector& Fleets, StockPtrVector& Stocks) {
  int i, j, k, found;

  for (i = 0; i < fleetnames.Size(); i++) {
    found = 0;
    for (j = 0; j < Fleets.Size(); j++) {
      if (strcasecmp(fleetnames[i], Fleets[j]->getName()) == 0) {
        found++;
        fleets.resize(1, Fleets[j]);
      }
    }
    if (found == 0)
      handle.logMessage(LOGFAIL, "Error in catchinkilos - unrecognised fleet", fleetnames[i]);
  }

  for (i = 0; i < stocknames.Size(); i++) {
    found = 0;
    for (j = 0; j < Stocks.Size(); j++) {
      if (Stocks[j]->isEaten()) {
        if (strcasecmp(stocknames[i], Stocks[j]->getPrey()->getName()) == 0) {
          found++;
          stocks.resize(1, Stocks[j]);
        }
      }
    }
    if (found == 0)
      handle.logMessage(LOGFAIL, "Error in catchinkilos - unrecognised stock", stocknames[i]);
  }

  //check fleet and stock areas
  if (handle.getLogLevel() >= LOGWARN) {
    for (j = 0; j < areas.Nrow(); j++) {
      found = 0;
      for (i = 0; i < fleets.Size(); i++)
        for (k = 0; k < areas.Ncol(j); k++)
          if (fleets[i]->isInArea(areas[j][k]))
            found++;
      if (found == 0)
        handle.logMessage(LOGWARN, "Warning in catchinkilos - fleet not defined on all areas");
    }

    for (j = 0; j < areas.Nrow(); j++) {
      found = 0;
      for (i = 0; i < stocks.Size(); i++)
        for (k = 0; k < areas.Ncol(j); k++)
          if (stocks[i]->isInArea(areas[j][k]))
            found++;
      if (found == 0)
        handle.logMessage(LOGWARN, "Warning in catchinkilos - stock not defined on all areas");
    }
  }

  for (i = 0; i < fleets.Size(); i++) {
    found = 0;
    preyindex.AddRows(1, 0);
    for (j = 0; j < fleets[i]->getPredator()->numPreys(); j++)
      for (k = 0; k < stocknames.Size(); k++)
        if (strcasecmp(stocknames[k], fleets[i]->getPredator()->getPrey(j)->getName()) == 0) {
          found++;
          preyindex[i].resize(1, j);
        }

    if ((handle.getLogLevel() >= LOGWARN) && (found == 0))
      handle.logMessage(LOGWARN, "Warning in catchinkilos - found no stocks for fleet", fleetnames[i]);
  }

  if (preyindex.Nrow() != fleets.Size())
    handle.logMessage(LOGFAIL, "Error in catchinkilos - failed to match stocks to fleets");
}

void CatchInKilos::readCatchInKilosData(CommentStream& infile,
  const TimeClass* TimeInfo, int numarea) {

  int i;
  int year, step;
  double tmpnumber = 0.0;
  char tmparea[MaxStrLength];
  char tmpfleet[MaxStrLength];
  strncpy(tmparea, "", MaxStrLength);
  strncpy(tmpfleet, "", MaxStrLength);
  int keepdata, timeid, areaid, fleetid, check;
  int count = 0;

  //Find start of distribution data in datafile
  infile >> ws;
  char c = infile.peek();
  if (!isdigit(c)) {
    infile.get(c);
    while (c != '\n' && !infile.eof())
      infile.get(c);
  }

  //Check the number of columns in the inputfile
  check = countColumns(infile);
  if (!(((yearly == 1) && ((check == 4) || (check == 5))) || ((yearly == 0) && (check == 5))))
    handle.logFileMessage(LOGFAIL, "Wrong number of columns in inputfile - should be 4 or 5");

  step = 1; //default value in case there are only 4 columns in the datafile
  while (!infile.eof()) {
    keepdata = 0;
    if ((yearly == 1) && (check == 4))
      infile >> year >> tmparea >> tmpfleet >> tmpnumber >> ws;
    else
      infile >> year >> step >> tmparea >> tmpfleet >> tmpnumber >> ws;

    //if tmparea is in areaindex find areaid, else dont keep the data
    areaid = -1;
    for (i = 0; i < areaindex.Size(); i++)
      if (strcasecmp(areaindex[i], tmparea) == 0)
        areaid = i;

    if (areaid == -1)
      keepdata = 1;

    //if tmpfleet is a required fleet keep the data, else ignore it
    fleetid = -1;
    for (i = 0; i < fleetnames.Size(); i++)
      if (strcasecmp(fleetnames[i], tmpfleet) == 0)
        fleetid = i;

    if (fleetid == -1)
      keepdata = 1;

    //check if the year and step are in the simulation
    timeid = -1;
    if ((TimeInfo->isWithinPeriod(year, step)) && (keepdata == 0)) {
      //if this is a new timestep, resize to store the data
      for (i = 0; i < Years.Size(); i++)
        if ((Years[i] == year) && (yearly || (Steps[i] == step)))
          timeid = i;

      if (timeid == -1) {
        Years.resize(1, year);
        if (!(yearly))
          Steps.resize(1, step);
        timeid = (Years.Size() - 1);
        obsDistribution.AddRows(1, numarea, 0.0);
        modelDistribution.AddRows(1, numarea, 0.0);
        likelihoodValues.AddRows(1, numarea, 0.0);
      }

    } else {
      //dont keep the data
      keepdata = 1;
    }

    if (keepdata == 0) {
      //distribution data is required, so store it
      count++;
      //note that we use += to sum the data over all fleets (and possibly time)
      obsDistribution[timeid][areaid] += tmpnumber;
    }
  }

  if (yearly)
    AAT.addActionsAllSteps(Years, TimeInfo);
  else
    AAT.addActions(Years, Steps, TimeInfo);

  if ((handle.getLogLevel() >= LOGMESSAGE) && (count == 0))
    handle.logMessage(LOGWARN, "Warning in catchinkilos - found no data in the data file for", this->getName());
  if (handle.getLogLevel() >= LOGMESSAGE)
    handle.logMessage(LOGMESSAGE, "Read catchinkilos data file - number of entries", count);
}

void CatchInKilos::printLikelihood(ofstream& outfile, const TimeClass* const TimeInfo) {
  if (!AAT.atCurrentTime(TimeInfo))
    return;

  if ((yearly == 1) && (TimeInfo->getStep() != TimeInfo->numSteps()))
    return;  //if data is aggregated into years then we need the last timestep

  int i, area, age, len;
  timeindex = -1;
  if (yearly == 1) {
    for (i = 0; i < Years.Size(); i++)
      if (Years[i] == TimeInfo->getYear())
        timeindex = i;  
  } else {
    for (i = 0; i < Years.Size(); i++)
      if ((Years[i] == TimeInfo->getYear()) && (Steps[i] == TimeInfo->getStep()))
        timeindex = i;
  }

  if (timeindex == -1)
    handle.logMessage(LOGFAIL, "Error in catchinkilos - invalid timestep");

  for (area = 0; area < modelDistribution.Ncol(timeindex); area++) {
    if (yearly == 0) {
      outfile << setw(lowwidth) << Years[timeindex] << sep << setw(lowwidth)
        << Steps[timeindex] << sep << setw(printwidth) << areaindex[area];
    } else {
      outfile << setw(lowwidth) << Years[timeindex] << "  all "
        << setw(printwidth) << areaindex[area];
    }
    if (fleetnames.Size() == 1) {
      outfile << sep << setw(printwidth) << fleetnames[0] << sep;
    } else {
      outfile << "  all     ";
    }
    outfile << setprecision(largeprecision) << setw(largewidth)
      << modelDistribution[timeindex][area] << endl;
  }
}

void CatchInKilos::printSummary(ofstream& outfile) {
  int year, area;

  for (year = 0; year < likelihoodValues.Nrow(); year++) {
    for (area = 0; area < likelihoodValues.Ncol(year); area++) {
      if (yearly == 0) {
        outfile << setw(lowwidth) << Years[year] << sep << setw(lowwidth)
          << Steps[year] << sep << setw(printwidth) << areaindex[area] << sep
          << setw(largewidth) << this->getName() << sep << setprecision(smallprecision)
          << setw(smallwidth) << weight << sep << setprecision(largeprecision)
          << setw(largewidth) << likelihoodValues[year][area] << endl;
      } else {
        outfile << setw(lowwidth) << Years[year] << "  all "
          << setw(printwidth) << areaindex[area] << sep
          << setw(largewidth) << this->getName() << sep << setprecision(smallprecision)
          << setw(smallwidth) << weight << sep << setprecision(largeprecision)
          << setw(largewidth) << likelihoodValues[year][area] << endl;
      }
    }
  }
  outfile.flush();
}
