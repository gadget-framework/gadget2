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

CatchStatistics::CatchStatistics(CommentStream& infile, const AreaClass* const Area,
  const TimeClass* const TimeInfo, double likweight, const char* name)
  : Likelihood(CATCHSTATISTICSLIKELIHOOD, likweight) {

  lgrpDiv = NULL;
  ErrorHandler handle;
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

  if (strcasecmp(functionname, "lengthcalcvar") == 0)
    functionnumber = 1;
  else if (strcasecmp(functionname, "lengthgivenvar") == 0)
    functionnumber = 2;
  else if (strcasecmp(functionname, "weightgivenvar") == 0)
    functionnumber = 3;
  else if (strcasecmp(functionname, "weightnovar") == 0)
    functionnumber = 4;
  else if (strcasecmp(functionname, "lengthnovar") == 0)
    functionnumber = 5;
  else if (strcasecmp(functionname, "experimental") == 0)
    functionnumber = 6;
  else
    handle.Message("Error in catchstatistics - unrecognised function", functionname);

  //Read in area aggregation from file
  readWordAndValue(infile, "areaaggfile", aggfilename);
  datafile.open(aggfilename);
  checkIfFailure(datafile, aggfilename);
  handle.Open(aggfilename);
  numarea = readAggregation(subdata, areas, areaindex);
  handle.Close();
  datafile.close();
  datafile.clear();

  //Read in age aggregation from file
  readWordAndValue(infile, "ageaggfile", aggfilename);
  datafile.open(aggfilename);
  checkIfFailure(datafile, aggfilename);
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

  //Read in the fleetnames
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

  //Read in the stocknames
  i = 0;
  if (!(strcasecmp(text, "stocknames") == 0))
    handle.Unexpected("stocknames", text);
  infile >> text;
  while (!infile.eof() && !(strcasecmp(text, "[component]") == 0)) {
    stocknames.resize(1);
    stocknames[i] = new char[strlen(text) + 1];
    strcpy(stocknames[i++], text);
    infile >> text >> ws;
  }

  //We have now read in all the data from the main likelihood file
  //But we have to read in the statistics data from datafilename
  datafile.open(datafilename);
  checkIfFailure(datafile, datafilename);
  handle.Open(datafilename);
  ReadStatisticsData(subdata, TimeInfo, numarea, numage);
  handle.Close();
  datafile.close();
  datafile.clear();
}

void CatchStatistics::ReadStatisticsData(CommentStream& infile,
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
  ErrorHandler handle;

  //Find start of statistics data in datafile
  infile >> ws;
  char c;
  c = infile.peek();
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

    //check if the year and step are in the simulation
    timeid = -1;
    if (TimeInfo->IsWithinPeriod(year, step)) {
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

    if (keepdata == 0) {
      //statistics data is required, so store it
      count++;
      (*numbers[timeid])[areaid][ageid] = tmpnumber;
      (*mean[timeid])[areaid][ageid] = tmpmean;
      if (needvar == 1)
        (*variance[timeid])[areaid][ageid] = tmpstddev * tmpstddev;
    }
  }
  AAT.AddActions(Years, Steps, TimeInfo);
  if (count == 0)
    cerr << "Warning in catchstatistics - found no data in the data file for " << csname << endl;
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
  if (lgrpDiv != NULL) {
    delete lgrpDiv;
    lgrpDiv = NULL;
  }
}

void CatchStatistics::Reset(const Keeper* const keeper) {
  Likelihood::Reset(keeper);
  timeindex = 0;
}

void CatchStatistics::Print(ofstream& outfile) const {
  int i;

  outfile << "\nCatch statistics " << csname << "\nlikelihood " << likelihood
    << "\nfunction " << functionname;
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

void CatchStatistics::SetFleetsAndStocks(FleetPtrVector& Fleets, StockPtrVector& Stocks) {
  int i, j, found;
  FleetPtrVector fleets;
  StockPtrVector stocks;

  for (i = 0; i < fleetnames.Size(); i++) {
    found = 0;
    for (j = 0; j < Fleets.Size(); j++)
      if (strcasecmp(fleetnames[i], Fleets[j]->Name()) == 0) {
        found = 1;
        fleets.resize(1, Fleets[j]);
      }

    if (found == 0) {
      cerr << "Error when searching for names of fleets for catchstatistics\n"
        << "Did not find any name matching " << fleetnames[i] << endl;
      exit(EXIT_FAILURE);
    }
  }

  for (i = 0; i < stocknames.Size(); i++) {
    found = 0;
    for (j = 0; j < Stocks.Size(); j++)
      if (Stocks[j]->IsEaten())
        if (strcasecmp(stocknames[i], Stocks[j]->ReturnPrey()->Name()) == 0) {
          found = 1;
          stocks.resize(1, Stocks[j]);
        }

    if (found == 0) {
      cerr << "Error when searching for names of stocks for catchstatistics\n"
        << "Did not find any name matching " << stocknames[i] << endl;
      exit(EXIT_FAILURE);
    }
  }

  lgrpDiv = new LengthGroupDivision(*(stocks[0]->ReturnPrey()->ReturnLengthGroupDiv()));
  for (i = 1; i < stocks.Size(); i++)
    if (!lgrpDiv->Combine(stocks[i]->ReturnPrey()->ReturnLengthGroupDiv())) {
      cerr << "Length group divisions for preys in catchstatistics not compatible\n";
      exit(EXIT_FAILURE);
    }

  aggregator = new FleetPreyAggregator(fleets, stocks, lgrpDiv, areas, ages, overconsumption);
}

void CatchStatistics::AddToLikelihood(const TimeClass* const TimeInfo) {

  if (AAT.AtCurrentTime(TimeInfo)) {
    aggregator->Sum(TimeInfo);
    //JMB - check the following if any new functionnumber values are added
    likelihood += SOSWeightOrLength();
    timeindex++;
  }
}

double CatchStatistics::SOSWeightOrLength() {
  double lik = 0.0;
  int nareas, age;
  double simmean, simvar, simnumber, simdiff;

  simmean = simvar = simnumber = simdiff = 0.0;
  const AgeBandMatrixPtrVector *alptr = &aggregator->AgeLengthDist();
  for (nareas = 0; nareas < alptr->Size(); nareas++) {
    for (age = 0; age < (*alptr)[nareas].Nrow(); age++) {
      PopStatistics PopStat((*alptr)[nareas][age], aggregator->ReturnLgrpDiv());

      switch(functionnumber) {
        case 1:
          simmean = PopStat.MeanLength();
          simdiff = simmean - (*mean[timeindex])[nareas][age];
          simvar = PopStat.StdDevOfLength() * PopStat.StdDevOfLength();
          simnumber = PopStat.TotalNumber();
          break;
        case 2:
        case 6:
          simmean = PopStat.MeanLength();
          simdiff = simmean - (*mean[timeindex])[nareas][age];
          simvar = (*variance[timeindex])[nareas][age];
          simnumber = PopStat.TotalNumber();
          break;
        case 3:
          simmean = PopStat.MeanWeight();
          simdiff = simmean - (*mean[timeindex])[nareas][age];
          simvar = (*variance[timeindex])[nareas][age];
          simnumber = PopStat.TotalNumber();
          break;
        case 4:
          simmean = PopStat.MeanWeight();
          simdiff = simmean - (*mean[timeindex])[nareas][age];
          simvar = 1.0;
          simnumber = PopStat.TotalNumber();
          break;
        case 5:
          simmean = PopStat.MeanLength();
          simdiff = simmean - (*mean[timeindex])[nareas][age];
          simvar = 1.0;
          simnumber = PopStat.TotalNumber();
          break;
        default:
          cerr << "Error in catchstatistics - unknown function " << functionname << endl;
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
          lik += simdiff * simdiff * (simvar > 0 ? 1 / simvar : 0) * (*numbers[timeindex])[nareas][age]
            * (1 - exp(-20 * simnumber)) + ((*mean[timeindex])[nareas][age]
            * (*mean[timeindex])[nareas][age] * exp(-20 * simnumber));
          break;
        case 6:
          if (isZero(simmean))
            lik += (*numbers[timeindex])[nareas][age];
          else if (simmean > verysmall && (*mean[timeindex])[nareas][age] > verysmall)
            lik += simdiff * simdiff * (simvar > 0 ? 1 / simvar : 0) * (*numbers[timeindex])[nareas][age];

          break;
        default:
          cerr << "Error in catchstatistics - unknown function " << functionname << endl;
          break;
      }
    }
  }
  return lik;
}
