#include "recstatistics.h"
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

RecStatistics::RecStatistics(CommentStream& infile, const AreaClass* const Area,
  const TimeClass* const TimeInfo, double weight, TagPtrVector Tag, const char* name)
  : Likelihood(RECSTATISTICSLIKELIHOOD, weight, name) {

  aggregator = 0;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  int i, j, numarea = 0;

  char datafilename[MaxStrLength];
  char aggfilename[MaxStrLength];
  strncpy(datafilename, "", MaxStrLength);
  strncpy(aggfilename, "", MaxStrLength);
  ifstream datafile;
  CommentStream subdata(datafile);

  functionname = new char[MaxStrLength];
  strncpy(functionname, "", MaxStrLength);
  readWordAndValue(infile, "datafile", datafilename);
  readWordAndValue(infile, "function", functionname);

  if ((strcasecmp(functionname, "lengthcalcvar") == 0) || (strcasecmp(functionname, "lengthcalcstddev") == 0))
    functionnumber = 1;
  else if ((strcasecmp(functionname, "lengthgivenvar") == 0) || (strcasecmp(functionname, "lengthgivenstddev") == 0))
    functionnumber = 2;
  else if ((strcasecmp(functionname, "lengthnovar") == 0) || (strcasecmp(functionname, "lengthnostddev") == 0))
    functionnumber = 3;
  else
    handle.Message("Error in recstatistics - unrecognised function", functionname);

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
      if ((areas[i][j] = Area->InnerArea(areas[i][j])) == -1)
        handle.UndefinedArea(areas[i][j]);

  //read in the fleetnames
  i = 0;
  infile >> text >> ws;
  if (!(strcasecmp(text, "fleetnames") == 0))
    handle.Unexpected("fleetnames", text);
  infile >> text;
  while (!infile.eof() && !(strcasecmp(text, "[component]") == 0)) {
    infile >> ws;
    fleetnames.resize(1);
    fleetnames[i] = new char[strlen(text) + 1];
    strcpy(fleetnames[i++], text);
    infile >> text;
  }

  //We have now read in all the data from the main likelihood file
  //But we have to read in the statistics data from datafilename
  datafile.open(datafilename, ios::in);
  handle.checkIfFailure(datafile, datafilename);
  handle.Open(datafilename);
  readStatisticsData(subdata, TimeInfo, numarea, Tag);
  handle.Close();
  datafile.close();
  datafile.clear();
}

void RecStatistics::readStatisticsData(CommentStream& infile,
  const TimeClass* TimeInfo, int numarea, TagPtrVector Tag) {

  int year, step;
  double tmpnumber, tmpmean, tmpstddev;
  char tmparea[MaxStrLength], tmptag[MaxStrLength];
  strncpy(tmparea, "", MaxStrLength);
  strncpy(tmptag, "", MaxStrLength);
  int keepdata, needvar;
  int i, timeid, tagid, areaid, tmpindex;
  int count = 0;

  if (functionnumber == 2)
    needvar = 1;
  else
    needvar = 0;

  //Check the number of columns in the inputfile
  infile >> ws;
  if ((needvar == 1) && (countColumns(infile) != 7))
    handle.Message("Wrong number of columns in inputfile - should be 7");
  else if ((needvar == 0) && (countColumns(infile) != 6))
    handle.Message("Wrong number of columns in inputfile - should be 6");

  while (!infile.eof()) {
    keepdata = 0;
    if (needvar == 1)
      infile >> tmptag >> year >> step >> tmparea >> tmpnumber >> tmpmean >> tmpstddev >> ws;
    else
      infile >> tmptag >> year >> step >> tmparea >> tmpnumber >> tmpmean >> ws;

    //if tmparea is in areaindex find areaid, else dont keep the data
    areaid = -1;
    for (i = 0; i < areaindex.Size(); i++)
      if (strcasecmp(areaindex[i], tmparea) == 0)
        areaid = i;

    if (areaid == -1)
      keepdata = 1;

    //check if the year and step are in the simulation
    timeid = -1;
    tagid = -1;
    if ((TimeInfo->isWithinPeriod(year, step)) && (keepdata == 0)) {
      for (i = 0; i < tagvec.Size(); i++)
        if (strcasecmp(tagvec[i]->Name(), tmptag) == 0)
          tagid = i;

      if (tagid == -1) {
        tmpindex = -1;
        for (i = 0; i < Tag.Size(); i++)
          if (strcasecmp(Tag[i]->Name(), tmptag) == 0)
            tmpindex = i;

        if (tmpindex == -1) {
          keepdata = 1;
        } else {
          tagvec.resize(1, Tag[tmpindex]);
          tagnames.resize(1, new char[strlen(tmptag) + 1]);
          strcpy(tagnames[tagnames.Size() - 1], tmptag);
          tagid = tagvec.Size() - 1;
          Years.AddRows(1, 1, year);
          Steps.AddRows(1, 1, step);
          timeid = 0;
          numbers.resize(1, new DoubleMatrix(1, numarea, 0.0));
          mean.resize(1, new DoubleMatrix(1, numarea, 0.0));
          if (needvar == 1)
            variance.resize(1, new DoubleMatrix(1, numarea, 0.0));
        }

      } else {
        for (i = 0; i < Years.Ncol(tagid); i++)
          if ((Years[tagid][i] == year) && (Steps[tagid][i] == step))
            timeid = i;

        //if this is a new timestep, resize to store the data
        if (timeid == -1) {
          Years[tagid].resize(1, year);
          Steps[tagid].resize(1, step);
          (*numbers[tagid]).AddRows(1, numarea, 0.0);
          (*mean[tagid]).AddRows(1, numarea, 0.0);
          if (needvar == 1)
            (*variance[tagid]).AddRows(1, numarea, 0.0);
          timeid = Years.Ncol(tagid) - 1;
        }
      }

    } else {
      //dont keep the data
      keepdata = 1;
    }

    if (keepdata == 0) {
      //statistics data is required, so store it
      count++;
      (*numbers[tagid])[timeid][areaid] = tmpnumber;
      (*mean[tagid])[timeid][areaid] = tmpmean;
      if (needvar == 1)
        (*variance[tagid])[timeid][areaid] = tmpstddev * tmpstddev;
    }
  }

  timeindex.resize(tagvec.Size(), -1);
  if (count == 0)
    handle.logWarning("Warning in recstatistics - found no data in the data file for", this->Name());
  handle.logMessage("Read recstatistics data file - number of entries", count);
}

RecStatistics::~RecStatistics() {
  int i;
  for (i = 0; i < fleetnames.Size(); i++)
    delete[] fleetnames[i];
  for (i = 0; i < tagnames.Size(); i++)
    delete[] tagnames[i];
  for (i = 0; i < areaindex.Size(); i++)
    delete[] areaindex[i];
  for (i = 0; i < numbers.Size(); i++) {
    delete numbers[i];
    delete mean[i];
  }
  for (i = 0; i < variance.Size(); i++)
    delete variance[i];
  delete[] functionname;
  if (aggregator != 0)  {
    for (i = 0; i < tagvec.Size(); i++)
      delete aggregator[i];
    delete[] aggregator;
    aggregator = 0;
  }
  delete LgrpDiv;
}

void RecStatistics::Reset(const Keeper* const keeper) {
  int i;
  Likelihood::Reset(keeper);
  for (i = 0; i < timeindex.Size(); i++)
    timeindex[i] = -1;
  handle.logMessage("Reset recstatistics component", this->Name());
}

void RecStatistics::Print(ofstream& outfile) const {
  int i;

  outfile << "\nRecapture Statistics " << this->Name() << " - likelihood value " << likelihood
    << "\n\tFunction " << functionname;
  outfile << "\n\tFleet names:";
  for (i = 0; i < fleetnames.Size(); i++)
    outfile << sep << fleetnames[i];
  outfile << endl;
  for (i = 0; i < tagnames.Size(); i++) {
    outfile << "\tTagging experiment:\t" << tagnames[i] << endl;
    aggregator[i]->Print(outfile);
    outfile << endl;
  }
  outfile.flush();
}

void RecStatistics::setFleetsAndStocks(FleetPtrVector& Fleets, StockPtrVector& Stocks) {
  int i, j, t, found, minage, maxage;
  FleetPtrVector fleets;
  StockPtrVector stocks;
  const CharPtrVector* stocknames;

  for (i = 0; i < fleetnames.Size(); i++) {
    found = 0;
    for (j = 0; j < Fleets.Size(); j++)
      if (strcasecmp(fleetnames[i], Fleets[j]->Name()) == 0) {
        found++;
        fleets.resize(1, Fleets[j]);
      }

    if (found == 0)
      handle.logFailure("Error in recstatistics - unrecognised fleet", fleetnames[i]);

  }

  aggregator = new RecAggregator*[tagvec.Size()];
  for (t = 0; t < tagvec.Size(); t++) {
    stocknames = tagvec[t]->getStockNames();
    for (i = 0; i < stocknames->Size(); i++) {
      found = 0;
      for (j = 0; j < Stocks.Size(); j++)
        if (Stocks[j]->isEaten())
          if (strcasecmp(stocknames->operator[](i), Stocks[j]->returnPrey()->Name()) == 0) {
            found++;
            stocks.resize(1, Stocks[j]);
          }

      if (found == 0)
        handle.logFailure("Error in recstatistics - unrecognised stock", stocknames->operator[](i));

    }

    LgrpDiv = new LengthGroupDivision(*(stocks[0]->returnPrey()->returnLengthGroupDiv()));
    for (i = 1; i < stocks.Size(); i++)
      if (!LgrpDiv->Combine(stocks[i]->returnPrey()->returnLengthGroupDiv()))
        handle.logFailure("Error in recstatistics - length groups not compatible");

    minage = 999;
    maxage = 0;
    for (i = 0; i < stocks.Size(); i++) {
      minage = (minage < stocks[i]->minAge() ? minage : stocks[i]->minAge());
      maxage = (maxage > stocks[i]->maxAge() ? maxage : stocks[i]->maxAge());
    }

    IntMatrix ages(1, 0);
    for (i = 0; i <= maxage - minage; i++)
      ages[0].resize(1, minage + i);
    aggregator[t] = new RecAggregator(fleets, stocks, LgrpDiv, areas, ages, tagvec[t]);

    while (stocks.Size() > 0)
      stocks.Delete(0);
  }
}

void RecStatistics::addLikelihood(const TimeClass* const TimeInfo) {
  int t, i, check;
  double l = 0.0;

  check = 0;
  for (t = 0; t < tagvec.Size(); t++) {
    timeindex[t] = -1;
    if (tagvec[t]->isWithinPeriod(TimeInfo->CurrentYear(), TimeInfo->CurrentStep())) {
      for (i = 0; i < Years.Ncol(t); i++) {
        if (Years[t][i] == TimeInfo->CurrentYear() && Steps[t][i] == TimeInfo->CurrentStep()) {
          if (check == 0)
            handle.logMessage("Calculating likelihood score for recstatistics component", this->Name());
          timeindex[t] = i;
          aggregator[t]->Sum(TimeInfo);
          check++;
        }
      }
    }
  }

  if (check > 0) {
    l = calcLikSumSquares();
    likelihood += l;
    handle.logMessage("The likelihood score for this component on this timestep is", l);
  }
}

double RecStatistics::calcLikSumSquares() {
  double lik = 0.0;
  int t, nareas;
  double simvar, simdiff;

  for (t = 0; t < tagvec.Size(); t++) {
    if (timeindex[t] > -1) {
      const AgeBandMatrixPtrVector *alptr = &aggregator[t]->returnSum();
      for (nareas = 0; nareas < alptr->Size(); nareas++) {
        PopStatistics PopStat((*alptr)[nareas][0], aggregator[t]->returnLengthGroupDiv(), 1);

        simdiff = PopStat.meanLength() - (*mean[t])[timeindex[t]][nareas];
        simvar = 0.0;

        switch(functionnumber) {
          case 1:
            simvar = PopStat.sdevLength() * PopStat.sdevLength();
            break;
          case 2:
            simvar = (*variance[t])[timeindex[t]][nareas];
            break;
          case 3:
            simvar = 1.0;
            break;
          default:
            handle.logWarning("Warning in recstatistics - unrecognised function", functionname);
            break;
        }

      if (isZero(simvar))
        lik += 0.0;
      else
        lik += simdiff * simdiff * (*numbers[t])[timeindex[t]][nareas] / simvar;

      }
    }
  }
  return lik;
}
