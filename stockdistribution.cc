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
  const AreaClass* const Area, const TimeClass* const TimeInfo,
  double w, const char* name)
  : Likelihood(STOCKDISTRIBUTIONLIKELIHOOD, w) {


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
  sdname = new char[strlen(name) + 1];
  strcpy(sdname, name);
  functionname = new char[MaxStrLength];
  strncpy(functionname, "", MaxStrLength);
  readWordAndValue(infile, "datafile", datafilename);
  readWordAndValue(infile, "function", functionname);
  if (strcasecmp(functionname, "multinomial") == 0)
    functionnumber = 1;
  else if (strcasecmp(functionname, "sumofsquares") == 0)
    functionnumber = 2;
  else
    handle.Message("Error in stockdistribution - unrecognised function", functionname);

  readWordAndVariable(infile, "overconsumption", overconsumption);
  if (overconsumption != 0 && overconsumption != 1)
    handle.Message("Error in stockdistribution - overconsumption must be 0 or 1");

  //JMB - changed to make the reading of minimum probability optional
  infile >> ws;
  char c = infile.peek();
  if ((c == 'm') || (c == 'M'))
    readWordAndVariable(infile, "minimumprobability", epsilon);
  else if ((c == 'e') || (c == 'E'))
    readWordAndVariable(infile, "epsilon", epsilon);
  else
    epsilon = 10;

  if (epsilon <= 0) {
    handle.Warning("Epsilon should be a positive integer - set to default value 10");
    epsilon = 10;
  }

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

  //Read in length aggregation from file
  readWordAndValue(infile, "lenaggfile", aggfilename);
  datafile.open(aggfilename);
  checkIfFailure(datafile, aggfilename);
  handle.Open(aggfilename);
  numlen = readLengthAggregation(subdata, lengths, lenindex);
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
  datafile.open(datafilename);
  checkIfFailure(datafile, datafilename);
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
  int count = 0;
  IntVector Years, Steps;

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
  if (countColumns(infile) != 7)
    handle.Message("Wrong number of columns in inputfile - should be 7");

  while (!infile.eof()) {
    keepdata = 0;
    infile >> year >> step >> tmparea >> tmpstock >> tmpage >> tmplen >> tmpnumber >> ws;

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
        timeid = (Years.Size() - 1);

        AgeLengthData.AddRows(1, numarea);
        Proportions.AddRows(1, numarea);
        for (i = 0; i < numarea; i++) {
          AgeLengthData[timeid][i] = new DoubleMatrix(numstock, (numage * numlen), 0.0);
          Proportions[timeid][i] = new DoubleMatrix(numstock, (numage * numlen), 0.0);
        }
      }

    } else {
      //dont keep the data
      keepdata = 1;
    }

    if (keepdata == 0) {
      //stock distribution data is required, so store it
      count++;
      i = ageid + (numage * lenid);
      //JMB - this should be stored as [time][area][stock][age][length]
      (*AgeLengthData[timeid][areaid])[stockid][i] = tmpnumber;
    }
  }
  AAT.AddActions(Years, Steps, TimeInfo);
  if (count == 0)
    cerr << "Warning in stockdistribution - found no data in the data file for " << sdname << endl;
}

StockDistribution::~StockDistribution() {
  int i, j;
  for (i = 0; i < stocknames.Size(); i++) {
    delete[] stocknames[i];
    delete aggregator[i];
  }
  delete[] aggregator;
  delete[] functionname;
  delete[] sdname;
  delete lgrpdiv;

  for (i = 0; i < fleetnames.Size(); i++)
    delete[] fleetnames[i];
  for (i = 0; i < areaindex.Size(); i++)
    delete[] areaindex[i];
  for (i = 0; i < ageindex.Size(); i++)
    delete[] ageindex[i];
  for (i = 0; i < lenindex.Size(); i++)
    delete[] lenindex[i];
  for (i = 0; i < AgeLengthData.Nrow(); i++)
    for (j = 0; j < AgeLengthData.Ncol(i); j++) {
      delete AgeLengthData[i][j];
      delete Proportions[i][j];
    }
}

void StockDistribution::Reset(const Keeper* const keeper) {
  Likelihood::Reset(keeper);
  timeindex = 0;
}

void StockDistribution::Print(ofstream& outfile) const {
  int i;
  outfile << "\nStock Distribution " << sdname << " - likelihood value " << likelihood
    << "\n\tFunction " << functionname << "\n\tStock names:";
  for (i = 0; i < stocknames.Size(); i++)
    outfile << sep << stocknames[i];
  outfile << "\n\tFleet names:";
  for (i = 0; i < fleetnames.Size(); i++)
    outfile << sep << fleetnames[i];
  outfile << endl;
  for (i = 0; i < stocknames.Size(); i++)
    aggregator[i]->Print(outfile);
  outfile.flush();
}

void StockDistribution::SetFleetsAndStocks(FleetPtrVector& Fleets, StockPtrVector& Stocks) {
  int i, j, found, stocknumber;
  FleetPtrVector fleets;
  aggregator = new FleetPreyAggregator*[stocknames.Size()];

  for (i = 0; i < fleetnames.Size(); i++) {
    found = 0;
    for (j = 0; j < Fleets.Size(); j++)
      if (strcasecmp(fleetnames[i], Fleets[j]->Name()) == 0) {
        found = 1;
        fleets.resize(1, Fleets[j]);
      }
    if (found == 0) {
      cerr << "Error: when searching for names of fleets for stockdistribution.\n"
        << "Did not find any name matching " << fleetnames[i] << endl;
      exit(EXIT_FAILURE);
    }
  }

  for (i = 0; i < stocknames.Size(); i++) {
    StockPtrVector stocks;
    found = 0;
    for (j = 0; j < Stocks.Size(); j++) {
      if (Stocks[j]->IsEaten())
        if (strcasecmp(stocknames[i], Stocks[j]->ReturnPrey()->Name()) == 0) {
          found = 1;
          stocks.resize(1, Stocks[j]);
        }
    }
    if (found == 0) {
      cerr << "Error: when searching for names of stocks for stockdistribution.\n"
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
      case 2:
        likelihood += LikSumSquares();
        break;
      default:
        cerr << "Error in stockdistribution - unknown function " << functionname << endl;
        break;
    }
    timeindex++;
  }
}

//The code here is probably unnessecarily complicated because
//is always only one length group with this class.
double StockDistribution::LikMultinomial() {
  const AgeBandMatrixPtrVector* alptr;
  DoubleMatrixPtrVector Dist(areas.Nrow(), NULL);
  int nareas, area, age, length, sn;
  int minage, maxage;
  double temp;

  for (area = 0; area < Dist.Size(); area++) {
    Dist[area] = new DoubleMatrix(aggregator[0]->NoAgeGroups() *
      aggregator[0]->NoLengthGroups(), stocknames.Size(), 0.0);
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
  Multinomial MN(epsilon);
  DoubleVector likdata(stocknames.Size());
  for (nareas = 0; nareas < Dist.Size(); nareas++) {
    for (area = 0; area < Dist[nareas]->Nrow(); area++) {
      for (sn = 0; sn < stocknames.Size(); sn++)
        likdata[sn] = (*AgeLengthData[timeindex][nareas])[sn][area];

      temp = MN.CalcLogLikelihood(likdata, (*Dist[nareas])[area]);
    }
    delete Dist[nareas];
  }
  return MN.ReturnLogLikelihood();
}

double StockDistribution::LikSumSquares() {

  double totallikelihood = 0.0, temp = 0.0;
  double totalmodel, totaldata;
  int age, len, nareas, sn, i;

  const AgeBandMatrixPtrVector* alptr;
  for (nareas = 0; nareas < areas.Nrow(); nareas++) {
    for (sn = 0; sn < stocknames.Size(); sn++) {
      alptr = &aggregator[sn]->AgeLengthDist();
      totalmodel = 0.0;
      totaldata = 0.0;

      for (age = (*alptr)[nareas].Minage(); age <= (*alptr)[nareas].Maxage(); age++) {
        for (len = (*alptr)[nareas].Minlength(age); len < (*alptr)[nareas].Maxlength(age); len++) {
          i = age + (ages.Nrow() * len);
          (*Proportions[timeindex][nareas])[sn][i] = ((*alptr)[nareas][age][len]).N;
          totalmodel += (*Proportions[timeindex][nareas])[sn][i];
          totaldata += (*AgeLengthData[timeindex][nareas])[sn][i];
        }
      }

      for (age = (*alptr)[nareas].Minage(); age <= (*alptr)[nareas].Maxage(); age++) {
        for (len = (*alptr)[nareas].Minlength(age); len < (*alptr)[nareas].Maxlength(age); len++) {
          i = age + (ages.Nrow() * len);
          if ((isZero(totaldata)) && (isZero(totalmodel)))
            temp = 0.0;
          else if (isZero(totaldata))
            temp = (*Proportions[timeindex][nareas])[sn][i] / totalmodel;
          else if (isZero(totalmodel))
            temp = (*AgeLengthData[timeindex][nareas])[sn][i] / totaldata;
          else
            temp = (((*AgeLengthData[timeindex][nareas])[sn][i] / totaldata)
              - ((*Proportions[timeindex][nareas])[sn][i] / totalmodel));

          totallikelihood += (temp * temp);
        }
      }
    }
  }
  return totallikelihood;
}
