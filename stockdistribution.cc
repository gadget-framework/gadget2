#include "stockdistribution.h"
#include "readfunc.h"
#include "readword.h"
#include "readaggregation.h"
#include "errorhandler.h"
#include "areatime.h"
#include "fleet.h"
#include "stock.h"
#include "multinomial.h"
#include "stockprey.h"
#include "gadget.h"

StockDistribution::StockDistribution(CommentStream& infile,
  const AreaClass* const Area, const TimeClass* const TimeInfo, double likweight)
  : Likelihood(STOCKDISTRIBUTIONLIKELIHOOD, likweight) {


  ErrorHandler handle;
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
  ReadWordAndValue(infile, "datafile", datafilename);
  ReadWordAndVariable(infile, "functionnumber", functionnumber);
  ReadWordAndVariable(infile, "overconsumption", overconsumption);
  ReadWordAndVariable(infile, "minimumprobability", minp);
  if (minp <= 0) {
    handle.Warning("Minimumprobability should be a positive integer - set to default value 20");
    minp = 20;
  }

  //Read in area aggregation from file
  ReadWordAndValue(infile, "areaaggfile", aggfilename);
  datafile.open(aggfilename);
  CheckIfFailure(datafile, aggfilename);
  handle.Open(aggfilename);
  numarea = ReadAggregation(subdata, areas, areaindex);
  handle.Close();
  datafile.close();
  datafile.clear();

  //Read in age aggregation from file
  ReadWordAndValue(infile, "ageaggfile", aggfilename);
  datafile.open(aggfilename);
  CheckIfFailure(datafile, aggfilename);
  handle.Open(aggfilename);
  numage = ReadAggregation(subdata, ages, ageindex);
  handle.Close();
  datafile.close();
  datafile.clear();

  //Read in length aggregation from file
  ReadWordAndValue(infile, "lenaggfile", aggfilename);
  datafile.open(aggfilename);
  CheckIfFailure(datafile, aggfilename);
  handle.Open(aggfilename);
  numlen = ReadLengthAggregation(subdata, lengths, lenindex);
  handle.Close();
  datafile.close();
  datafile.clear();

  //Must change from outer areas to inner areas.
  for (i = 0; i < areas.Nrow(); i++)
    for (j = 0; j < areas.Ncol(i); j++)
      if ((areas[i][j] = Area->InnerArea(areas[i][j])) == -1)
        handle.UndefinedArea(areas[i][j]);

  //Must create the length group division
  lgrpdiv = new LengthGroupDivision(lengths);

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
  infile >> text >> ws;
  while (!infile.eof() && !(strcasecmp(text, "[component]") == 0)) {
    stocknames.resize(1);
    stocknames[i] = new char[strlen(text) + 1];
    strcpy(stocknames[i++], text);
    infile >> text >> ws;
  }

  //We have now read in all the data from the main likelihood file
  //But we have to read in the statistics data from datafilename
  datafile.open(datafilename);
  CheckIfFailure(datafile, datafilename);
  handle.Open(datafilename);
  ReadStockData(subdata, TimeInfo, numarea, numage, numlen);
  handle.Close();
  datafile.close();
  datafile.clear();
}

void StockDistribution::ReadStockData(CommentStream& infile,
  const TimeClass* TimeInfo, int numarea, int numage, int numlen) {

  ErrorHandler handle;
  int i, j;
  int year, step;
  double tmpnumber;
  char tmparea[MaxStrLength], tmpstock[MaxStrLength];
  char tmpage[MaxStrLength], tmplen[MaxStrLength];
  strncpy(tmparea, "", MaxStrLength);
  strncpy(tmpstock, "", MaxStrLength);
  strncpy(tmpage, "", MaxStrLength);
  strncpy(tmplen, "", MaxStrLength);
  int keepdata, timeid, stockid, ageid, areaid, lenid;
  int numstock = stocknames.Size();
  intvector Years, Steps;

  //Find start of distribution data in datafile
  infile >> ws;
  char c;
  c = infile.peek();
  if (!isdigit(c)) {
    infile.get(c);
    while (c != '\n' && !infile.eof())
      infile.get(c);
  }

  //Check the number of columns in the inputfile
  if (!(CheckColumns(infile, 7)))
    handle.Message("Wrong number of columns in inputfile - should be 7");

  while (!infile.eof()) {
    keepdata = 0;
    infile >> year >> step >> tmparea >> tmpstock >> tmpage >> tmplen >> tmpnumber >> ws;

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
        timeid = (Years.Size() - 1);

        AgeLengthData.AddRows(1, numarea);
        for (i = 0; i < numarea; i++)
          AgeLengthData[timeid][i] = new doublematrix(numstock, (numage * numlen), 0.0);
      }

    } else {
      //dont keep the data
      keepdata = 1;
    }

    //if tmpstock is in stocknames find stockid, else dont keep the data
    stockid = -1;
    for (i = 0; i < stocknames.Size(); i++)
      if (strcasecmp(stocknames[i], tmpstock) == 0)
        stockid = i;

    if (stockid == -1)
      keepdata = 1;

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

    if (keepdata == 0) {
      //stock distribution data is required, so store it
      i = ageid + (numage * lenid);
      //JMB - this should be stored as [time][area][stock][age][length]
      (*AgeLengthData[timeid][areaid])[stockid][i] = tmpnumber;
    }
  }
  AAT.AddActions(Years, Steps, TimeInfo);
}

StockDistribution::~StockDistribution() {
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
  for (i = 0; i < AgeLengthData.Nrow(); i++)
    for (j = 0; j < AgeLengthData.Ncol(i); j++)
      delete AgeLengthData[i][j];
  delete aggregator;
}

void StockDistribution::Reset(const Keeper* const keeper) {
  Likelihood::Reset(keeper);
  timeindex = 0;
}

void StockDistribution::Print(ofstream& outfile) const {
  int i, j, y, a;

  outfile << "\nStock Distribution\nlikelihood " << likelihood
    << "\nfunction number " << functionnumber << "\n\tStocknames:";
  for (i = 0; i < stocknames.Size(); i++)
    outfile << sep << stocknames[i];
  outfile << "\n\tFleetnames:";
  for (i = 0; i < fleetnames.Size(); i++)
    outfile << sep << fleetnames[i];
  outfile << endl;
  for (i = 0; i < stocknames.Size(); i++)
    aggregator[i]->Print(outfile);

  outfile << "\tAge-Length Distribution Data:\n";
  for (y = 0; y < AgeLengthData.Nrow(); y++) {
    outfile << "\tyear and step " << y << endl;
    for (a = 0; a < AgeLengthData.Ncol(y); a++) {
      outfile << "\tareas " << a << endl;
      for (i = 0; i < AgeLengthData[y][a]->Nrow(); i++) {
        outfile << TAB;
        for (j = 0; j < AgeLengthData[y][a]->Ncol(i); j++) {
          outfile.width(printwidth);
          outfile.precision(smallprecision);
          outfile << (*AgeLengthData[y][a])[i][j] << sep;
        }
        outfile << endl;
      }
    }
  }
}

void StockDistribution::SetFleetsAndStocks(Fleetptrvector& Fleets, Stockptrvector& Stocks) {
  int i, j, found, stocknumber;
  Fleetptrvector fleets;

  for (i = 0; i < fleetnames.Size(); i++) {
    found = 0;
    for (j = 0; j < Fleets.Size(); j++)
      if (strcasecmp(fleetnames[i], Fleets[j]->Name()) == 0) {
        found = 1;
        fleets.resize(1, Fleets[j]);
      }
    if (found == 0) {
      cerr << "Error: when searching for names of fleets for Stockdistribution.\n"
        << "Did not find any name matching " << fleetnames[i] << endl;
      exit(EXIT_FAILURE);
    }
  }

  aggregator = new FleetPreyAggregator*[stocknames.Size()];
  for (i = 0; i < stocknames.Size(); i++) {
    Stockptrvector stocks;
    found = 0;
    for (j = 0; j < Stocks.Size(); j++) {
      if (Stocks[j]->IsEaten())
        if (strcasecmp(stocknames[i], Stocks[j]->ReturnPrey()->Name()) == 0) {
          found = 1;
          stocks.resize(1, Stocks[j]);
        }
    }
    if (found == 0) {
      cerr << "Error: when searching for names of stocks for Stockdistribution.\n"
        << "Did not find any name matching " << stocknames[i] << endl;
      exit(EXIT_FAILURE);
    }
    aggregator[i] = new FleetPreyAggregator(fleets, stocks, lgrpdiv, areas, ages, overconsumption);
  }
}

void StockDistribution::AddToLikelihood(const TimeClass* const TimeInfo) {
  int i;
  if (AAT.AtCurrentTime(TimeInfo)) {
    for (i = 0; i < stocknames.Size(); i++)
      aggregator[i]->Sum(TimeInfo);

    switch(functionnumber) {
      case 1:
        likelihood += LikMultinomial();
        break;
      default:
        break;
    }
    timeindex++;
  }
}

//The code here is probably unnessecarily complicated because
//is always only one length group with this class.
double StockDistribution::LikMultinomial() {
  const Agebandmatrixvector* alptr;
  doublematrixptrvector Dist(areas.Nrow(), NULL);
  int nareas, area, age, length, sn;
  int minage, maxage;

  for (area = 0; area < Dist.Size(); area++) {
    Dist[area] = new doublematrix(aggregator[0]->NoAgeGroups() *
      aggregator[0]->NoLengthGroups(), stocknames.Size(), 0);
    for (sn = 0; sn < stocknames.Size(); sn++) {
      alptr = &aggregator[sn]->AgeLengthDist();
      minage = (*alptr)[area].Minage();
      maxage = (*alptr)[area].Maxage();
      for (age = minage; age <= maxage; age++) {
        for (length = (*alptr)[area].Minlength(age);
           length < (*alptr)[area].Maxlength(age); length++)
         (*Dist[area])[age + length * (maxage + 1)][sn] = ((*alptr)[area][age][length]).N;
      }
    }
  }

  //The object MN does most of the work, accumulating likelihood
  Multinomial MN(minp);
  doublevector likdata(stocknames.Size());
  for (nareas = 0; nareas < Dist.Size(); nareas++) {
    for (area = 0; area < Dist[nareas]->Nrow(); area++) {
      for (sn = 0; sn < stocknames.Size(); sn++)
        likdata[sn] = (*AgeLengthData[timeindex][nareas])[sn][area];

      MN.LogLikelihood(likdata, (*Dist[nareas])[area]);
    }
    delete Dist[nareas];
  }
  return MN.ReturnLogLikelihood();
}
