#include "surveydistribution.h"
#include "readfunc.h"
#include "readword.h"
#include "readaggregation.h"
#include "errorhandler.h"
#include "mathfunc.h"
#include "areatime.h"
#include "stock.h"
#include "suitfuncptrvector.h"
#include "gadget.h"
#include "global.h"

SurveyDistribution::SurveyDistribution(CommentStream& infile, const AreaClass* const Area,
  const TimeClass* const TimeInfo, Keeper* const keeper, double weight, const char* name)
  : Likelihood(SURVEYDISTRIBUTIONLIKELIHOOD, weight, name), alptr(0) {

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
  readWordAndValue(infile, "datafile", datafilename);

  timeindex = 0;
  fittype = new char[MaxStrLength];
  strncpy(fittype, "", MaxStrLength);
  liketype = new char[MaxStrLength];
  strncpy(liketype, "", MaxStrLength);

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

  //read in length aggregation from file
  readWordAndValue(infile, "lenaggfile", aggfilename);
  datafile.open(aggfilename, ios::binary);
  handle.checkIfFailure(datafile, aggfilename);
  handle.Open(aggfilename);
  numlen = readLengthAggregation(subdata, lengths, lenindex);
  handle.Close();
  datafile.close();
  datafile.clear();

  LgrpDiv = new LengthGroupDivision(lengths);
  if (LgrpDiv->Error())
    handle.logMessage(LOGFAIL, "Error in surveydistribution - failed to create length group");

  //read in age aggregation from file
  readWordAndValue(infile, "ageaggfile", aggfilename);
  datafile.open(aggfilename, ios::binary);
  handle.checkIfFailure(datafile, aggfilename);
  handle.Open(aggfilename);
  numage = readAggregation(subdata, ages, ageindex);
  handle.Close();
  datafile.close();
  datafile.clear();

  //read in the stocknames
  i = 0;
  infile >> text >> ws;
  if (strcasecmp(text, "stocknames") != 0)
    handle.logFileUnexpected(LOGFAIL, "stocknames", text);
  infile >> text;
  while (!infile.eof() && (strcasecmp(text, "fittype") != 0)) {
    infile >> ws;
    stocknames.resize(new char[strlen(text) + 1]);
    strcpy(stocknames[i++], text);
    infile >> text;
  }
  if (stocknames.Size() == 0)
    handle.logFileMessage(LOGFAIL, "\nError in surveydistribution - failed to read stocks");
  handle.logMessage(LOGMESSAGE, "Read stock data - number of stocks", stocknames.Size());

  infile >> fittype >> ws;
  fitnumber = 0;
  if (strcasecmp(fittype, "linearfit") == 0) {
    fitnumber = 1;
  } else if (strcasecmp(fittype, "powerfit") == 0) {
    fitnumber = 2;
  } else
    handle.logFileMessage(LOGFAIL, "\nError in surveydistribution - unrecognised fittype", fittype);

  parameters.resize(2, keeper);
  infile >> text >> ws;
  if (strcasecmp(text, "parameters") != 0)
    handle.logFileUnexpected(LOGFAIL, "parameters", text);
  parameters.read(infile, TimeInfo, keeper);

  suit.resize(LgrpDiv->numLengthGroups(), 0.0);
  infile >> text >> ws;
  if ((strcasecmp(text, "function") == 0)) {
    //read suitability function
    infile >> text >> ws;
    SuitFuncPtrVector tempsuitfunc;
    tempsuitfunc.readSuitFunction(infile, text, TimeInfo, keeper);
    suitfunction = tempsuitfunc[0];
    if (suitfunction->usesPredLength())
      suitfunction->setPredLength(0.0);

    for (i = 0; i < LgrpDiv->numLengthGroups(); i++) {
      if (suitfunction->usesPreyLength())
        suitfunction->setPreyLength(LgrpDiv->meanLength(i));

      suit[i] = suitfunction->calculate();
    }

  } else if (strcasecmp(text, "suitfile") == 0) {
    //read values from file
    for (i = 0; i < LgrpDiv->numLengthGroups(); i++)
      infile >> suit[i];

  } else {
    handle.logFileMessage(LOGFAIL, "\nError in surveydistribution - unrecognised suitability", text);
  }

  //JMB - changed to make the reading of epsilon optional
  infile >> ws;
  char c = infile.peek();
  if ((c == 'e') || (c == 'E'))
    readWordAndVariable(infile, "epsilon", epsilon);
  else
    epsilon = 1.0;

  if (epsilon < verysmall) {
    handle.logFileMessage(LOGWARN, "epsilon should be a positive integer - set to default value 1");
    epsilon = 1.0;
  }

  readWordAndValue(infile, "likelihoodtype", liketype);
  likenumber = 0;
  if (strcasecmp(liketype, "pearson") == 0)
    likenumber = 1;
  else if (strcasecmp(liketype, "multinomial") == 0)
    likenumber = 2;
  else if (strcasecmp(liketype, "gamma") == 0)
    likenumber = 3;
  else if (strcasecmp(liketype, "log") == 0)
    likenumber = 4;
  else
    handle.logFileMessage(LOGFAIL, "\nError in surveydistribution - unrecognised likelihoodtype", liketype);

  //read the survey distribution data from the datafile
  datafile.open(datafilename, ios::binary);
  handle.checkIfFailure(datafile, datafilename);
  handle.Open(datafilename);
  readDistributionData(subdata, TimeInfo, numarea, numage, numlen);
  handle.Close();
  datafile.close();
  datafile.clear();

  //prepare for next likelihood component
  infile >> ws;
  if (!infile.eof()) {
    infile >> text >> ws;
    if (strcasecmp(text, "[component]") != 0)
      handle.logFileUnexpected(LOGFAIL, "[component]", text);
  }
}

void SurveyDistribution::readDistributionData(CommentStream& infile,
  const TimeClass* TimeInfo, int numarea, int numage, int numlen) {

  double tmpnumber;
  char tmparea[MaxStrLength], tmpage[MaxStrLength], tmplen[MaxStrLength];
  strncpy(tmparea, "", MaxStrLength);
  strncpy(tmpage, "", MaxStrLength);
  strncpy(tmplen, "", MaxStrLength);
  int keepdata, timeid, areaid, ageid, lenid;
  int i, year, step, count, reject;

  //Check the number of columns in the inputfile
  if (countColumns(infile) != 6)
    handle.logFileMessage(LOGFAIL, "wrong number of columns in inputfile - should be 6");

  year = step = count = reject = 0;
  while (!infile.eof()) {
    keepdata = 1;
    infile >> year >> step >> tmparea >> tmpage >> tmplen >> tmpnumber >> ws;

    //crude check to see if something has gone wrong and avoid infinite loops
    if (strlen(tmparea) == 0)
      handle.logFileMessage(LOGFAIL, "failed to read data from file");

    //if tmparea is in areaindex keep data, else dont keep the data
    areaid = -1;
    for (i = 0; i < areaindex.Size(); i++)
      if (strcasecmp(areaindex[i], tmparea) == 0)
        areaid = i;

    if (areaid == -1)
      keepdata = 0;

    //if tmpage is in ageindex keep data, else dont keep the data
    ageid = -1;
    for (i = 0; i < ageindex.Size(); i++)
      if (strcasecmp(ageindex[i], tmpage) == 0)
        ageid = i;

    if (ageid == -1)
      keepdata = 0;

    //if tmplen is in lenindex keep data, else dont keep the data
    lenid = -1;
    for (i = 0; i < lenindex.Size(); i++)
      if (strcasecmp(lenindex[i], tmplen) == 0)
        lenid = i;

    if (lenid == -1)
      keepdata = 0;

    //check if the year and step are in the simulation
    timeid = -1;
    if ((TimeInfo->isWithinPeriod(year, step)) && (keepdata == 1)) {
      for (i = 0; i < Years.Size(); i++)
        if ((Years[i] == year) && (Steps[i] == step))
          timeid = i;

      if (timeid == -1) {
        Years.resize(1, year);
        Steps.resize(1, step);
        timeid = (Years.Size() - 1);

        obsDistribution.resize();
        modelDistribution.resize();
        likelihoodValues.AddRows(1, numarea, 0.0);
        for (i = 0; i < numarea; i++) {
          obsDistribution[timeid].resize(new DoubleMatrix(numage, numlen, 0.0));
          modelDistribution[timeid].resize(new DoubleMatrix(numage, numlen, 0.0));
        }
      }

    } else
      keepdata = 0;


    if (keepdata == 1) {
      //survey distribution data is required, so store it
      count++;
      (*obsDistribution[timeid][areaid])[ageid][lenid] = tmpnumber;
    } else
      reject++;  //count number of rejected data points read from file
  }

  AAT.addActions(Years, Steps, TimeInfo);
  if (count == 0)
    handle.logMessage(LOGWARN, "Warning in surveydistribution - found no data in the data file for", this->getName());

  if (Steps.Size() > 0) {
    //JMB - to be comparable, this should only take place on the same step in each year
    step = Steps[0];
    timeid = 0;
    for (i = 1; i < Steps.Size(); i++)
      if (Steps[i] != step)
        timeid++;

    if (timeid != 0)
      handle.logMessage(LOGWARN, "Warning in surveydistribution - differing timesteps for", this->getName());
  }

  if (reject != 0)
    handle.logMessage(LOGMESSAGE, "Discarded invalid surveydistribution data - number of invalid entries", reject);
  handle.logMessage(LOGMESSAGE, "Read surveydistribution data file - number of entries", count);
}

SurveyDistribution::~SurveyDistribution() {
  int i, j;
  for (i = 0; i < stocknames.Size(); i++)
    delete[] stocknames[i];
  for (i = 0; i < areaindex.Size(); i++)
    delete[] areaindex[i];
  for (i = 0; i < ageindex.Size(); i++)
    delete[] ageindex[i];
  for (i = 0; i < lenindex.Size(); i++)
    delete[] lenindex[i];

  if (suitfunction != NULL) {
    delete suitfunction;
    suitfunction = NULL;
  }
  for (i = 0; i < obsDistribution.Nrow(); i++) {
    for (j = 0; j < obsDistribution.Ncol(i); j++) {
      delete obsDistribution[i][j];
      delete modelDistribution[i][j];
    }
  }

  if (aggregator != 0)
    delete aggregator;
  delete LgrpDiv;
  delete[] fittype;
  delete[] liketype;
}

void SurveyDistribution::Reset(const Keeper* const keeper) {
  Likelihood::Reset(keeper);
  if (isZero(weight))
    handle.logMessage(LOGWARN, "Warning in surveydistribution - zero weight for", this->getName());
  int i, j;
  for (i = 0; i < modelDistribution.Nrow(); i++)
    for (j = 0; j < modelDistribution.Ncol(i); j++)
      (*modelDistribution[i][j]).setToZero();
  if (handle.getLogLevel() >= LOGMESSAGE)
    handle.logMessage(LOGMESSAGE, "Reset surveydistribution component", this->getName());
}

void SurveyDistribution::Print(ofstream& outfile) const {
  int i;
  outfile << "\nSurvey Distribution " << this->getName() << " - likelihood value " << likelihood
    << "\n\tFunction " << liketype << "\n\tStock names:";
  for (i = 0; i < stocknames.Size(); i++)
    outfile << sep << stocknames[i];
  outfile << endl;
  aggregator->Print(outfile);
  outfile.flush();
}

void SurveyDistribution::printLikelihood(ofstream& outfile, const TimeClass* const TimeInfo) {

  if (!AAT.atCurrentTime(TimeInfo))
    return;

  int i, area, age, len;
  timeindex = -1;
  for (i = 0; i < Years.Size(); i++)
    if ((Years[i] == TimeInfo->getYear()) && (Steps[i] == TimeInfo->getStep()))
      timeindex = i;
  if (timeindex == -1)
    handle.logMessage(LOGFAIL, "Error in surveydistribution - invalid timestep");

  for (area = 0; area < modelDistribution.Ncol(timeindex); area++) {
    for (age = 0; age < modelDistribution[timeindex][area]->Nrow(); age++) {
      for (len = 0; len < modelDistribution[timeindex][area]->Ncol(age); len++) {
        outfile << setw(lowwidth) << Years[timeindex] << sep << setw(lowwidth)
          << Steps[timeindex] << sep << setw(printwidth) << areaindex[area] << sep
          << setw(printwidth) << ageindex[age] << sep << setw(printwidth)
          << lenindex[len] << sep << setprecision(largeprecision) << setw(largewidth);

        //JMB crude filter to remove the 'silly' values from the output
        if ((*modelDistribution[timeindex][area])[age][len] < rathersmall)
          outfile << 0 << endl;
        else
          outfile << (*modelDistribution[timeindex][area])[age][len] << endl;
      }
    }
  }
}

void SurveyDistribution::setFleetsAndStocks(FleetPtrVector& Fleets, StockPtrVector& Stocks) {

  int i, j, k, found, minage, maxage;
  StockPtrVector stocks;

  for (i = 0; i < stocknames.Size(); i++) {
    found = 0;
    for (j = 0; j < Stocks.Size(); j++) {
      if (strcasecmp(stocknames[i], Stocks[j]->getName()) == 0) {
        found++;
        stocks.resize(Stocks[j]);
      }
    }
    if (found == 0)
      handle.logMessage(LOGFAIL, "Error in surveydistribution - failed to match stock", stocknames[i]);
  }

  for (i = 0; i < stocks.Size(); i++)
    for (j = 0; j < stocks.Size(); j++)
      if ((strcasecmp(stocks[i]->getName(), stocks[j]->getName()) == 0) && (i != j))
        handle.logMessage(LOGFAIL, "Error in surveydistribution - repeated stock", stocks[i]->getName());

  //check areas, ages and lengths
  if (handle.getLogLevel() >= LOGWARN) {
    for (j = 0; j < areas.Nrow(); j++) {
      found = 0;
      for (i = 0; i < stocks.Size(); i++)
        for (k = 0; k < areas.Ncol(j); k++)
          if (stocks[i]->isInArea(areas[j][k]))
            found++;
      if (found == 0)
        handle.logMessage(LOGWARN, "Warning in surveydistribution - stock not defined on all areas");
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
      handle.logMessage(LOGWARN, "Warning in surveydistribution - minimum age less than stock age");

    found = 0;
    for (i = 0; i < stocks.Size(); i++)
      if (maxage <= stocks[i]->maxAge())
        found++;
    if (found == 0)
      handle.logMessage(LOGWARN, "Warning in surveydistribution - maximum age greater than stock age");

    found = 0;
    for (i = 0; i < stocks.Size(); i++)
      if (LgrpDiv->maxLength(0) > stocks[i]->getLengthGroupDiv()->minLength())
        found++;
    if (found == 0)
      handle.logMessage(LOGWARN, "Warning in surveydistribution - minimum length group less than stock length");

    found = 0;
    for (i = 0; i < stocks.Size(); i++)
      if (LgrpDiv->minLength(LgrpDiv->numLengthGroups()) < stocks[i]->getLengthGroupDiv()->maxLength())
        found++;
    if (found == 0)
      handle.logMessage(LOGWARN, "Warning in surveydistribution - maximum length group greater than stock length");
  }

  aggregator = new StockAggregator(stocks, LgrpDiv, areas, ages);
}

void SurveyDistribution::calcIndex(const TimeClass* const TimeInfo) {
  //written by kgf 13/10 98

  int area, age, len;
  if (suitfunction != NULL) {
    suitfunction->updateConstants(TimeInfo);
    if ((timeindex == 0) || (suitfunction->didChange(TimeInfo))) {
      if (suitfunction->usesPredLength())
        suitfunction->setPredLength(0.0);

      for (len = 0; len < LgrpDiv->numLengthGroups(); len++) {
        if (suitfunction->usesPreyLength())
          suitfunction->setPreyLength(LgrpDiv->meanLength(len));

        suit[len] = suitfunction->calculate();
      }
    }
  }

  parameters.Update(TimeInfo);
  switch (fitnumber) {
    case 1:
      for (area = 0; area < areas.Nrow(); area++)
        for (age = (*alptr)[area].minAge(); age <= (*alptr)[area].maxAge(); age++)
          for (len = (*alptr)[area].minLength(age); len < (*alptr)[area].maxLength(age); len++)
            (*modelDistribution[timeindex][area])[age][len] = parameters[0] * suit[len] * (((*alptr)[area][age][len]).N + parameters[1]);
      break;
    case 2:
      for (area = 0; area < areas.Nrow(); area++)
        for (age = (*alptr)[area].minAge(); age <= (*alptr)[area].maxAge(); age++)
          for (len = (*alptr)[area].minLength(age); len < (*alptr)[area].maxLength(age); len++)
            (*modelDistribution[timeindex][area])[age][len] = parameters[0] * suit[len] * pow(((*alptr)[area][age][len]).N, parameters[1]);
      break;
    default:
      handle.logMessage(LOGWARN, "Warning in surveydistribution - unrecognised fittype", fittype);
      break;
  }
}

void SurveyDistribution::addLikelihood(const TimeClass* const TimeInfo) {

  if ((!(AAT.atCurrentTime(TimeInfo))) || (isZero(weight)))
    return;

  int i;
  timeindex = -1;
  for (i = 0; i < Years.Size(); i++)
    if ((Years[i] == TimeInfo->getYear()) && (Steps[i] == TimeInfo->getStep()))
      timeindex = i;
  if (timeindex == -1)
    handle.logMessage(LOGFAIL, "Error in surveydistribution - invalid timestep");

  double l = 0.0;
  aggregator->Sum();
  if (handle.getLogLevel() >= LOGMESSAGE)
    handle.logMessage(LOGMESSAGE, "Calculating likelihood score for surveydistribution component", this->getName());

  alptr = &aggregator->getSum();
  this->calcIndex(TimeInfo);
  switch (likenumber) {
    case 1:
      l = calcLikPearson();
      break;
    case 2:
      l = calcLikMultinomial();
      break;
    case 3:
      l = calcLikGamma();
      break;
    case 4:
      l = calcLikLog();
      break;
    default:
      handle.logMessage(LOGWARN, "Warning in surveydistribution - unrecognised likelihoodtype", liketype);
      break;
  }

  if (handle.getLogLevel() >= LOGMESSAGE)
    handle.logMessage(LOGMESSAGE, "The likelihood score for this component on this timestep is", l);
  likelihood += l;
}

double SurveyDistribution::calcLikMultinomial() {
  //written by kgf 30/10 98
  //Multinomial formula from H J Skaug

  double temp, total, obstotal, modtotal;
  int area, age, len;

  total = 0.0;
  for (area = 0; area < areas.Nrow(); area++) {
    temp = 0.0;
    obstotal = 0.0;
    modtotal = 0.0;
    for (age = 0; age < (*obsDistribution[timeindex][area]).Nrow(); age++) {
      for (len = 0; len < (*obsDistribution[timeindex][area]).Ncol(age); len++) {
        temp -= (*obsDistribution[timeindex][area])[age][len] *
                 log(((*modelDistribution[timeindex][area])[age][len]) + epsilon);
        obstotal += (*obsDistribution[timeindex][area])[age][len];
        modtotal += ((*modelDistribution[timeindex][area])[age][len] + epsilon);
      }
    }

    if ((modtotal < verysmall) && (!(isZero(obstotal)))) {
      likelihoodValues[timeindex][area] = 0.0;
    } else {
      temp /= obstotal;
      temp += log(modtotal);
      likelihoodValues[timeindex][area] = temp;
    }
    total += likelihoodValues[timeindex][area];
  }
  return total;
}

double SurveyDistribution::calcLikPearson() {
  //written by kgf 13/10 98

  double temp, total, diff;
  int area, age, len;

  total = 0.0;
  for (area = 0; area < areas.Nrow(); area++) {
    temp = 0.0;
    for (age = 0; age < (*obsDistribution[timeindex][area]).Nrow(); age++) {
      for (len = 0; len < (*obsDistribution[timeindex][area]).Ncol(age); len++) {
        diff = ((*modelDistribution[timeindex][area])[age][len] - (*obsDistribution[timeindex][area])[age][len]);
        diff *= diff;
        diff /= ((*modelDistribution[timeindex][area])[age][len] + epsilon);
        temp += diff;
      }
    }
    likelihoodValues[timeindex][area] = temp;
    total += likelihoodValues[timeindex][area];
  }
  return total;
}

double SurveyDistribution::calcLikGamma() {
  //written by kgf 24/5 00
  //The gamma likelihood function as described by
  //Hans J Skaug 15/3 00, at present without internal weighting.

  double total, temp;
  int area, age, len;

  total = 0.0;
  for (area = 0; area < areas.Nrow(); area++) {
    temp = 0.0;
    for (age = 0; age < (*obsDistribution[timeindex][area]).Nrow(); age++)
      for (len = 0; len < (*obsDistribution[timeindex][area]).Ncol(age); len++)
        temp += (((*obsDistribution[timeindex][area])[age][len] /
                 ((*modelDistribution[timeindex][area])[age][len] + epsilon)) +
                 log((*modelDistribution[timeindex][area])[age][len] + epsilon));

    likelihoodValues[timeindex][area] = temp;
    total += likelihoodValues[timeindex][area];
  }
  return total;
}

double SurveyDistribution::calcLikLog() {
  //corrected by kgf 27/11 98 to sum first and then take the logarithm
  double total, obstotal, modtotal;
  int area, age, len;

  total = 0.0;
  for (area = 0; area < areas.Nrow(); area++) {
    obstotal = 0.0;
    modtotal = 0.0;
    for (age = 0; age < (*obsDistribution[timeindex][area]).Nrow(); age++) {
      for (len = 0; len < (*obsDistribution[timeindex][area]).Ncol(age); len++) {
        modtotal += (*modelDistribution[timeindex][area])[age][len];
        obstotal += (*obsDistribution[timeindex][area])[age][len];
      }
    }

    if (!(isZero(modtotal)))
      likelihoodValues[timeindex][area] = (log(obstotal / modtotal) * log(obstotal / modtotal));
    else
      likelihoodValues[timeindex][area] = verybig;

    total += likelihoodValues[timeindex][area];
  }
  return total;
}

void SurveyDistribution::printSummary(ofstream& outfile) {
  int year, area;

  for (year = 0; year < likelihoodValues.Nrow(); year++) {
    for (area = 0; area < likelihoodValues.Ncol(year); area++) {
      outfile << setw(lowwidth) << Years[year] << sep << setw(lowwidth)
        << Steps[year] << sep << setw(printwidth) << areaindex[area] << sep
        << setw(largewidth) << this->getName() << sep << setprecision(smallprecision)
        << setw(smallwidth) << weight << sep << setprecision(largeprecision)
        << setw(largewidth) << likelihoodValues[year][area] << endl;
    }
  }
  outfile.flush();
}
