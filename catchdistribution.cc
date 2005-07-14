#include "catchdistribution.h"
#include "readfunc.h"
#include "readword.h"
#include "readaggregation.h"
#include "errorhandler.h"
#include "areatime.h"
#include "fleet.h"
#include "stock.h"
#include "multinomial.h"
#include "mathfunc.h"
#include "stockprey.h"
#include "ecosystem.h"
#include "ludecomposition.h"
#include "gadget.h"

extern ErrorHandler handle;

CatchDistribution::CatchDistribution(CommentStream& infile, const AreaClass* const Area,
  const TimeClass* const TimeInfo, Keeper* const keeper, double weight, const char* name)
  : Likelihood(CATCHDISTRIBUTIONLIKELIHOOD, weight, name), alptr(0) {

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
  yearly = 0;
  functionname = new char[MaxStrLength];
  strncpy(functionname, "", MaxStrLength);

  readWordAndValue(infile, "datafile", datafilename);
  readWordAndValue(infile, "function", functionname);

  functionnumber = 0;
  if (strcasecmp(functionname, "multinomial") == 0) {
    MN = Multinomial();
    functionnumber = 1;
  } else if (strcasecmp(functionname, "pearson") == 0) {
    functionnumber = 2;
  } else if (strcasecmp(functionname, "gamma") == 0) {
    functionnumber = 3;
  } else if (strcasecmp(functionname, "sumofsquares") == 0) {
    functionnumber = 4;
  } else if (strcasecmp(functionname, "mvn") == 0) {
    functionnumber = 5;

    readWordAndVariable(infile, "lag", lag);
    readWordAndFormula(infile, "sigma", sigma);
    sigma.Inform(keeper);

    params.resize(lag, keeper);
    for (i = 0; i < lag; i++)
      readWordAndFormula(infile, "param", params[i]);
    params.Inform(keeper);

  } else if (strcasecmp(functionname, "mvlogistic") == 0) {
    functionnumber = 6;

    readWordAndFormula(infile, "sigma", sigma);
    sigma.Inform(keeper);

  } else if (strcasecmp(functionname, "log") == 0) {
    //JMB moved the logcatch function to here instead of it being a seperate class
    functionnumber = 7;

  } else
    handle.logFileMessage(LOGFAIL, "Error in catchdistribution - unrecognised function", functionname);

  infile >> ws;
  char c = infile.peek();
  if ((c == 'a') || (c == 'A')) {
    //we have found either aggregationlevel or areaaggfile ...
    streampos pos = infile.tellg();

    infile >> text >> ws;
    if ((strcasecmp(text, "aggregation_level") == 0) || (strcasecmp(text, "aggregationlevel") == 0))
      infile >> yearly >> ws;
    else if (strcasecmp(text, "areaaggfile") == 0)
      infile.seekg(pos);
    else
      handle.logFileUnexpected(LOGFAIL, "areaaggfile", text);

    //JMB - peek at the next char
    c = infile.peek();

    if (yearly != 0 && yearly != 1)
      handle.logFileMessage(LOGFAIL, "Error in catchdistribution - aggregationlevel must be 0 or 1");
  }

  //JMB - changed to make the reading of overconsumption optional
  if ((c == 'o') || (c == 'O')) {
    readWordAndVariable(infile, "overconsumption", overconsumption);
    infile >> ws;
    c = infile.peek();
  } else
    overconsumption = 0;

  if (overconsumption != 0 && overconsumption != 1)
    handle.logFileMessage(LOGFAIL, "Error in catchdistribution - overconsumption must be 0 or 1");

  //JMB - changed to make the reading of minimum probability optional
  if ((c == 'm') || (c == 'M'))
    readWordAndVariable(infile, "minimumprobability", epsilon);
  else if ((c == 'e') || (c == 'E'))
    readWordAndVariable(infile, "epsilon", epsilon);
  else
    epsilon = 10.0;

  if (epsilon < verysmall) {
    handle.logFileMessage(LOGWARN, "Epsilon should be a positive integer - set to default value 10");
    epsilon = 10.0;
  }

  //read in area aggregation from file
  readWordAndValue(infile, "areaaggfile", aggfilename);
  datafile.open(aggfilename, ios::in);
  handle.checkIfFailure(datafile, aggfilename);
  handle.Open(aggfilename);
  numarea = readAggregation(subdata, areas, areaindex);
  handle.Close();
  datafile.close();
  datafile.clear();

  //read in age aggregation from file
  readWordAndValue(infile, "ageaggfile", aggfilename);
  datafile.open(aggfilename, ios::in);
  handle.checkIfFailure(datafile, aggfilename);
  handle.Open(aggfilename);
  numage = readAggregation(subdata, ages, ageindex);
  handle.Close();
  datafile.close();
  datafile.clear();

  //read in length aggregation from file
  readWordAndValue(infile, "lenaggfile", aggfilename);
  datafile.open(aggfilename, ios::in);
  handle.checkIfFailure(datafile, aggfilename);
  handle.Open(aggfilename);
  numlen = readLengthAggregation(subdata, lengths, lenindex);
  handle.Close();
  datafile.close();
  datafile.clear();

  LgrpDiv = new LengthGroupDivision(lengths);

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
    fleetnames.resize(1, new char[strlen(text) + 1]);
    strcpy(fleetnames[i++], text);
    infile >> text >> ws;
  }
  if (fleetnames.Size() == 0)
    handle.logFileMessage(LOGFAIL, "Error in catchdistribution - failed to read fleets");
  handle.logMessage(LOGMESSAGE, "Read fleet data - number of fleets", fleetnames.Size());

  //read in the stocknames
  i = 0;
  if (!(strcasecmp(text, "stocknames") == 0))
    handle.logFileUnexpected(LOGFAIL, "stocknames", text);
  infile >> text;
  while (!infile.eof() && !(strcasecmp(text, "[component]") == 0)) {
    infile >> ws;
    stocknames.resize(1, new char[strlen(text) + 1]);
    strcpy(stocknames[i++], text);
    infile >> text;
  }
  if (stocknames.Size() == 0)
    handle.logFileMessage(LOGFAIL, "Error in catchdistribution - failed to read stocks");
  handle.logMessage(LOGMESSAGE, "Read stock data - number of stocks", stocknames.Size());

  //We have now read in all the data from the main likelihood file
  //But we have to read in the statistics data from datafilename
  datafile.open(datafilename, ios::in);
  handle.checkIfFailure(datafile, datafilename);
  handle.Open(datafilename);
  readDistributionData(subdata, TimeInfo, numarea, numage, numlen);
  handle.Close();
  datafile.close();
  datafile.clear();

  switch (functionnumber) {
    case 2:
    case 3:
    case 7:
      modelYearData.resize(numarea);
      obsYearData.resize(numarea);
      for (i = 0; i < numarea; i++) {
        modelYearData[i] = new DoubleMatrix(numage, numlen, 0.0);
        obsYearData[i] = new DoubleMatrix(numage, numlen, 0.0);
      }
      break;
    case 1:
    case 4:
    case 5:
    case 6:
      if (yearly == 1)
        handle.logFileMessage(LOGWARN, "Warning in catchdistribution - yearly aggregation is ignored for function", functionname);
      break;
    default:
      handle.logMessage(LOGWARN, "Warning in catchdistribution - unrecognised function", functionname);
      break;
  }
}

void CatchDistribution::readDistributionData(CommentStream& infile,
  const TimeClass* TimeInfo, int numarea, int numage, int numlen) {

  int i;
  int year, step;
  double tmpnumber;
  char tmparea[MaxStrLength], tmpage[MaxStrLength], tmplen[MaxStrLength];
  strncpy(tmparea, "", MaxStrLength);
  strncpy(tmpage, "", MaxStrLength);
  strncpy(tmplen, "", MaxStrLength);
  int keepdata, timeid, ageid, areaid, lenid;
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
  if (countColumns(infile) != 6)
    handle.logFileMessage(LOGFAIL, "Wrong number of columns in inputfile - should be 6");

  while (!infile.eof()) {
    keepdata = 0;
    infile >> year >> step >> tmparea >> tmpage >> tmplen >> tmpnumber >> ws;

    //if tmparea is in areaindex find areaid, else dont keep the data
    areaid = -1;
    for (i = 0; i < areaindex.Size(); i++)
      if (strcasecmp(areaindex[i], tmparea) == 0)
        areaid = i;

    if (areaid == -1)
      keepdata = 1;

    //if tmpage is in ageindex find ageid, else dont keep the data
    ageid = -1;
    for (i = 0; i < ageindex.Size(); i++)
      if (strcasecmp(ageindex[i], tmpage) == 0)
        ageid = i;

    if (ageid == -1)
      keepdata = 1;

    //if tmplen is in lenindex find lenid, else dont keep the data
    lenid = -1;
    for (i = 0; i < lenindex.Size(); i++)
      if (strcasecmp(lenindex[i], tmplen) == 0)
        lenid = i;

    if (lenid == -1)
      keepdata = 1;

    //check if the year and step are in the simulation
    timeid = -1;
    if ((TimeInfo->isWithinPeriod(year, step)) && (keepdata == 0)) {
      //if this is a new timestep, resize to store the data
      for (i = 0; i < Years.Size(); i++)
        if ((Years[i] == year) && (Steps[i] == step))
          timeid = i;

      if (timeid == -1) {
        Years.resize(1, year);
        Steps.resize(1, step);
        timeid = (Years.Size() - 1);

        obsDistribution.AddRows(1, numarea);
        modelDistribution.AddRows(1, numarea);
        likelihoodValues.AddRows(1, numarea, 0.0);
        for (i = 0; i < numarea; i++) {
          obsDistribution[timeid][i] = new DoubleMatrix(numage, numlen, 0.0);
          modelDistribution[timeid][i] = new DoubleMatrix(numage, numlen, 0.0);
        }
      }

    } else {
      //dont keep the data
      keepdata = 1;
    }

    if (keepdata == 0) {
      //distribution data is required, so store it
      count++;
      (*obsDistribution[timeid][areaid])[ageid][lenid] = tmpnumber;
    }
  }

  AAT.addActions(Years, Steps, TimeInfo);
  if (count == 0)
    handle.logMessage(LOGWARN, "Warning in catchdistribution - found no data in the data file for", this->getName());
  handle.logMessage(LOGMESSAGE, "Read catchdistribution data file - number of entries", count);
}

CatchDistribution::~CatchDistribution() {
  int i, j;
  for (i = 0; i < stocknames.Size(); i++)
    delete[] stocknames[i];
  for (i = 0; i < fleetnames.Size(); i++)
    delete[] fleetnames[i];
  for (i = 0; i < areaindex.Size(); i++)
    delete[] areaindex[i];
  for (i = 0; i < ageindex.Size(); i++)
    delete[] ageindex[i];
  for (i = 0; i < lenindex.Size(); i++)
    delete[] lenindex[i];
  for (i = 0; i < obsDistribution.Nrow(); i++)
    for (j = 0; j < obsDistribution.Ncol(i); j++) {
      delete obsDistribution[i][j];
      delete modelDistribution[i][j];
    }
  for (i = 0; i < modelYearData.Size(); i++) {
    delete modelYearData[i];
    delete obsYearData[i];
  }
  delete aggregator;
  delete LgrpDiv;
  delete[] functionname;
}

void CatchDistribution::Reset(const Keeper* const keeper) {
  Likelihood::Reset(keeper);

  switch (functionnumber) {
    case 2:
    case 3:
    case 4:
    case 6:
    case 7:
      break;
    case 1:
      MN.setValue(epsilon);
      break;
    case 5:
      illegal = 0;
      this->calcCorrelation();
      if (illegal == 1 || LU.isIllegal() == 1)
        handle.logMessage(LOGWARN, "Warning in catchdistribution - multivariate normal out of bounds");
      break;
    default:
      handle.logMessage(LOGWARN, "Warning in catchdistribution - unrecognised function", functionname);
      break;
  }

  if (handle.getLogLevel() >= LOGMESSAGE)
    handle.logMessage(LOGMESSAGE, "Reset catchdistribution component", this->getName());
}

void CatchDistribution::Print(ofstream& outfile) const {

  int i;
  outfile << "\nCatch Distribution " << this->getName() << " - likelihood value " << likelihood
    << "\n\tFunction " << functionname << "\n\tStock names:";
  for (i = 0; i < stocknames.Size(); i++)
    outfile << sep << stocknames[i];
  outfile << "\n\tFleet names:";
  for (i = 0; i < fleetnames.Size(); i++)
    outfile << sep << fleetnames[i];
  outfile << endl;

  switch (functionnumber) {
    case 1:
    case 2:
    case 3:
    case 4:
    case 7:
      break;
    case 5:
      outfile << "\tMultivariate normal distribution parameters: sigma " << sigma;
      for (i = 0; i < lag; i++)
        outfile << " param" << i + 1 << " " << params[i];
      outfile << endl;
      break;
    case 6:
      outfile << "\tMultivariate logistic distribution parameter: sigma " << sigma << endl;
      break;
    default:
      handle.logMessage(LOGWARN, "Warning in catchdistribution - unrecognised function", functionname);
      break;
  }

  aggregator->Print(outfile);
  outfile.flush();
}

void CatchDistribution::printLikelihood(ofstream& outfile, const TimeClass* const TimeInfo) {

  if (!AAT.atCurrentTime(TimeInfo))
    return;

  int i, area, age, len;
  timeindex = -1;
  for (i = 0; i < Years.Size(); i++)
    if ((Years[i] == TimeInfo->getYear()) && (Steps[i] == TimeInfo->getStep()))
      timeindex = i;
  if (timeindex == -1)
    handle.logMessage(LOGFAIL, "Error in catchdistribution - invalid timestep");

  for (area = 0; area < modelDistribution.Ncol(timeindex); area++) {
    for (age = 0; age < modelDistribution[timeindex][area]->Nrow(); age++) {
      for (len = 0; len < modelDistribution[timeindex][area]->Ncol(age); len++) {
        outfile << setw(lowwidth) << Years[timeindex] << sep << setw(lowwidth)
          << Steps[timeindex] << sep << setw(printwidth) << areaindex[area] << sep
          << setw(printwidth) << ageindex[age] << sep << setw(printwidth)
          << lenindex[len] << sep << setprecision(largeprecision) << setw(largewidth)
          << (*modelDistribution[timeindex][area])[age][len] << endl;
      }
    }
  }
}

void CatchDistribution::setFleetsAndStocks(FleetPtrVector& Fleets, StockPtrVector& Stocks) {
  int i, j, k, found, minage, maxage;
  FleetPtrVector fleets;
  StockPtrVector stocks;

  for (i = 0; i < fleetnames.Size(); i++) {
    found = 0;
    for (j = 0; j < Fleets.Size(); j++) {
      if (strcasecmp(fleetnames[i], Fleets[j]->getName()) == 0) {
        found ++;
        fleets.resize(1, Fleets[j]);
      }
    }
    if (found == 0)
      handle.logMessage(LOGFAIL, "Error in catchdistribution - unrecognised fleet", fleetnames[i]);
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
      handle.logMessage(LOGFAIL, "Error in catchdistribution - unrecognised stock", stocknames[i]);
  }

  //check fleet areas and stock areas, ages and lengths
  if (handle.getLogLevel() >= LOGWARN) {
    for (j = 0; j < areas.Nrow(); j++) {
      found = 0;
      for (i = 0; i < fleets.Size(); i++)
        for (k = 0; k < areas.Ncol(j); k++)
          if (fleets[i]->isInArea(areas[j][k]))
            found++;
      if (found == 0)
        handle.logMessage(LOGWARN, "Warning in catchdistribution - fleet not defined on all areas");
    }

    for (j = 0; j < areas.Nrow(); j++) {
      found = 0;
      for (i = 0; i < stocks.Size(); i++)
        for (k = 0; k < areas.Ncol(j); k++)
          if (stocks[i]->isInArea(areas[j][k]))
            found++;
      if (found == 0)
        handle.logMessage(LOGWARN, "Warning in catchdistribution - stock not defined on all areas");
    }

    minage = 9999;
    maxage = 0;
    for (i = 0; i < ages.Nrow(); i++) {
      for (j = 0; j < ages.Ncol(i); j++) {
        minage = min(ages[i][j], minage);
        maxage = max(ages[i][j], maxage);
      }
    }

    found = 0;
    for (i = 0; i < stocks.Size(); i++)
      if (minage >= stocks[i]->minAge())
        found++;
    if (found == 0)
      handle.logMessage(LOGWARN, "Warning in catchdistribution - minimum age less than stock age");

    found = 0;
    for (i = 0; i < stocks.Size(); i++)
      if (maxage <= stocks[i]->maxAge())
        found++;
    if (found == 0)
      handle.logMessage(LOGWARN, "Warning in catchdistribution - maximum age greater than stock age");

    found = 0;
    for (i = 0; i < stocks.Size(); i++)
      if (LgrpDiv->maxLength(0) > stocks[i]->getLengthGroupDiv()->minLength())
        found++;
    if (found == 0)
      handle.logMessage(LOGWARN, "Warning in catchdistribution - minimum length group less than stock length");

    found = 0;
    for (i = 0; i < stocks.Size(); i++)
      if (LgrpDiv->minLength(LgrpDiv->numLengthGroups()) < stocks[i]->getLengthGroupDiv()->maxLength())
        found++;
    if (found == 0)
      handle.logMessage(LOGWARN, "Warning in catchdistribution - maximum length group greater than stock length");
  }

  aggregator = new FleetPreyAggregator(fleets, stocks, LgrpDiv, areas, ages, overconsumption);
}

void CatchDistribution::addLikelihood(const TimeClass* const TimeInfo) {

  if (!(AAT.atCurrentTime(TimeInfo)))
    return;

  int i;
  timeindex = -1;
  for (i = 0; i < Years.Size(); i++)
    if ((Years[i] == TimeInfo->getYear()) && (Steps[i] == TimeInfo->getStep()))
      timeindex = i;
  if (timeindex == -1)
    handle.logMessage(LOGFAIL, "Error in catchdistribution - invalid timestep");

  double l = 0.0;
  aggregator->Sum(TimeInfo);
  if ((handle.getLogLevel() >= LOGWARN) && (aggregator->checkCatchData() == 1))
    handle.logMessage(LOGWARN, "Warning in catchdistribution - zero catch found");
  alptr = &aggregator->getSum();

  switch (functionnumber) {
    case 1:
      l = calcLikMultinomial();
      break;
    case 2:
      l = calcLikPearson(TimeInfo);
      break;
    case 3:
      l = calcLikGamma(TimeInfo);
      break;
    case 4:
      l = calcLikSumSquares();
      break;
    case 5:
      l = calcLikMVNormal();
      break;
    case 6:
      l = calcLikMVLogistic();
      break;
    case 7:
      l = calcLikLog(TimeInfo);
      break;
    default:
      handle.logMessage(LOGWARN, "Warning in catchdistribution - unrecognised function", functionname);
      break;
  }

  if ((yearly == 0) || (TimeInfo->getStep() == TimeInfo->numSteps())) {
    likelihood += l;
    if (handle.getLogLevel() >= LOGMESSAGE) {
      handle.logMessage(LOGMESSAGE, "Calculating likelihood score for catchdistribution component", this->getName());
      handle.logMessage(LOGMESSAGE, "The likelihood score for this component on this timestep is", l);
    }
  }
}

double CatchDistribution::calcLikMultinomial() {
  int area, age, len;

  MN.Reset();
  //the object MN does most of the work, accumulating likelihood
  for (area = 0; area < areas.Nrow(); area++) {
    likelihoodValues[timeindex][area] = 0.0;
    for (age = (*alptr)[area].minAge(); age <= (*alptr)[area].maxAge(); age++)
      for (len = (*alptr)[area].minLength(age); len < (*alptr)[area].maxLength(age); len++)
        (*modelDistribution[timeindex][area])[age][len] = ((*alptr)[area][age][len]).N;

    if (obsDistribution[timeindex][area]->Nrow() == 1) {
      //Only one age-group, so calculate multinomial based on length distribution
      likelihoodValues[timeindex][area] +=
        MN.calcLogLikelihood((*obsDistribution[timeindex][area])[0],
          (*modelDistribution[timeindex][area])[0]);

    } else {
      //Many age-groups, so calculate multinomial based on age distribution per length group
      DoubleVector dist(obsDistribution[timeindex][area]->Nrow(), 0.0);
      DoubleVector data(obsDistribution[timeindex][area]->Nrow(), 0.0);
      for (len = 0; len < obsDistribution[timeindex][area]->Ncol(0); len++) {
        for (age = 0; age < obsDistribution[timeindex][area]->Nrow(); age++) {
          dist[age] = (*modelDistribution[timeindex][area])[age][len];
          data[age] = (*obsDistribution[timeindex][area])[age][len];
        }
        likelihoodValues[timeindex][area] += MN.calcLogLikelihood(data, dist);
      }
    }
  }
  return MN.getLogLikelihood();
}

double CatchDistribution::calcLikPearson(const TimeClass* const TimeInfo) {
  /* written by Hoskuldur Bjornsson 29/8 98
   * corrected by kgf 16/9 98
   * modified by kgf 11/11 98 to make it possible to sum up catches
   * and calculated catches on year basis.
   * Modified 3/5 99 by kgf to check the age intervals of the stock
   * and the catch data, and make use of the ages that are common
   * for the stock and the catch data.*/

  double totallikelihood = 0.0;
  int age, len, area;

  for (area = 0; area < areas.Nrow(); area++) {
    likelihoodValues[timeindex][area] = 0.0;
    if (TimeInfo->getStep() == 1) { //start of a new year
      for (age = (*alptr)[area].minAge(); age <= (*alptr)[area].maxAge(); age++) {
        for (len = (*alptr)[area].minLength(age); len < (*alptr)[area].maxLength(age); len++) {
          (*modelYearData[area])[age][len] = 0.0;
          (*obsYearData[area])[age][len] = 0.0;
        }
      }
    }

    //JMB - changed to remove the need to store minrow and mincol stuff ...
    for (age = (*alptr)[area].minAge(); age <= (*alptr)[area].maxAge(); age++) {
      for (len = (*alptr)[area].minLength(age); len < (*alptr)[area].maxLength(age); len++) {
        (*modelDistribution[timeindex][area])[age][len] = (*alptr)[area][age][len].N;
        (*modelYearData[area])[age][len] += (*modelDistribution[timeindex][area])[age][len];
        (*obsYearData[area])[age][len] += (*obsDistribution[timeindex][area])[age][len];
      }
    }

    if (yearly == 0) { //calculate likelihood on all steps
      for (age = (*alptr)[area].minAge(); age <= (*alptr)[area].maxAge(); age++) {
        for (len = (*alptr)[area].minLength(age); len < (*alptr)[area].maxLength(age); len++) {
          likelihoodValues[timeindex][area] +=
            ((*modelDistribution[timeindex][area])[age][len] -
            (*obsDistribution[timeindex][area])[age][len]) *
            ((*modelDistribution[timeindex][area])[age][len] -
            (*obsDistribution[timeindex][area])[age][len]) /
            absolute(((*modelDistribution[timeindex][area])[age][len] + epsilon));
        }
      }
      totallikelihood += likelihoodValues[timeindex][area];

    } else { //calculate likelihood on year basis
      if (TimeInfo->getStep() < TimeInfo->numSteps())
        likelihoodValues[timeindex][area] = 0.0;
      else { //last step in year, is to calc likelihood contribution
        for (age = (*alptr)[area].minAge(); age <= (*alptr)[area].maxAge(); age++) {
          for (len = (*alptr)[area].minLength(age); len < (*alptr)[area].maxLength(age); len++) {
            likelihoodValues[timeindex][area] +=
              ((*modelYearData[area])[age][len] - (*obsYearData[area])[age][len]) *
              ((*modelYearData[area])[age][len] - (*obsYearData[area])[age][len]) /
              absolute(((*modelYearData[area])[age][len] + epsilon));
          }
        }
        totallikelihood += likelihoodValues[timeindex][area];
      }
    }
  }
  return totallikelihood;
}

double CatchDistribution::calcLikGamma(const TimeClass* const TimeInfo) {
  //written kgf 24/5 00
  //Formula by Hans J Skaug, 15/3 00 No weighting at present.
  //This function is scale independent.

  double totallikelihood = 0.0;
  int age, len, area;

  for (area = 0; area < areas.Nrow(); area++) {
    likelihoodValues[timeindex][area] = 0.0;
    if (TimeInfo->getStep() == 1) { //start of a new year
      for (age = (*alptr)[area].minAge(); age <= (*alptr)[area].maxAge(); age++) {
        for (len = (*alptr)[area].minLength(age); len < (*alptr)[area].maxLength(age); len++) {
          (*modelYearData[area])[age][len] = 0.0;
          (*obsYearData[area])[age][len] = 0.0;
        }
      }
    }

    //JMB - changed to remove the need to store minrow and mincol stuff ...
    for (age = (*alptr)[area].minAge(); age <= (*alptr)[area].maxAge(); age++) {
      for (len = (*alptr)[area].minLength(age); len < (*alptr)[area].maxLength(age); len++) {
        (*modelDistribution[timeindex][area])[age][len] = (*alptr)[area][age][len].N;
        (*modelYearData[area])[age][len] += (*modelDistribution[timeindex][area])[age][len];
        (*obsYearData[area])[age][len] += (*obsDistribution[timeindex][area])[age][len];
      }
    }

    if (yearly == 0) { //calculate likelihood on all steps
      for (age = (*alptr)[area].minAge(); age <= (*alptr)[area].maxAge(); age++) {
        for (len = (*alptr)[area].minLength(age); len < (*alptr)[area].maxLength(age); len++) {
          likelihoodValues[timeindex][area] +=
            (*obsDistribution[timeindex][area])[age][len] /
            ((*modelDistribution[timeindex][area])[age][len] + epsilon) +
            log((*modelDistribution[timeindex][area])[age][len] + epsilon);
        }
      }
      totallikelihood += likelihoodValues[timeindex][area];

    } else { //calculate likelihood on year basis
      if (TimeInfo->getStep() < TimeInfo->numSteps())
        likelihoodValues[timeindex][area] = 0.0;
      else { //last step in year, is to calc likelihood contribution
        for (age = (*alptr)[area].minAge(); age <= (*alptr)[area].maxAge(); age++) {
          for (len = (*alptr)[area].minLength(age); len < (*alptr)[area].maxLength(age); len++) {
            likelihoodValues[timeindex][area] +=
              (*obsYearData[area])[age][len] / ((*modelYearData[area])[age][len] + epsilon) +
              log((*modelYearData[area])[age][len] + epsilon);
          }
        }
        totallikelihood += likelihoodValues[timeindex][area];
      }
    }
  }
  return totallikelihood;
}

double CatchDistribution::calcLikLog(const TimeClass* const TimeInfo) {
  //written by kgf 23/11 98 to get a better scaling of the stocks.
  //modified by kgf 27/11 98 to sum first and then take the logarithm

  double totallikelihood = 0.0;
  int area, age, len;
  double totalmodel, totaldata, ratio;

  for (area = 0; area < areas.Nrow(); area++) {
    likelihoodValues[timeindex][area] = 0.0;
    totalmodel = 0.0;
    totaldata = 0.0;
    if (TimeInfo->getStep() == 1) { //start of a new year
      for (age = (*alptr)[area].minAge(); age <= (*alptr)[area].maxAge(); age++) {
        for (len = (*alptr)[area].minLength(age); len < (*alptr)[area].maxLength(age); len++) {
          (*modelYearData[area])[age][len] = 0.0;
          (*obsYearData[area])[age][len] = 0.0;
        }
      }
    }

    //JMB - changed to remove the need to store minrow and mincol stuff ...
    for (age = (*alptr)[area].minAge(); age <= (*alptr)[area].maxAge(); age++) {
      for (len = (*alptr)[area].minLength(age); len < (*alptr)[area].maxLength(age); len++) {
        (*modelDistribution[timeindex][area])[age][len] = (*alptr)[area][age][len].N;
        (*modelYearData[area])[age][len] += (*modelDistribution[timeindex][area])[age][len];
        (*obsYearData[area])[age][len] += (*obsDistribution[timeindex][area])[age][len];
      }
    }

    if (yearly == 0) { //calculate likelihood on all steps
      for (age = (*alptr)[area].minAge(); age <= (*alptr)[area].maxAge(); age++) {
        for (len = (*alptr)[area].minLength(age); len < (*alptr)[area].maxLength(age); len++) {
          totalmodel += (*modelDistribution[timeindex][area])[age][len];
          totaldata += (*obsDistribution[timeindex][area])[age][len];
        }
      }
      ratio = log(totaldata / totalmodel);
      likelihoodValues[timeindex][area] += (ratio * ratio);

    } else { //calculate likelihood on year basis
      if (TimeInfo->getStep() < TimeInfo->numSteps())
        likelihoodValues[timeindex][area] = 0.0;
      else { //last step in year, is to calculate likelihood contribution
        for (age = (*alptr)[area].minAge(); age <= (*alptr)[area].maxAge(); age++) {
          for (len = (*alptr)[area].minLength(age); len < (*alptr)[area].maxLength(age); len++) {
            totalmodel += (*modelYearData[area])[age][len];
            totaldata += (*obsYearData[area])[age][len];
          }
        }
        ratio = log(totaldata / totalmodel);
        likelihoodValues[timeindex][area] += (ratio * ratio);
      }
    }
    totallikelihood += likelihoodValues[timeindex][area];
  }
  return totallikelihood;
}

double CatchDistribution::calcLikSumSquares() {

  double totallikelihood = 0.0, temp = 0.0;
  double totalmodel, totaldata;
  int age, len, area;

  for (area = 0; area < areas.Nrow(); area++) {
    totalmodel = 0.0;
    totaldata = 0.0;
    likelihoodValues[timeindex][area] = 0.0;
    for (age = (*alptr)[area].minAge(); age <= (*alptr)[area].maxAge(); age++) {
      for (len = (*alptr)[area].minLength(age); len < (*alptr)[area].maxLength(age); len++) {
        (*modelDistribution[timeindex][area])[age][len] = ((*alptr)[area][age][len]).N;
        totalmodel += (*modelDistribution[timeindex][area])[age][len];
        totaldata += (*obsDistribution[timeindex][area])[age][len];
      }
    }

    for (age = (*alptr)[area].minAge(); age <= (*alptr)[area].maxAge(); age++) {
      for (len = (*alptr)[area].minLength(age); len < (*alptr)[area].maxLength(age); len++) {
        if ((isZero(totaldata)) && (isZero(totalmodel)))
          temp = 0.0;
        else if (isZero(totaldata))
          temp = (*modelDistribution[timeindex][area])[age][len] / totalmodel;
        else if (isZero(totalmodel))
          temp = (*obsDistribution[timeindex][area])[age][len] / totaldata;
        else
          temp = (((*obsDistribution[timeindex][area])[age][len] / totaldata)
            - ((*modelDistribution[timeindex][area])[age][len] / totalmodel));

        likelihoodValues[timeindex][area] += (temp * temp);
      }
    }
    totallikelihood += likelihoodValues[timeindex][area];
  }
  return totallikelihood;
}

void CatchDistribution::calcCorrelation() {
  int i, j, l, p;
  p = LgrpDiv->numLengthGroups();
  DoubleMatrix correlation(p, p, 0.0);

  for (i = 0; i < lag; i++)
    if (abs(params[i] - 1.0) >= 1.0)
      illegal = 1;

  if (illegal == 0) {
    for (i = 0; i < p; i++) {
      for (j = 0; j <= i; j++) {
        for (l = 1; l <= lag; l++) {
          if ((i - l) >= 0) {
            correlation[i][j] += (params[l - 1] - 1.0) * correlation[i - l][j];
            correlation[j][i] += (params[l - 1] - 1.0) * correlation[i - l][j];
          }
        }
      }
      correlation[i][i] += sigma * sigma;
    }
    LU = LUDecomposition(correlation);
  }
}


double CatchDistribution::calcLikMVNormal() {

  double totallikelihood = 0.0;
  double sumdata, sumdist;
  int age, len, area;

  if (illegal == 1 || LU.isIllegal() == 1 || isZero(sigma))
    return verybig;

  DoubleVector diff(LgrpDiv->numLengthGroups(), 0.0);
  for (area = 0; area < areas.Nrow(); area++) {
    sumdata = 0.0;
    sumdist = 0.0;
    likelihoodValues[timeindex][area] = 0.0;
    for (age = (*alptr)[area].minAge(); age <= (*alptr)[area].maxAge(); age++) {
      for (len = (*alptr)[area].minLength(age); len < (*alptr)[area].maxLength(age); len++) {
        (*modelDistribution[timeindex][area])[age][len] = ((*alptr)[area][age][len]).N;
        sumdata += (*obsDistribution[timeindex][area])[age][len];
        sumdist += (*modelDistribution[timeindex][area])[age][len];
      }
    }

    if (isZero(sumdata))
      sumdata = verybig;
    else
      sumdata = 1.0 / sumdata;
    if (isZero(sumdist))
      sumdist = verybig;
    else
      sumdist = 1.0 / sumdist;

    for (age = (*alptr)[area].minAge(); age <= (*alptr)[area].maxAge(); age++) {
      for (len = 0; len < diff.Size(); len++)
        diff[len] = 0.0;

      for (len = (*alptr)[area].minLength(age); len < (*alptr)[area].maxLength(age); len++)
        diff[len] = ((*obsDistribution[timeindex][area])[age][len] * sumdata)
                    - ((*modelDistribution[timeindex][area])[age][len] * sumdist);

      likelihoodValues[timeindex][area] += diff * LU.Solve(diff);
    }
    totallikelihood += likelihoodValues[timeindex][area];
  }

  totallikelihood += LU.getLogDet() * alptr->Size();
  return totallikelihood;
}

double CatchDistribution::calcLikMVLogistic() {

  double totallikelihood = 0.0;
  double sumdata = 0.0, sumdist = 0.0, sumnu = 0.0;
  int age, len, area, p;

  p = LgrpDiv->numLengthGroups();
  DoubleVector nu(p, 0.0);

  for (area = 0; area < areas.Nrow(); area++) {
    likelihoodValues[timeindex][area] = 0.0;
    for (age = (*alptr)[area].minAge(); age <= (*alptr)[area].maxAge(); age++) {
      for (len = 0; len < nu.Size(); len++)
        nu[len] = 0.0;

      sumdata = 0.0;
      sumdist = 0.0;
      for (len = (*alptr)[area].minLength(age); len < (*alptr)[area].maxLength(age); len++) {
        (*modelDistribution[timeindex][area])[age][len] = ((*alptr)[area][age][len]).N;
        sumdata += (*obsDistribution[timeindex][area])[age][len];
        sumdist += (*modelDistribution[timeindex][area])[age][len];
      }

      if (isZero(sumdata))
        sumdata = verybig;
      else
        sumdata = 1.0 / sumdata;
      if (isZero(sumdist))
        sumdist = verybig;
      else
        sumdist = 1.0 / sumdist;

      sumnu = 0.0;
      for (len = (*alptr)[area].minLength(age); len < (*alptr)[area].maxLength(age); len++) {
        nu[len] = log(((*obsDistribution[timeindex][area])[age][len] * sumdata) + verysmall)
                  - log(((*modelDistribution[timeindex][area])[age][len] * sumdist) + verysmall);

        sumnu += nu[len];
      }
      sumnu = sumnu / p;

      for (len = (*alptr)[area].minLength(age); len < (*alptr)[area].maxLength(age); len++)
        likelihoodValues[timeindex][area] += (nu[len] - sumnu) * (nu[len] - sumnu);
    }
    totallikelihood += likelihoodValues[timeindex][area];
  }

  if (isZero(sigma)) {
    handle.logMessage(LOGWARN, "Warning in catchdistribution - multivariate logistic sigma is zero");
    return verybig;
  }

  totallikelihood = (totallikelihood / (sigma * sigma)) + (log(sigma) * (p - 1));
  return totallikelihood;
}

void CatchDistribution::printSummary(ofstream& outfile) {
  int year, area;

  for (year = 0; year < likelihoodValues.Nrow(); year++) {
    for (area = 0; area < likelihoodValues.Ncol(year); area++) {
      if (yearly == 0) {
        outfile << setw(lowwidth) << Years[year] << sep << setw(lowwidth)
          << Steps[year] << sep << setw(printwidth) << areaindex[area] << sep
          << setw(largewidth) << this->getName() << sep << setw(smallwidth) << weight
          << sep << setprecision(largeprecision) << setw(largewidth)
          << likelihoodValues[year][area] << endl;
      } else {
        if (isZero(likelihoodValues[year][area])) {
          // assume that this isnt the last step for that year and ignore
        } else {
          outfile << setw(lowwidth) << Years[year] << "  all "
            << setw(printwidth) << areaindex[area] << sep
            << setw(largewidth) << this->getName() << sep << setprecision(smallprecision)
            << setw(smallwidth) << weight << sep << setprecision(largeprecision)
            << setw(largewidth) << likelihoodValues[year][area] << endl;
        }
      }
    }
  }
  outfile.flush();
}
