#include "catchstatistics.h"
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

CatchStatistics::CatchStatistics(CommentStream& infile, const AreaClass* const Area,
				 const TimeClass* const TimeInfo, double weight, const char* name)
  : Likelihood(CATCHSTATISTICSLIKELIHOOD, weight, name), alptr(0) {

  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  int i, j;
  int numarea = 0, numage = 0, numlen = 0;
  
  char datafilename[MaxStrLength];
  char aggfilename[MaxStrLength];
  strncpy(datafilename, "", MaxStrLength);
  strncpy(aggfilename, "", MaxStrLength);
  ifstream datafile;
  CommentStream subdata(datafile);

  timeindex = 0;
  functionname = new char[MaxStrLength];
  strncpy(functionname, "", MaxStrLength);
  readWordAndValue(infile, "datafile", datafilename);
  readWordAndValue(infile, "function", functionname);

  //JMB - changed to make the reading of overconsumption optional
  infile >> ws;
  char c = infile.peek();
  if ((c == 'o') || (c == 'O'))
    readWordAndVariable(infile, "overconsumption", overconsumption);
  else
    overconsumption = 0;

  if (overconsumption != 0 && overconsumption != 1)
    handle.logFileMessage(LOGFAIL, "\nError in catchstatistics - overconsumption must be 0 or 1");

  if ((strcasecmp(functionname, "lengthcalcvar") == 0) || (strcasecmp(functionname, "lengthcalcstddev") == 0))
    functionnumber = 1;
  else if ((strcasecmp(functionname, "lengthgivenvar") == 0) || (strcasecmp(functionname, "lengthgivenstddev") == 0))
    functionnumber = 2;
  else if ((strcasecmp(functionname, "weiguhtgivenvar") == 0) || (strcasecmp(functionname, "weightgivenstddev") == 0))
    functionnumber = 3;
  else if ((strcasecmp(functionname, "weightnovar") == 0) || (strcasecmp(functionname, "weightnostddev") == 0))
    functionnumber = 4;
  else if ((strcasecmp(functionname, "lengthnovar") == 0) || (strcasecmp(functionname, "lengthnostddev") == 0))
    functionnumber = 5;
  else if ((strcasecmp(functionname, "weightgivenvarlen") == 0) || (strcasecmp(functionname, "weightgivenstddevlen") == 0))
    functionnumber = 6;
  else if ((strcasecmp(functionname, "weightnovarlen") == 0) || (strcasecmp(functionname, "weightnostddevlen") == 0))
    functionnumber = 7;  
  else
    handle.logFileMessage(LOGFAIL, "\nError in catchstatistics - unrecognised function", functionname);

  //read in area aggregation from file
  readWordAndValue(infile, "areaaggfile", aggfilename);
  datafile.open(aggfilename, ios::binary);
  handle.checkIfFailure(datafile, aggfilename);
  handle.Open(aggfilename);
  numarea = readAggregation(subdata, areas, areaindex);
  handle.Close();
  datafile.close();
  datafile.clear();

  //read optionally from length aggregation file
  if ((functionnumber==6) || (functionnumber==7)){
    readWordAndValue(infile, "lenaggfile", aggfilename);
    datafile.open(aggfilename, ios::binary);
    handle.checkIfFailure(datafile, aggfilename);
    handle.Open(aggfilename);
    numlen = readAggregation(subdata, lengths, lenindex);
    handle.Close();
    datafile.close();
    datafile.clear();
  } 

  //read in age aggregation from file
  readWordAndValue(infile, "ageaggfile", aggfilename);
  datafile.open(aggfilename, ios::binary);
  handle.checkIfFailure(datafile, aggfilename);
  handle.Open(aggfilename);
  numage = readAggregation(subdata, ages, ageindex);
  handle.Close();
  datafile.close();
  datafile.clear();
  
  //Must change from outer areas to inner areas.
  for (i = 0; i < areas.Nrow(); i++)
    for (j = 0; j < areas.Ncol(i); j++){
      areas[i][j] = Area->getInnerArea(areas[i][j]);
    }

  //read in the fleetnames
  i = 0;
  infile >> text >> ws;
  if (strcasecmp(text, "fleetnames") != 0)
    handle.logFileUnexpected(LOGFAIL, "fleetnames", text);
  infile >> text >> ws;
  while (!infile.eof() && (strcasecmp(text, "stocknames") != 0)) {
    fleetnames.resize(new char[strlen(text) + 1]);
    strcpy(fleetnames[i++], text);
    infile >> text >> ws;
  }
  if (fleetnames.Size() == 0)
    handle.logFileMessage(LOGFAIL, "\nError in catchstatistics - failed to read fleets");
  handle.logMessage(LOGMESSAGE, "Read fleet data - number of fleets", fleetnames.Size());

  //read in the stocknames
  i = 0;
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
    handle.logFileMessage(LOGFAIL, "\nError in catchstatistics - failed to read stocks");
  handle.logMessage(LOGMESSAGE, "Read stock data - number of stocks", stocknames.Size());

  //We have now read in all the data from the main likelihood file
  //But we have to read in the statistics data from datafilename
  //different data names if statistics is by length
  if ((functionnumber==6) || (functionnumber==7)){
    datafile.open(datafilename, ios::binary);
    handle.checkIfFailure(datafile, datafilename);
    handle.Open(datafilename);
    readStatisticsData(subdata, TimeInfo, numarea, 0, numlen);
    handle.Close();
    datafile.close();
    datafile.clear();
  } else {
    datafile.open(datafilename, ios::binary);
    handle.checkIfFailure(datafile, datafilename);
    handle.Open(datafilename);
    readStatisticsData(subdata, TimeInfo, numarea, numage, 0);
    handle.Close();
    datafile.close();
    datafile.clear();
  }
}

void CatchStatistics::readStatisticsData(CommentStream& infile,
					 const TimeClass* TimeInfo, int numarea, int numage, int numlen) {

  int i, year, step;
  double tmpnumber, tmpmean, tmpstddev;
  char tmparea[MaxStrLength], tmpage[MaxStrLength], tmplen[MaxStrLength];
  strncpy(tmparea, "", MaxStrLength);
  strncpy(tmpage, "", MaxStrLength);
  strncpy(tmplen, "", MaxStrLength);
  int keepdata, needvar, readvar;
  int timeid, ageid, lenid, areaid, count, reject;
  
  //NK - add cases 6 and 7 (weight at length)
  readvar = 0;
  needvar = 0;
  switch (functionnumber) {
  case 1:
    readvar = 0;
    needvar = 1;
    break;
  case 2:
  case 3:
  case 6:
    readvar = 1;
    needvar = 1;
    break;
  case 4:
  case 5:
  case 7:
    break;
  default:
    handle.logMessage(LOGWARN, "Warning in catchstatistics 1 - unrecognised function", functionname);
    break;
  }

//Check the number of columns in the inputfile
  infile >> ws;

  if ((readvar) && (countColumns(infile) != 7))
    handle.logFileMessage(LOGFAIL, "wrong number of columns in inputfile - should be 7");
  else if ((!readvar) && (countColumns(infile) != 6))
    handle.logFileMessage(LOGFAIL, "wrong number of columns in inputfile - should be 6");


  year = step = count = reject = 0;
  while (!infile.eof()) {
   
    if ((functionnumber==6) || (functionnumber==7)){
      keepdata = 1;
      if (readvar)
	infile >> year >> step >> tmparea >> tmplen >> tmpnumber >> tmpmean >> tmpstddev >> ws;
      else
	infile >> year >> step >> tmparea >> tmplen >> tmpnumber >> tmpmean >> ws;
       
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
    
      //if tmplen is in lenindex find lenid, else dont keep the data
      lenid = -1;
      for (i = 0; i < lenindex.Size(); i++)
	if (strcasecmp(lenindex[i], tmplen) == 0)
	  lenid = i;

      if (lenid == -1)
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
	  likelihoodValues.AddRows(1, numarea, 0.0);
	  numbers.resize(new DoubleMatrix(numarea, numlen, 0.0));
	  obsMean.resize(new DoubleMatrix(numarea, numlen, 0.0));
	  modelMean.resize(new DoubleMatrix(numarea, numlen, 0.0));
	  if (readvar)
	    obsStdDev.resize(new DoubleMatrix(numarea, numlen, 0.0));
	  if (needvar)
	    modelStdDev.resize(new DoubleMatrix(numarea, numlen, 0.0));
	  
	  timeid = (Years.Size() - 1);
	}

      } else
	keepdata = 0;
      
      if (keepdata == 1) {
	//statistics data is required, so store it
	count++;
	(*numbers[timeid])[areaid][lenid] = tmpnumber;
	(*obsMean[timeid])[areaid][lenid] = tmpmean;
	if (readvar)
	  (*obsStdDev[timeid])[areaid][lenid] = tmpstddev;
      } else
	reject++;  //count number of rejected data points read from file

    }else{
      keepdata = 1;
      if (readvar)
	infile >> year >> step >> tmparea >> tmpage >> tmpnumber >> tmpmean >> tmpstddev >> ws;
      else
	infile >> year >> step >> tmparea >> tmpage >> tmpnumber >> tmpmean >> ws;

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

      //if tmpage is in ageindex find ageid, else dont keep the data
      ageid = -1;
      for (i = 0; i < ageindex.Size(); i++)
	if (strcasecmp(ageindex[i], tmpage) == 0)
	  ageid = i;
      
      if (ageid == -1)
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
	  likelihoodValues.AddRows(1, numarea, 0.0);
	  numbers.resize(new DoubleMatrix(numarea, numage, 0.0));
	  obsMean.resize(new DoubleMatrix(numarea, numage, 0.0));
	  modelMean.resize(new DoubleMatrix(numarea, numage, 0.0));
	  if (readvar)
	    obsStdDev.resize(new DoubleMatrix(numarea, numage, 0.0));
	  if (needvar)
	    modelStdDev.resize(new DoubleMatrix(numarea, numage, 0.0));
	  timeid = (Years.Size() - 1);
	}

      } else
	keepdata = 0;
      
      if (keepdata == 1) {
	//statistics data is required, so store it
	count++;
	(*numbers[timeid])[areaid][ageid] = tmpnumber;
	(*obsMean[timeid])[areaid][ageid] = tmpmean;
	if (readvar)
	  (*obsStdDev[timeid])[areaid][ageid] = tmpstddev;
      } else
	reject++;  //count number of rejected data points read from file
    }
  }

  AAT.addActions(Years, Steps, TimeInfo);
  if (count == 0)
    handle.logMessage(LOGWARN, "Warning in catchstatistics - found no data in the data file for", this->getName());
  if (reject != 0)
    handle.logMessage(LOGMESSAGE, "Discarded invalid catchstatistics data - number of invalid entries", reject);
  handle.logMessage(LOGMESSAGE, "Read catchstatistics data file - number of entries", count);
}

CatchStatistics::~CatchStatistics() {
  int i;
  for (i = 0; i < stocknames.Size(); i++)
    delete[] stocknames[i];
  for (i = 0; i < fleetnames.Size(); i++)
    delete[] fleetnames[i];
  for (i = 0; i < areaindex.Size(); i++)
    delete[] areaindex[i];
  for (i = 0; i < lenindex.Size(); i++)
    delete[] lenindex[i];
  for (i = 0; i < ageindex.Size(); i++)
    delete[] ageindex[i];
  delete aggregator;
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
  delete LgrpDiv;

}

void CatchStatistics::Reset(const Keeper* const keeper) {
  Likelihood::Reset(keeper);
  if (isZero(weight))
    handle.logMessage(LOGWARN, "Warning in catchstatistics - zero weight for", this->getName());
  int i;
  for (i = 0; i < modelMean.Size(); i++)
    (*modelMean[i]).setToZero();
  if (handle.getLogLevel() >= LOGMESSAGE)
    handle.logMessage(LOGMESSAGE, "Reset catchstatistics component", this->getName());
}

void CatchStatistics::Print(ofstream& outfile) const {
  int i;

  outfile << "\nCatch Statistics " << this->getName() << " - likelihood value " << likelihood
    << "\n\tFunction " << functionname << "\n\tStock names:";
  for (i = 0; i < stocknames.Size(); i++)
    outfile << sep << stocknames[i];
  outfile << "\n\tFleet names:";
  for (i = 0; i < fleetnames.Size(); i++)
    outfile << sep << fleetnames[i];
  outfile << endl;
  aggregator->Print(outfile);
  outfile.flush();
}

void CatchStatistics::setFleetsAndStocks(FleetPtrVector& Fleets, StockPtrVector& Stocks) {
  int i, j, k, found, minage, maxage;
  FleetPtrVector fleets;
  StockPtrVector stocks;

  for (i = 0; i < fleetnames.Size(); i++) {
    found = 0;
    for (j = 0; j < Fleets.Size(); j++) {
      if (strcasecmp(fleetnames[i], Fleets[j]->getName()) == 0) {
        found++;
        fleets.resize(Fleets[j]);
      }
    }
    if (found == 0)
      handle.logMessage(LOGFAIL, "Error in catchstatistics - unrecognised fleet", fleetnames[i]);
  }

  for (i = 0; i < fleets.Size(); i++)
    for (j = 0; j < fleets.Size(); j++)
      if ((strcasecmp(fleets[i]->getName(), fleets[j]->getName()) == 0) && (i != j))
        handle.logMessage(LOGFAIL, "Error in catchstatistics - repeated fleet", fleets[i]->getName());

  for (i = 0; i < stocknames.Size(); i++) {
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
      handle.logMessage(LOGFAIL, "Error in catchstatistics - unrecognised stock", stocknames[i]);
  }

  for (i = 0; i < stocks.Size(); i++)
    for (j = 0; j < stocks.Size(); j++)
      if ((strcasecmp(stocks[i]->getName(), stocks[j]->getName()) == 0) && (i != j))
        handle.logMessage(LOGFAIL, "Error in catchstatistics - repeated stock", stocks[i]->getName());

  for (i = 0; i < stocks.Size(); i++)
    LgrpDiv = new LengthGroupDivision(*(stocks[i]->getPrey()->getLengthGroupDiv()));
  for (i = 0; i < stocks.Size(); i++)
    if (!LgrpDiv->Combine(stocks[i]->getPrey()->getLengthGroupDiv()))
      handle.logMessage(LOGFAIL, "Error in catchstatistics - length groups for preys not compatible");

  if (LgrpDiv->Error())
    handle.logMessage(LOGFAIL, "Error in catchstatistics - failed to create length group");

  //check fleet areas and stock areas, ages and lengths
  if (handle.getLogLevel() >= LOGWARN) {
    for (j = 0; j < areas.Nrow(); j++) {
      found = 0;
      for (i = 0; i < fleets.Size(); i++)
        for (k = 0; k < areas.Ncol(j); k++)
          if (fleets[i]->isInArea(areas[j][k]))
            found++;
      if (found == 0)
        handle.logMessage(LOGWARN, "Warning in catchstatistics - fleet not defined on all areas");
    }

    for (j = 0; j < areas.Nrow(); j++) {
      found = 0;
      for (i = 0; i < stocks.Size(); i++)
        for (k = 0; k < areas.Ncol(j); k++)
          if (stocks[i]->isInArea(areas[j][k]))
            found++;
      if (found == 0)
        handle.logMessage(LOGWARN, "Warning in catchstatistics - stock not defined on all areas");
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
      handle.logMessage(LOGWARN, "Warning in catchstatistics - minimum age less than stock age");

    found = 0;
    for (i = 0; i < stocks.Size(); i++)
      if (maxage <= stocks[i]->maxAge())
        found++;
    if (found == 0)
      handle.logMessage(LOGWARN, "Warning in catchstatistics - maximum age greater than stock age");

    found = 0;
    for (i = 0; i < stocks.Size(); i++)
      if (LgrpDiv->maxLength(0) > stocks[i]->getLengthGroupDiv()->minLength())
        found++;
    if (found == 0)
      handle.logMessage(LOGWARN, "Warning in catchstatistics - minimum length group less than stock length");

    found = 0;
    for (i = 0; i < stocks.Size(); i++)
      if (LgrpDiv->minLength(LgrpDiv->numLengthGroups()) < stocks[i]->getLengthGroupDiv()->maxLength())
        found++;
    if (found == 0)
      handle.logMessage(LOGWARN, "Warning in catchstatistics - maximum length group greater than stock length");
  }

  aggregator = new FleetPreyAggregator(fleets, stocks, LgrpDiv, areas, ages, overconsumption);
}


void CatchStatistics::addLikelihood(const TimeClass* const TimeInfo) {

  if ((!(AAT.atCurrentTime(TimeInfo))) || (isZero(weight)))
    return;

  int i;
  timeindex = -1;
  for (i = 0; i < Years.Size(); i++)
    if ((Years[i] == TimeInfo->getYear()) && (Steps[i] == TimeInfo->getStep()))
      timeindex = i;
  if (timeindex == -1)
    handle.logMessage(LOGFAIL, "Error in catchstatistics - invalid timestep");


  double l = 0.0;
  if (handle.getLogLevel() >= LOGMESSAGE)
    handle.logMessage(LOGMESSAGE, "Calculating likelihood score for catchstatistics component", this->getName());
  aggregator->Sum();
  if ((handle.getLogLevel() >= LOGWARN) && (aggregator->checkCatchData()))
    handle.logMessage(LOGWARN, "Warning in catchstatistics - zero catch found");

  l = calcLikSumSquares();
  likelihood += l;
  if (handle.getLogLevel() >= LOGMESSAGE)
    handle.logMessage(LOGMESSAGE, "The likelihood score for this component on this timestep is", l);

}

double CatchStatistics::calcLikSumSquares() {
  int area, age, lengr;
  int numlen = LgrpDiv->numLengthGroups();
  double lik, total, simvar, simdiff;

  lik = total = simvar = simdiff = 0.0;
  alptr = &aggregator->getSum();

  if ((functionnumber==6) || (functionnumber==7)){
    for (area = 0; area < areas.Nrow(); area++) {
      likelihoodValues[timeindex][area] = 0.0;

      for (lengr = 0; lengr < numlen; lengr++) {
	switch (functionnumber) {
	case 6:
          ps.calcStatistics((*alptr)[area], lengr);
	  (*modelMean[timeindex])[area][lengr] = ps.meanWeight();
	  simvar = (*obsStdDev[timeindex])[area][lengr] * (*obsStdDev[timeindex])[area][lengr];
          break;
        case 7:
          ps.calcStatistics((*alptr)[area], lengr);
          (*modelMean[timeindex])[area][lengr] = ps.meanWeight();
          simvar = 1.0;
          break;
        default:
          handle.logMessage(LOGWARN, "Warning in catchstatistics 3 - unrecognised function", functionname);
          break;
	}
	
	if (!(isZero(simvar))) {
	  simdiff = (*modelMean[timeindex])[area][lengr] - (*obsMean[timeindex])[area][lengr];
	  lik = simdiff * simdiff * (*numbers[timeindex])[area][lengr] / simvar;
	  likelihoodValues[timeindex][area] += lik;
	  
	}
  
      }
      total += likelihoodValues[timeindex][area];
    }

  } else{
    for (area = 0; area < areas.Nrow(); area++) {
      likelihoodValues[timeindex][area] = 0.0;
      
      for (age = (*alptr)[area].minAge(); age <= (*alptr)[area].maxAge(); age++) {
	switch (functionnumber) {
	case 1:
	  ps.calcStatistics((*alptr)[area][age], LgrpDiv, 0);
	  (*modelMean[timeindex])[area][age] = ps.meanLength();
	  simvar = ps.sdevLength() * ps.sdevLength();
	  (*modelStdDev[timeindex])[area][age] = ps.sdevLength();
	  break;
	case 2:
	  ps.calcStatistics((*alptr)[area][age], LgrpDiv, 0);
	  (*modelMean[timeindex])[area][age] = ps.meanLength();
	  simvar = (*obsStdDev[timeindex])[area][age] * (*obsStdDev[timeindex])[area][age];
	  break;
	case 3:
	  ps.calcStatistics((*alptr)[area][age], LgrpDiv);
	  (*modelMean[timeindex])[area][age] = ps.meanWeight();
	  simvar = (*obsStdDev[timeindex])[area][age] * (*obsStdDev[timeindex])[area][age];
	  break;
	case 4:
	  ps.calcStatistics((*alptr)[area][age], LgrpDiv);
	  (*modelMean[timeindex])[area][age] = ps.meanWeight();
	  simvar = 1.0;
	  break;
	case 5:
	  ps.calcStatistics((*alptr)[area][age], LgrpDiv, 0);
	  (*modelMean[timeindex])[area][age] = ps.meanLength();
	  simvar = 1.0;
	  break;
	default:
	  handle.logMessage(LOGWARN, "Warning in catchstatistics 2 - unrecognised function", functionname);
	  break;
	}
	  
	if (!(isZero(simvar))) {
	  simdiff = (*modelMean[timeindex])[area][age] - (*obsMean[timeindex])[area][age];
	  lik = simdiff * simdiff * (*numbers[timeindex])[area][age] / simvar;
	  likelihoodValues[timeindex][area] += lik;
	}
      }
      total += likelihoodValues[timeindex][area];
    }
  }
  return total;

}

void CatchStatistics::printLikelihood(ofstream& outfile, const TimeClass* const TimeInfo) {

  if (!AAT.atCurrentTime(TimeInfo))
    return;

  int i, area, age, length;
  timeindex = -1;
  for (i = 0; i < Years.Size(); i++)
    if ((Years[i] == TimeInfo->getYear()) && (Steps[i] == TimeInfo->getStep()))
      timeindex = i;
  if (timeindex == -1)
    handle.logMessage(LOGFAIL, "Error in catchstatistics - invalid timestep");

  if ((functionnumber==6) || (functionnumber==7)){
    for (area = 0; area < (*modelMean[timeindex]).Nrow(); area++) {
      for (length = 0; length < (*modelMean[timeindex]).Ncol(area); length++) {
	outfile << setw(lowwidth) << Years[timeindex] << sep << setw(lowwidth)
		<< Steps[timeindex] << sep << setw(printwidth) << areaindex[area] << sep
		<< setw(printwidth) << lenindex[length] << sep << setprecision(printprecision)
		<< setw(printwidth) << (*numbers[timeindex])[area][length]
		<< sep << setprecision(largeprecision) << setw(largewidth);
	
	
	//JMB crude filter to remove the 'silly' values from the output
	if ((*modelMean[timeindex])[area][length] < rathersmall)
	  outfile << 0;
	else
	  outfile << (*modelMean[timeindex])[area][length];
	
	switch (functionnumber) {
        case 6:
          outfile << sep << setprecision(printprecision) << setw(printwidth)
            << (*obsStdDev[timeindex])[area][length] << endl;
          break;
        case 7:
          outfile << endl;
          break;
        default:
          handle.logMessage(LOGWARN, "Warning in catchstatistics 5 - unrecognised function", functionname);
          break;
	}
      }
    }
  }else{
    for (area = 0; area < (*modelMean[timeindex]).Nrow(); area++) {
      for (age = 0; age < (*modelMean[timeindex]).Ncol(area); age++) {
	outfile << setw(lowwidth) << Years[timeindex] << sep << setw(lowwidth)
		<< Steps[timeindex] << sep << setw(printwidth) << areaindex[area] << sep
		<< setw(printwidth) << ageindex[age] << sep << setprecision(printprecision)
		<< setw(printwidth) << (*numbers[timeindex])[area][age]
		<< sep << setprecision(largeprecision) << setw(largewidth);
	
	//JMB crude filter to remove the 'silly' values from the output
	if ((*modelMean[timeindex])[area][age] < rathersmall)
	  outfile << 0;
	else
	  outfile << (*modelMean[timeindex])[area][age];
	
	switch (functionnumber) {
        case 1:
          outfile << sep << setprecision(printprecision) << setw(printwidth)
		  << (*modelStdDev[timeindex])[area][age] << endl;
          break;
        case 2:
        case 3:
          outfile << sep << setprecision(printprecision) << setw(printwidth)
		  << (*obsStdDev[timeindex])[area][age] << endl;
          break;
        case 4:
        case 5:
          outfile << endl;
          break;
        default:
          handle.logMessage(LOGWARN, "Warning in catchstatistics 4 - unrecognised function", functionname);
          break;
	}
      }
    }
  }
  
}

void CatchStatistics::printSummary(ofstream& outfile) {
  int year, area;

  for (year = 0; year < likelihoodValues.Nrow(); year++)
    for (area = 0; area < likelihoodValues.Ncol(year); area++)
      outfile << setw(lowwidth) << Years[year] << sep << setw(lowwidth)
        << Steps[year] << sep << setw(printwidth) << areaindex[area] << sep
        << setw(largewidth) << this->getName() << sep << setprecision(smallprecision)
        << setw(smallwidth) << weight << sep << setprecision(largeprecision)
        << setw(largewidth) << likelihoodValues[year][area] << endl;

  outfile.flush();
}
