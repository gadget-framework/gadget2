#include "catchstatistics.h"
#include "commentstream.h"
#include "readfunc.h"
#include "readword.h"
#include "errorhandler.h"
#include "areatime.h"
#include "fleet.h"
#include "stock.h"
#include "stockprey.h"
#include "popstatistics.h"
#include "readaggregation.h"
#include "gadget.h"

extern ErrorHandler handle;

CatchStatistics::CatchStatistics(CommentStream& infile, const AreaClass* const Area,
  const TimeClass* const TimeInfo, double weight, const char* name)
  : Likelihood(CATCHSTATISTICSLIKELIHOOD, weight) {

  LgrpDiv = NULL;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  int i, j;
  int numarea = 0, numage = 0;

  char datafilename[MaxStrLength];
  char aggfilename[MaxStrLength];
  strncpy(datafilename, "", MaxStrLength);
  strncpy(aggfilename, "", MaxStrLength);
  ifstream datafile;
  CommentStream subdata(datafile);

  timeindex = 0;
  csname = new char[strlen(name) + 1];
  strcpy(csname, name);

  functionname = new char[MaxStrLength];
  strncpy(functionname, "", MaxStrLength);
  readWordAndValue(infile, "datafile", datafilename);
  readWordAndValue(infile, "function", functionname);
  readWordAndVariable(infile, "overconsumption", overconsumption);
  if (overconsumption != 0 && overconsumption != 1)
    handle.Message("Error in catchstatistics - overconsumption must be 0 or 1");

  if ((strcasecmp(functionname, "lengthcalcvar") == 0) || (strcasecmp(functionname, "lengthcalcstddev") == 0))
    functionnumber = 1;
  else if ((strcasecmp(functionname, "lengthgivenvar") == 0) || (strcasecmp(functionname, "lengthgivenstddev") == 0))
    functionnumber = 2;
  else if ((strcasecmp(functionname, "weightgivenvar") == 0) || (strcasecmp(functionname, "weightgivenstddev") == 0))
    functionnumber = 3;
  else if ((strcasecmp(functionname, "weightnovar") == 0) || (strcasecmp(functionname, "weightnostddev") == 0))
    functionnumber = 4;
  else if ((strcasecmp(functionname, "lengthnovar") == 0) || (strcasecmp(functionname, "lengthnostddev") == 0))
    functionnumber = 5;
  else if (strcasecmp(functionname, "experimental") == 0)
    functionnumber = 6;
  else
    handle.Message("Error in catchstatistics - unrecognised function", functionname);

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

  //Must change from outer areas to inner areas.
  for (i = 0; i < areas.Nrow(); i++)
    for (j = 0; j < areas.Ncol(i); j++)
      if ((areas[i][j] = Area->InnerArea(areas[i][j])) == -1)
        handle.UndefinedArea(areas[i][j]);

  //read in the fleetnames
  i = 0;
  infile >> text >> ws;
  if (!(strcasecmp(text, "fleetnames") == 0))
    handle.Unexpected("fleetnames", text);
  infile >> text >> ws;
  while (!infile.eof() && !(strcasecmp(text, "stocknames") == 0)) {
    fleetnames.resize(1);
    fleetnames[i] = new char[strlen(text) + 1];
    strcpy(fleetnames[i++], text);
    infile >> text >> ws;
  }

  //read in the stocknames
  i = 0;
  if (!(strcasecmp(text, "stocknames") == 0))
    handle.Unexpected("stocknames", text);
  infile >> text;
  while (!infile.eof() && !(strcasecmp(text, "[component]") == 0)) {
    infile >> ws;
    stocknames.resize(1);
    stocknames[i] = new char[strlen(text) + 1];
    strcpy(stocknames[i++], text);
    infile >> text;
  }

  //We have now read in all the data from the main likelihood file
  //But we have to read in the statistics data from datafilename
  datafile.open(datafilename, ios::in);
  handle.checkIfFailure(datafile, datafilename);
  handle.Open(datafilename);
  readStatisticsData(subdata, TimeInfo, numarea, numage);
  handle.Close();
  datafile.close();
  datafile.clear();
}

void CatchStatistics::readStatisticsData(CommentStream& infile,
  const TimeClass* TimeInfo, int numarea, int numage) {

  int i;
  IntVector Years, Steps;
  int year, step;
  double tmpnumber, tmpmean, tmpstddev;
  char tmparea[MaxStrLength], tmpage[MaxStrLength];
  strncpy(tmparea, "", MaxStrLength);
  strncpy(tmpage, "", MaxStrLength);
  int keepdata, needvar;
  int timeid, ageid, areaid;
  int count = 0;

  //Find start of statistics data in datafile
  infile >> ws;
  char c = infile.peek();
  if (!isdigit(c)) {
    infile.get(c);
    while (c != '\n' && !infile.eof())
      infile.get(c);
  }

  needvar = 0;
  switch(functionnumber) {
    case 2:
    case 3:
    case 6:
      needvar = 1;
  }

  //Check the number of columns in the inputfile
  if ((needvar == 1) && (countColumns(infile) != 7))
    handle.Message("Wrong number of columns in inputfile - should be 7");
  else if ((needvar == 0) && (countColumns(infile) != 6))
    handle.Message("Wrong number of columns in inputfile - should be 6");

  while (!infile.eof()) {
    keepdata = 0;
    if (needvar == 1)
      infile >> year >> step >> tmparea >> tmpage >> tmpnumber >> tmpmean >> tmpstddev >> ws;
    else
      infile >> year >> step >> tmparea >> tmpage >> tmpnumber >> tmpmean >> ws;

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

    //check if the year and step are in the simulation
    timeid = -1;
    if ((TimeInfo->IsWithinPeriod(year, step)) && (keepdata == 0)) {
      //if this is a new timestep, resize to store the data
      for (i = 0; i < Years.Size(); i++)
        if ((Years[i] == year) && (Steps[i] == step))
          timeid = i;

      if (timeid == -1) {
        Years.resize(1, year);
        Steps.resize(1, step);
        numbers.resize(1, new DoubleMatrix(numarea, numage, 0.0));
        mean.resize(1, new DoubleMatrix(numarea, numage, 0.0));
        if (needvar == 1)
          variance.resize(1, new DoubleMatrix(numarea, numage, 0.0));
        timeid = (Years.Size() - 1);
      }

    } else {
      //dont keep the data
      keepdata = 1;
    }

    if (keepdata == 0) {
      //statistics data is required, so store it
      count++;
      (*numbers[timeid])[areaid][ageid] = tmpnumber;
      (*mean[timeid])[areaid][ageid] = tmpmean;
      if (needvar == 1)
        (*variance[timeid])[areaid][ageid] = tmpstddev * tmpstddev;
    }
  }
  AAT.addActions(Years, Steps, TimeInfo);
  if (count == 0)
    handle.logWarning("Warning in catchstatistics - found no data in the data file for", csname);
  handle.logMessage("Read catchstatistics data file - number of entries", count);
}

CatchStatistics::~CatchStatistics() {
  int i;
  for (i = 0; i < stocknames.Size(); i++)
    delete[] stocknames[i];
  for (i = 0; i < fleetnames.Size(); i++)
    delete[] fleetnames[i];
  for (i = 0; i < areaindex.Size(); i++)
    delete[] areaindex[i];
  for (i = 0; i < ageindex.Size(); i++)
    delete[] ageindex[i];
  delete aggregator;
  for (i = 0; i < numbers.Size(); i++) {
    delete numbers[i];
    delete mean[i];
  }
  for (i = 0; i < variance.Size(); i++)
    delete variance[i];
  delete[] csname;
  delete[] functionname;
  if (LgrpDiv != NULL) {
    delete LgrpDiv;
    LgrpDiv = NULL;
  }
}

void CatchStatistics::Reset(const Keeper* const keeper) {
  Likelihood::Reset(keeper);
  timeindex = 0;
  handle.logMessage("Reset catchstatistics component", csname);
}

void CatchStatistics::Print(ofstream& outfile) const {
  int i;

  outfile << "\nCatch Statistics " << csname << " - likelihood value " << likelihood
    << "\n\tFunction " << functionname;
  outfile << "\n\tStock names:";
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
  int i, j, found;
  FleetPtrVector fleets;
  StockPtrVector stocks;

  for (i = 0; i < fleetnames.Size(); i++) {
    found = 0;
    for (j = 0; j < Fleets.Size(); j++)
      if (strcasecmp(fleetnames[i], Fleets[j]->Name()) == 0) {
        found++;
        fleets.resize(1, Fleets[j]);
      }

    if (found == 0)
      handle.logFailure("Error in catchstatistics - unknown fleet", fleetnames[i]);

  }

  for (i = 0; i < stocknames.Size(); i++) {
    found = 0;
    for (j = 0; j < Stocks.Size(); j++)
      if (Stocks[j]->IsEaten())
        if (strcasecmp(stocknames[i], Stocks[j]->returnPrey()->Name()) == 0) {
          found++;
          stocks.resize(1, Stocks[j]);
        }

    if (found == 0)
      handle.logFailure("Error in catchstatistics - unknown stock", stocknames[i]);

  }

  LgrpDiv = new LengthGroupDivision(*(stocks[0]->returnPrey()->returnLengthGroupDiv()));
  for (i = 1; i < stocks.Size(); i++)
    if (!LgrpDiv->Combine(stocks[i]->returnPrey()->returnLengthGroupDiv()))
      handle.logFailure("Error in catchstatistics - length groups for preys not compatible");

  aggregator = new FleetPreyAggregator(fleets, stocks, LgrpDiv, areas, ages, overconsumption);
}

void CatchStatistics::addLikelihood(const TimeClass* const TimeInfo) {

  if (AAT.AtCurrentTime(TimeInfo)) {
    handle.logMessage("Calculating likelihood score for catchstatistics component", csname);
    aggregator->Sum(TimeInfo);
    likelihood += calcLikSumSquares();
    timeindex++;
    handle.logMessage("The likelihood score for this component has increased to", likelihood);
  }
}

double CatchStatistics::calcLikSumSquares() {
  double lik = 0.0;
  int nareas, age;
  double simmean, simvar, simnumber, simdiff;

  simmean = simvar = simnumber = simdiff = 0.0;
  const AgeBandMatrixPtrVector *alptr = &aggregator->returnSum();
  for (nareas = 0; nareas < alptr->Size(); nareas++) {
    for (age = 0; age < (*alptr)[nareas].Nrow(); age++) {
      PopStatistics PopStat((*alptr)[nareas][age], aggregator->returnLengthGroupDiv());

      switch(functionnumber) {
        case 1:
          simmean = PopStat.meanLength();
          simdiff = simmean - (*mean[timeindex])[nareas][age];
          simvar = PopStat.sdevLength() * PopStat.sdevLength();
          simnumber = PopStat.totalNumber();
          break;
        case 2:
        case 6:
          simmean = PopStat.meanLength();
          simdiff = simmean - (*mean[timeindex])[nareas][age];
          simvar = (*variance[timeindex])[nareas][age];
          simnumber = PopStat.totalNumber();
          break;
        case 3:
          simmean = PopStat.meanWeight();
          simdiff = simmean - (*mean[timeindex])[nareas][age];
          simvar = (*variance[timeindex])[nareas][age];
          simnumber = PopStat.totalNumber();
          break;
        case 4:
          simmean = PopStat.meanWeight();
          simdiff = simmean - (*mean[timeindex])[nareas][age];
          simvar = 1.0;
          simnumber = PopStat.totalNumber();
          break;
        case 5:
          simmean = PopStat.meanLength();
          simdiff = simmean - (*mean[timeindex])[nareas][age];
          simvar = 1.0;
          simnumber = PopStat.totalNumber();
          break;
        default:
          handle.logWarning("Warning in catchstatistics - unknown function", functionname);
          break;
      }
      switch(functionnumber) {
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
          /* THLTH 10.03.01: Changed this to make it diff. and to add a penalty, so that
           * when the model is fishing out all the fish (aka the number in the model goes
           * down to zero and therefore the mean length or weight as well), we get the
           * value l^2 for that timestep, where l is the meanlength/-weight in the sample
           * (that is, we get some fairly high number, instead of zero before).
           * Further, the reason why the population is multiplied with a 20 is simply to
           * avoid the penalty to come in to quickly. */
          lik += simdiff * simdiff * (simvar > 0 ? 1 / simvar : 0.0) * (*numbers[timeindex])[nareas][age]
            * (1 - exp(-20 * simnumber)) + ((*mean[timeindex])[nareas][age]
            * (*mean[timeindex])[nareas][age] * exp(-20 * simnumber));
          break;
        case 6:
          if (isZero(simmean))
            lik += (*numbers[timeindex])[nareas][age];
          else if (simmean > verysmall && (*mean[timeindex])[nareas][age] > verysmall)
            lik += simdiff * simdiff * (simvar > 0 ? 1 / simvar : 0.0) * (*numbers[timeindex])[nareas][age];

          break;
        default:
          handle.logWarning("Warning in catchstatistics - unknown function", functionname);
          break;
      }
    }
  }
  return lik;
}
