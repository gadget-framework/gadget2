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

extern ErrorHandler handle;

StockDistribution::StockDistribution(CommentStream& infile,
  const AreaClass* const Area, const TimeClass* const TimeInfo,
  double weight, const char* name)
  : Likelihood(STOCKDISTRIBUTIONLIKELIHOOD, weight) {

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

  //Must change from outer areas to inner areas.
  for (i = 0; i < areas.Nrow(); i++)
    for (j = 0; j < areas.Ncol(i); j++)
      if ((areas[i][j] = Area->InnerArea(areas[i][j])) == -1)
        handle.UndefinedArea(areas[i][j]);

  //Must create the length group division
  LgrpDiv = new LengthGroupDivision(lengths);

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
  readStockData(subdata, TimeInfo, numarea, numage, numlen);
  handle.Close();
  datafile.close();
  datafile.clear();

  likelihoodValues.AddRows(obsDistribution.Nrow(), numarea, 0.0);
}

void StockDistribution::readStockData(CommentStream& infile,
  const TimeClass* TimeInfo, int numarea, int numage, int numlen) {

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

  //Find start of distribution data in datafile
  infile >> ws;
  char c = infile.peek();
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

        obsDistribution.AddRows(1, numarea);
        modelDistribution.AddRows(1, numarea);
        for (i = 0; i < numarea; i++) {
          obsDistribution[timeid][i] = new DoubleMatrix(numstock, (numage * numlen), 0.0);
          modelDistribution[timeid][i] = new DoubleMatrix(numstock, (numage * numlen), 0.0);
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
      (*obsDistribution[timeid][areaid])[stockid][i] = tmpnumber;
    }
  }
  AAT.addActions(Years, Steps, TimeInfo);
  if (count == 0)
    handle.logWarning("Warning in stockdistribution - found no data in the data file for", sdname);
  handle.logMessage("Read stockdistribution data file - number of entries", count);
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
  delete LgrpDiv;

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
}

void StockDistribution::Reset(const Keeper* const keeper) {
  Likelihood::Reset(keeper);
  timeindex = 0;
  handle.logMessage("Reset stockdistribution component", sdname);
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

void StockDistribution::setFleetsAndStocks(FleetPtrVector& Fleets, StockPtrVector& Stocks) {
  int i, j, found, stocknumber;
  FleetPtrVector fleets;
  aggregator = new FleetPreyAggregator*[stocknames.Size()];

  for (i = 0; i < fleetnames.Size(); i++) {
    found = 0;
    for (j = 0; j < Fleets.Size(); j++)
      if (strcasecmp(fleetnames[i], Fleets[j]->Name()) == 0) {
        found++;
        fleets.resize(1, Fleets[j]);
      }
    if (found == 0)
      handle.logFailure("Error in stockdistribution - unknown fleet", fleetnames[i]);

  }

  for (i = 0; i < stocknames.Size(); i++) {
    StockPtrVector stocks;
    found = 0;
    for (j = 0; j < Stocks.Size(); j++) {
      if (Stocks[j]->IsEaten())
        if (strcasecmp(stocknames[i], Stocks[j]->returnPrey()->Name()) == 0) {
          found++;
          stocks.resize(1, Stocks[j]);
        }
    }
    if (found == 0)
      handle.logFailure("Error in stockdistribution - unknown stock", stocknames[i]);

    aggregator[i] = new FleetPreyAggregator(fleets, stocks, LgrpDiv, areas, ages, overconsumption);
  }
}

void StockDistribution::addLikelihood(const TimeClass* const TimeInfo) {
  int i;
  double l = 0.0;
  if (AAT.AtCurrentTime(TimeInfo)) {
    handle.logMessage("Calculating likelihood score for stockdistribution component", sdname);
    for (i = 0; i < stocknames.Size(); i++)
      aggregator[i]->Sum(TimeInfo);

    switch(functionnumber) {
      case 1:
        l = calcLikMultinomial();
        break;
      case 2:
        l = calcLikSumSquares();
        break;
      default:
        handle.logWarning("Warning in stockdistribution - unknown function", functionname);
        break;
    }
    likelihood += l;
    handle.logMessage("The likelihood score for this component on this timestep is", l);
    timeindex++;
  }
}

//The code here is probably unnessecarily complicated because
//is always only one length group with this class.
double StockDistribution::calcLikMultinomial() {
  const AgeBandMatrixPtrVector* alptr;
  DoubleMatrixPtrVector Dist(areas.Nrow());
  int nareas, area, age, len, sn;
  int minage, maxage, i;

  for (area = 0; area < Dist.Size(); area++) {
    Dist[area] = new DoubleMatrix(aggregator[0]->numAgeGroups() *
      aggregator[0]->numLengthGroups(), stocknames.Size(), 0.0);
    for (sn = 0; sn < stocknames.Size(); sn++) {
      alptr = &aggregator[sn]->returnSum();
      minage = (*alptr)[area].minAge();
      maxage = (*alptr)[area].maxAge();
      for (age = minage; age <= maxage; age++) {
        for (len = (*alptr)[area].minLength(age); len < (*alptr)[area].maxLength(age); len++) {
          i = age + (ages.Nrow() * len);
          (*modelDistribution[timeindex][area])[sn][i] = ((*alptr)[area][age][len]).N;
          (*Dist[area])[i][sn] = ((*alptr)[area][age][len]).N;
        }
      }
    }
  }

  //The object MN does most of the work, accumulating likelihood
  Multinomial MN(epsilon);
  DoubleVector likdata(stocknames.Size());
  for (nareas = 0; nareas < Dist.Size(); nareas++) {
    likelihoodValues[timeindex][nareas] = 0.0;
    for (area = 0; area < Dist[nareas]->Nrow(); area++) {
      for (sn = 0; sn < stocknames.Size(); sn++)
        likdata[sn] = (*obsDistribution[timeindex][nareas])[sn][area];

      likelihoodValues[timeindex][nareas] += MN.calcLogLikelihood(likdata, (*Dist[nareas])[area]);
    }
    delete Dist[nareas];
  }
  return MN.returnLogLikelihood();
}

double StockDistribution::calcLikSumSquares() {

  double totallikelihood = 0.0, temp = 0.0;
  double totalmodel, totaldata;
  int age, len, nareas, sn, i;

  const AgeBandMatrixPtrVector* alptr;
  for (nareas = 0; nareas < areas.Nrow(); nareas++) {
    likelihoodValues[timeindex][nareas] = 0.0;
    for (sn = 0; sn < stocknames.Size(); sn++) {
      alptr = &aggregator[sn]->returnSum();
      totalmodel = 0.0;
      totaldata = 0.0;

      for (age = (*alptr)[nareas].minAge(); age <= (*alptr)[nareas].maxAge(); age++) {
        for (len = (*alptr)[nareas].minLength(age); len < (*alptr)[nareas].maxLength(age); len++) {
          i = age + (ages.Nrow() * len);
          (*modelDistribution[timeindex][nareas])[sn][i] = ((*alptr)[nareas][age][len]).N;
          totalmodel += (*modelDistribution[timeindex][nareas])[sn][i];
          totaldata += (*obsDistribution[timeindex][nareas])[sn][i];
        }
      }

      for (age = (*alptr)[nareas].minAge(); age <= (*alptr)[nareas].maxAge(); age++) {
        for (len = (*alptr)[nareas].minLength(age); len < (*alptr)[nareas].maxLength(age); len++) {
          i = age + (ages.Nrow() * len);
          if ((isZero(totaldata)) && (isZero(totalmodel)))
            temp = 0.0;
          else if (isZero(totaldata))
            temp = (*modelDistribution[timeindex][nareas])[sn][i] / totalmodel;
          else if (isZero(totalmodel))
            temp = (*obsDistribution[timeindex][nareas])[sn][i] / totaldata;
          else
            temp = (((*obsDistribution[timeindex][nareas])[sn][i] / totaldata)
              - ((*modelDistribution[timeindex][nareas])[sn][i] / totalmodel));

          likelihoodValues[timeindex][nareas] += (temp * temp);
        }
      }
    }
    totallikelihood += likelihoodValues[timeindex][nareas];
  }
  return totallikelihood;
}

void StockDistribution::LikelihoodPrint(ofstream& outfile) {
  int i, j, y, a;

  outfile << "\nStock Distribution " << sdname << "\n\nLikelihood " << likelihood
    << "\nFunction " << functionname << "\nWeight " << weight << "\nStock names:";
  for (i = 0; i < stocknames.Size(); i++)
    outfile << sep << stocknames[i];
  outfile << "\nInternal areas:";
  for (i  = 0; i < areas.Nrow(); i++) {
    outfile << endl;
    for (j = 0; j < areas.Ncol(i); j++)
      outfile << areas[i][j] << sep;
  }
  outfile << "\nAges:";
  for (i  = 0; i < ages.Nrow(); i++) {
    outfile << endl;
    for (j = 0; j < ages.Ncol(i); j++)
      outfile << ages[i][j] << sep;
  }
  outfile << "\nLengths:";
  for (i = 0; i < lengths.Size(); i++)
    outfile << sep << lengths[i];
  outfile << "\nFleet names:";
  for (i = 0; i < fleetnames.Size(); i++)
    outfile << sep << fleetnames[i];
  outfile << endl;

  outfile << "\nAge length distribution data:\n";
  for (y = 0; y < obsDistribution.Nrow(); y++) {
    outfile << "\nYear " << Years[y] << " and step " << Steps[y];
    for (a = 0; a < obsDistribution.Ncol(y); a++) {
      outfile << "\nInternal area: " << a << "\nMeasurements";
      for (i = 0; i < obsDistribution[y][a]->Nrow(); i++) {
        outfile << endl;
        for (j = 0; j < obsDistribution[y][a]->Ncol(i); j++) {
          outfile.width(smallwidth);
          outfile.precision(smallprecision);
          outfile << sep << (*obsDistribution[y][a])[i][j];
        }
      }
      outfile << "\nNumber caught according to model";
      for (i = 0; i < modelDistribution[y][a]->Nrow(); i++) {
        outfile << endl;
        for (j = 0; j < modelDistribution[y][a]->Ncol(i); j++) {
          outfile.width(smallwidth);
          outfile.precision(smallprecision);
          outfile << sep << (*modelDistribution[y][a])[i][j];
        }
      }
    }
    outfile << "\nLikelihood values:";
    for (a = 0; a < obsDistribution.Ncol(y); a++) {
      outfile.width(smallwidth);
      outfile.precision(smallprecision);
      outfile << sep << likelihoodValues[y][a];
    }
    outfile << endl;
  }
  outfile.flush();
}

void StockDistribution::SummaryPrint(ofstream& outfile) {
  int year, area;

  for (year = 0; year < likelihoodValues.Nrow(); year++)
    for (area = 0; area < likelihoodValues.Ncol(year); area++)
      outfile << setw(lowwidth) << Years[year] << sep << setw(lowwidth)
        << Steps[year] << sep << setw(printwidth) << areaindex[area] << sep
        << setw(largewidth) << sdname << sep << setw(smallwidth) << weight << sep
        << setprecision(largeprecision) << setw(largewidth)
        << likelihoodValues[year][area] << endl;

  outfile.flush();
}
