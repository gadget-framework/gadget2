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

RecStatistics::RecStatistics(CommentStream& infile, const AreaClass* const Area,
  const TimeClass* const TimeInfo, double w, TagPtrVector Tags, const char* name)
  : Likelihood(RECSTATISTICSLIKELIHOOD, w) {

  lgrpDiv = NULL;
  aggregator = 0;
  ErrorHandler handle;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  int i, j, numarea = 0;

  char datafilename[MaxStrLength];
  char aggfilename[MaxStrLength];
  strncpy(datafilename, "", MaxStrLength);
  strncpy(aggfilename, "", MaxStrLength);
  ifstream datafile;
  CommentStream subdata(datafile);

  rsname = new char[strlen(name) + 1];
  strcpy(rsname, name);

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

  //Read in area aggregation from file
  readWordAndValue(infile, "areaaggfile", aggfilename);
  datafile.open(aggfilename);
  checkIfFailure(datafile, aggfilename);
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

  //Read in the fleetnames
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
  datafile.open(datafilename);
  checkIfFailure(datafile, datafilename);
  handle.Open(datafilename);
  ReadStatisticsData(subdata, TimeInfo, numarea, Tags);
  handle.Close();
  datafile.close();
  datafile.clear();
}

void RecStatistics::ReadStatisticsData(CommentStream& infile,
  const TimeClass* TimeInfo, int numarea, TagPtrVector Tags) {

  int year, step;
  double tmpnumber, tmpmean, tmpstddev;
  char tmparea[MaxStrLength], tmptag[MaxStrLength];
  strncpy(tmparea, "", MaxStrLength);
  strncpy(tmptag, "", MaxStrLength);
  int keepdata, needvar;
  int i, timeid, tagid, areaid, tmpindex;
  int count = 0;
  ErrorHandler handle;

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
    if ((TimeInfo->IsWithinPeriod(year, step)) && (keepdata == 0)) {
      for (i = 0; i < tagvec.Size(); i++)
        if (strcasecmp(tagvec[i]->TagName(), tmptag) == 0)
          tagid = i;

      if (tagid == -1) {
        tmpindex = -1;
        for (i = 0; i < Tags.Size(); i++)
          if (strcasecmp(Tags[i]->TagName(), tmptag) == 0)
            tmpindex = i;

        if (tmpindex == -1) {
          keepdata = 1;
        } else {
          tagvec.resize(1, Tags[tmpindex]);
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
    cerr << "Warning in recstatistics - found no data in the data file for " << rsname << endl;
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
  delete[] rsname;
  delete[] functionname;
  if (aggregator != 0)  {
    for (i = 0; i < tagvec.Size(); i++)
      delete aggregator[i];
    delete[] aggregator;
    aggregator = 0;
  }
  if (lgrpDiv != NULL) {
    delete lgrpDiv;
    lgrpDiv = NULL;
  }
}

void RecStatistics::Reset(const Keeper* const keeper) {
  int i;
  Likelihood::Reset(keeper);
  for (i = 0; i < timeindex.Size(); i++)
    timeindex[i] = -1;
}

void RecStatistics::Print(ofstream& outfile) const {
  int i;

  outfile << "\nRecapture Statistics " << rsname << " - likelihood value " << likelihood
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

void RecStatistics::SetFleetsAndStocks(FleetPtrVector& Fleets, StockPtrVector& Stocks) {
  int i, j, t, found, minage, maxage;
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
      cerr << "Error when searching for names of fleets for recstatistics\n"
        << "Did not find any name matching " << fleetnames[i] << endl;
      exit(EXIT_FAILURE);
    }
  }

  aggregator = new RecAggregator*[tagvec.Size()];
  for (t = 0; t < tagvec.Size(); t++) {
    const CharPtrVector* stocknames = tagvec[t]->getStocknames();
    for (i = 0; i < stocknames->Size(); i++) {
      found = 0;
      for (j = 0; j < Stocks.Size(); j++)
        if (Stocks[j]->IsEaten())
          if (strcasecmp(stocknames->operator[](i), Stocks[j]->ReturnPrey()->Name()) == 0) {
            found = 1;
            stocks.resize(1, Stocks[j]);
          }

      if (found == 0) {
        cerr << "Error when searching for names of stocks for recstatistics\n"
          << "Did not find any name matching " << stocknames->operator[](i) << endl;
        exit(EXIT_FAILURE);
      }
    }

    lgrpDiv = new LengthGroupDivision(*(stocks[0]->ReturnPrey()->ReturnLengthGroupDiv()));
    for (i = 1; i < stocks.Size(); i++)
      if (!lgrpDiv->Combine(stocks[i]->ReturnPrey()->ReturnLengthGroupDiv())) {
        cerr << "Length group divisions for preys in recstatistics not compatible\n";
        exit(EXIT_FAILURE);
      }

    minage = 999;
    maxage = 0;
    for (i = 0; i < stocks.Size(); i++) {
      minage = (minage < stocks[i]->Minage() ? minage : stocks[i]->Minage());
      maxage = (maxage > stocks[i]->Maxage() ? maxage : stocks[i]->Maxage());
    }

    IntMatrix ages(1, 0);
    for (i = 0; i <= maxage - minage; i++)
      ages[0].resize(1, minage + i);
    aggregator[t] = new RecAggregator(fleets, stocks, lgrpDiv, areas, ages, tagvec[t]);

    while (stocks.Size() > 0)
      stocks.Delete(0);
  }
}

void RecStatistics::AddToLikelihood(const TimeClass* const TimeInfo) {
  int t, i;
  for (t = 0; t < tagvec.Size(); t++) {
    timeindex[t] = -1;
    if (tagvec[t]->IsWithinPeriod(TimeInfo->CurrentYear(), TimeInfo->CurrentStep())) {
      for (i = 0; i < Years.Ncol(t); i++)
        if (Years[t][i] == TimeInfo->CurrentYear() && Steps[t][i] == TimeInfo->CurrentStep())
          timeindex[t] = i;
          aggregator[t]->Sum(TimeInfo);
    }
  }
  likelihood += SOSWeightOrLength();
}

double RecStatistics::SOSWeightOrLength() {
  double lik = 0.0;
  int t, nareas;
  double simmean, simvar, simnumber, simdiff;

  for (t = 0; t < tagvec.Size(); t++) {
    if (timeindex[t] > -1) {
      const AgeBandMatrixPtrVector *alptr = &aggregator[t]->AgeLengthDist();
      for (nareas = 0; nareas < alptr->Size(); nareas++) {
        PopStatistics PopStat((*alptr)[nareas][0], aggregator[t]->ReturnLgrpDiv(), 1);

        simmean = PopStat.MeanLength();
        simdiff = simmean - (*mean[t])[timeindex[t]][nareas];
        simnumber = PopStat.TotalNumber();
        simvar = 0.0;

        switch(functionnumber) {
          case 1:
            simvar = PopStat.StdDevOfLength() * PopStat.StdDevOfLength();
            break;
          case 2:
            simvar = (*variance[t])[timeindex[t]][nareas];
            break;
          case 3:
            simvar = 1.0;
            break;
          default:
            cerr << "Error in recstatistics - unknown function " << functionname << endl;
            break;
        }

        lik += simdiff * simdiff * (simvar > 0 ? 1 / simvar : 0.0) * (*numbers[t])[timeindex[t]][nareas]
          * (1 - exp(-20 * simnumber)) + ((*mean[t])[timeindex[t]][nareas]
          * (*mean[t])[timeindex[t]][nareas] * exp(-20 * simnumber));
      }
    }
  }
  return lik;
}
