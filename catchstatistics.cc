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
  const TimeClass* const TimeInfo, double likweight)
  : Likelihood(CATCHSTATISTICSLIKELIHOOD, likweight) {

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
  ReadWordAndValue(infile, "datafile", datafilename);
  ReadWordAndVariable(infile, "functionnumber", functionnumber);
  ReadWordAndVariable(infile, "overconsumption", overconsumption);

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
  CheckIfFailure(datafile, datafilename);
  handle.Open(datafilename);
  ReadStatisticsData(subdata, TimeInfo, numarea, numage);
  handle.Close();
  datafile.close();
  datafile.clear();
}

void CatchStatistics::ReadStatisticsData(CommentStream& infile,
  const TimeClass* TimeInfo, int numarea, int numage) {

  int i;
  intvector Years, Steps;
  int year, step;
  double tmpnumber, tmpmean, tmpvariance;
  char tmparea[MaxStrLength], tmpage[MaxStrLength];
  strncpy(tmparea, "", MaxStrLength);
  strncpy(tmpage, "", MaxStrLength);
  int keepdata, needvar;
  int timeid, ageid, areaid;
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
    case 5:
      needvar = 1;
  }

  //Check the number of columns in the inputfile
  if (needvar == 1) {
    if (!(CheckColumns(infile, 7)))
      handle.Message("Wrong number of columns in inputfile - should be 7");
  } else {
    if (!(CheckColumns(infile, 6)))
      handle.Message("Wrong number of columns in inputfile - should be 6");
  }

  while (!infile.eof()) {
    keepdata = 0;
    if (needvar == 1)
      infile >> year >> step >> tmparea >> tmpage >> tmpnumber >> tmpmean >> tmpvariance >> ws;
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
        numbers.resize(1, new doublematrix(numarea, numage, 0.0));
        mean.resize(1, new doublematrix(numarea, numage, 0.0));
        if (needvar == 1)
          variance.resize(1, new doublematrix(numarea, numage, 0.0));
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
      (*numbers[timeid])[areaid][ageid] = tmpnumber;
      (*mean[timeid])[areaid][ageid] = tmpmean;
      if (needvar == 1)
        (*variance[timeid])[areaid][ageid] = tmpvariance;

    }
  }
  AAT.AddActions(Years, Steps, TimeInfo);
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
}

void CatchStatistics::Reset(const Keeper* const keeper) {
  Likelihood::Reset(keeper);
  timeindex = 0;
}

void CatchStatistics::Print(ofstream& outfile) const {
  int i, j, y;

  outfile << "\nCatch Statistics\nlikelihood " << likelihood
    << "\nfunction number " << functionnumber;
  outfile << "\n\tStocknames: " << sep;
  for (i = 0; i < stocknames.Size(); i++)
    outfile << stocknames[i] << sep;
  outfile << "\n\tFleetnames: " << sep;
  for (i = 0; i < fleetnames.Size(); i++)
    outfile << fleetnames[i] << sep;
  outfile << endl;

  aggregator->Print(outfile);
  outfile << "\tStatistics Data:\n";
  for (y = 0; y < mean.Size(); y++) {
    outfile << "\tyear and step " << y << "\n\tmean\n";
    for (i = 0; i < mean[y]->Nrow(); i++) {
      outfile << TAB;
      for (j = 0; j < mean[y]->Ncol(i); j++) {
        outfile.width(printwidth);
        outfile.precision(smallprecision);
        outfile << (*mean[y])[i][j] << sep;
      }
      outfile << endl;
    }
    outfile << "\tvariance\n";
    if (variance.Size() != 0) {
      for (i = 0; i < variance[y]->Nrow(); i++) {
        outfile << TAB;
        for (j = 0; j < variance[y]->Ncol(i); j++) {
          outfile.width(printwidth);
          outfile.precision(smallprecision);
          outfile << (*variance[y])[i][j] << sep;
        }
      }
      outfile << endl;
    }
  }
  outfile.flush();
}

void CatchStatistics::SetFleetsAndStocks(Fleetptrvector& Fleets, Stockptrvector& Stocks) {
  int i, j, found;
  Fleetptrvector fleets;
  Stockptrvector stocks;

  for (i = 0; i < fleetnames.Size(); i++) {
    found = 0;
    for (j = 0; j < Fleets.Size(); j++)
      if (strcasecmp(fleetnames[i], Fleets[j]->Name()) == 0) {
        found = 1;
        fleets.resize(1, Fleets[j]);
      }

    if (found == 0) {
      cerr << "Error when searching for names of fleets for Catchstatistics\n"
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
      cerr << "Error when searching for names of stocks for Catchstatistics\n"
        << "Did not find any name matching " << stocknames[i] << endl;
      exit(EXIT_FAILURE);
    }
  }

  LengthGroupDivision* LgrpDiv =
    new LengthGroupDivision(*(stocks[0]->ReturnPrey()->ReturnLengthGroupDiv()));

  for (i = 1; i < stocks.Size(); i++)
    if (!LgrpDiv->Combine(stocks[i]->ReturnPrey()->ReturnLengthGroupDiv())) {
      cerr << "Length group divisions for preys in Catchstatistic not compatible\n";
      exit(EXIT_FAILURE);
    }

  aggregator = new FleetPreyAggregator(fleets, stocks, LgrpDiv, areas, ages, overconsumption);
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
  double sim_mean, sim_var, sim_number, tmp = 0.0;

  const Agebandmatrixvector *alptr = &aggregator->AgeLengthDist();
  for (nareas = 0; nareas < alptr->Size(); nareas++)
    for (age = 0; age < (*alptr)[nareas].Nrow(); age++) {
      PopStatistics PopStat((*alptr)[nareas][age], aggregator->ReturnLgrpDiv());
      sim_mean = 0.0;
      sim_var = 0.0;
      sim_number = 0.0;

      switch(functionnumber) {
        case 1:
          sim_mean = PopStat.MeanLength();
          sim_var = PopStat.StdDevOfLength();
          sim_var *= sim_var;
          break;
        case 2:
        case 5:
          sim_mean = PopStat.MeanLength();
          sim_var = (*variance[timeindex])[nareas][age];
          break;
        case 3:
          sim_mean = PopStat.MeanWeight();
          sim_var = (*variance[timeindex])[nareas][age];
          break;
        case 4:
          sim_mean = PopStat.MeanWeight();
          sim_var = 1.0;
          break;
        default:
          cerr << "Error in catchstatistics - unknown functionnumber " << functionnumber << endl;
          break;
      }
      switch(functionnumber) {
        case 1:
        case 2:
        case 3:
        case 4:
          /* THLTH 10.03.01: Changed this to make it diff. and to add a penalty, so that
           * when the model is fishing out all the fish (aka the number in the model goes
           * down to zero and therefore the mean length or weight as well), we get the
           * value l^2 for that timestep, where l is the meanlength/-weight in the sample
           * (that is, we get some fairly high number, instead of zero before).
           * Further, the reason why the sim_number is multiplied with a 20 is simply to
           * avoid the penalty to come in to quickly, that is for to large values of
           * sim_number. */
          sim_number = 20 * PopStat.TotalNumber();
          tmp = (*mean[timeindex])[nareas][age] - sim_mean;
          lik += tmp * tmp * (sim_var > 0 ? 1 / sim_var : 0) * (*numbers[timeindex])[nareas][age] *
            (1 - 1 / exp(sim_number)) + (*mean[timeindex])[nareas][age] *
            (*mean[timeindex])[nareas][age] * 1 / exp(sim_number);
          break;
        case 5:
          if (iszero(sim_mean))
            lik += (*numbers[timeindex])[nareas][age];
          else if (sim_mean > verysmall && (*mean[timeindex])[nareas][age] > verysmall) {
            //We don't want to calculate the variance if there is no catch
            tmp = (*mean[timeindex])[nareas][age] - sim_mean;
            lik += tmp * tmp * (sim_var > 0 ? 1 / sim_var : 0) * (*numbers[timeindex])[nareas][age];
          }
          break;
        default:
          cerr << "Error in catchstatistics - unknown functionnumber " << functionnumber << endl;
          break;
      }
    }
  return lik;
}
