#include "logcatchfunction.h"
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

LogCatches::LogCatches(CommentStream& infile,
  const AreaClass* const Area, const TimeClass* const TimeInfo,
  double likweight) : Likelihood(LOGCATCHLIKELIHOOD, likweight) {

  ErrorHandler handle;
  int i, j;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  int numarea = 0, numage = 0, numlen = 0;

  char datafilename[MaxStrLength];
  char aggfilename[MaxStrLength];
  char weightfilename[MaxStrLength];
  strncpy(datafilename, "", MaxStrLength);
  strncpy(aggfilename, "", MaxStrLength);
  strncpy(weightfilename, "", MaxStrLength);
  ifstream datafile;
  CommentStream subdata(datafile);

  timeindex = 0;
  ReadWordAndValue(infile, "datafile", datafilename);
  //Check to see if we read in weights
  readWeights = 0;
  char c;
  c = infile.peek();
  if (c == 'w') {
    readWeights = 1;
    ReadWordAndValue(infile, "weightfile", weightfilename);
  }

  ReadWordAndVariable(infile, "functionnumber", functionnumber);
  ReadWordAndVariable(infile, "overconsumption", overconsumption);
  ReadWordAndVariable(infile, "minimumprobability", minp);
  ReadWordAndVariable(infile, "aggregation_level", agg_lev);
  if (minp <= 0) {
    handle.Warning("Minimumprobability should be a positive integer - set to default value 1");
    minp = 1;
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

  //Read in the fleetnames
  i = 0;
  infile >> text >> ws;
  if (!(strcasecmp(text, "fleetnames") == 0))
    handle.Unexpected("fleetnames", text);
  infile >> text;
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
  ReadLogCatchData(subdata, TimeInfo, numarea, numage, numlen);
  handle.Close();
  datafile.close();
  datafile.clear();

  //If readWeights then read in the weight info from file
  if (readWeights == 1) {
    datafile.open(weightfilename);
    CheckIfFailure(datafile, weightfilename);
    handle.Open(weightfilename);
    ReadLogWeightsData(subdata, TimeInfo, numlen);
    handle.Close();
    datafile.close();
    datafile.clear();
  }

  //Needed for -catchprint header
  MinAge = max(min_stock_age, ages[0][0]);
  MaxAge = min(max_stock_age, ages[ages.Nrow() - 1][0]);
  dl = lengths[1] - lengths[0];
  for (i = 2; i < lengths.Size(); i++)
    if (lengths[i] - lengths[i - 1] != dl) {
      dl = 0.0;
      break;
    }

  int numtime = AgeLengthData.Nrow();
  Proportions.AddRows(numtime, numarea);
  Likelihoodvalues.AddRows(numtime, numarea);
  for (i = 0; i < numtime; i++)
    for (j = 0; j <  AgeLengthData.Ncol(i); j++)
      Proportions[i][j] = new doublematrix(AgeLengthData[i][j]->Nrow(), AgeLengthData[i][j]->Ncol(), 0.0);

  calc_c.resize(numarea);
  obs_c.resize(numarea);
  for (i = 0; i < numarea; i++) {
    calc_c[i] = new doublematrix(AgeLengthData[0][i]->Nrow(), AgeLengthData[0][i]->Ncol(), 0.0);
    obs_c[i] = new doublematrix(AgeLengthData[0][i]->Nrow(), AgeLengthData[0][i]->Ncol(), 0.0);
  }

  obs_biomass.resize(numarea);
  calc_biomass.resize(numarea);
  agg_obs_biomass.resize(numarea);
  agg_calc_biomass.resize(numarea);
  for (i = 0; i < numarea; i++) {
    obs_biomass[i] = new doublematrix(numtime, AgeLengthData[0][i]->Nrow(), 0.0);
    calc_biomass[i] = new doublematrix(numtime, AgeLengthData[0][i]->Nrow(), 0.0);
    if (agg_lev) {
      agg_obs_biomass[i] = new doublematrix(numtime, AgeLengthData[0][i]->Nrow(), 0.0);
      agg_calc_biomass[i] = new doublematrix(numtime, AgeLengthData[0][i]->Nrow(), 0.0);
    }
  }
}

void LogCatches::ReadLogCatchData(CommentStream& infile,
  const TimeClass* TimeInfo, int numarea, int numage, int numlen) {

  ErrorHandler handle;
  int i;
  int year, step;
  double tmpnumber;
  char tmparea[MaxStrLength], tmpage[MaxStrLength], tmplen[MaxStrLength];
  strncpy(tmparea, "", MaxStrLength);
  strncpy(tmpage, "", MaxStrLength);
  strncpy(tmplen, "", MaxStrLength);
  int keepdata, timeid, ageid, areaid, lenid;

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
  if (!(CheckColumns(infile, 6)))
    handle.Message("Wrong number of columns in inputfile - should be 6");

  while (!infile.eof()) {
    keepdata = 0;
    infile >> year >> step >> tmparea >> tmpage >> tmplen >> tmpnumber >> ws;

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
          AgeLengthData[timeid][i] = new doublematrix(numage, numlen, 0.0);
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

    //if tmplen is in lenindex find lenid, else dont keep the data
    lenid = -1;
    for (i = 0; i < lenindex.Size(); i++)
      if (strcasecmp(lenindex[i], tmplen) == 0)
        lenid = i;

    if (lenid == -1)
      keepdata = 1;

    if (keepdata == 0) {
      //log catch data is required, so store it
      (*AgeLengthData[timeid][areaid])[ageid][lenid] = tmpnumber;
    }
  }
  AAT.AddActions(Years, Steps, TimeInfo);
}

void LogCatches::ReadLogWeightsData(CommentStream& infile,
  const TimeClass* TimeInfo, int numlen) {

  ErrorHandler handle;
  int i;
  int year, step;
  double tmpnumber;
  int keepdata, timeid, lenid;
  char tmplen[MaxStrLength];
  strncpy(tmplen, "", MaxStrLength);

  //JMB - surely weight should be area dependant as well???
  //We already know the size that weights[][] will be
  weights.AddRows(Years.Size(), numlen);

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
  if (!(CheckColumns(infile, 4)))
    handle.Message("Wrong number of columns in inputfile - should be 4");

  while (!infile.eof()) {
    keepdata = 0;
    infile >> year >> step >> tmplen >> tmpnumber >> ws;

    //check if the year and step are in the simulation
    timeid = -1;
    if (TimeInfo->IsWithinPeriod(year, step))
      //find the timeid from Years and Steps
      for (i = 0; i < Years.Size(); i++)
        if ((Years[i] == year) && (Steps[i] == step))
          timeid = i;

    if (timeid == -1)
      keepdata = 1;

    //if tmplen is in lenindex find lenid, else dont keep the data
    lenid = -1;
    for (i = 0; i < lenindex.Size(); i++)
      if (strcasecmp(lenindex[i], tmplen) == 0)
        lenid = i;

    if (lenid == -1)
      keepdata = 1;

    if (keepdata == 0) {
      //weight data is required, so store it
      //JMB - but surely this should be weights[timeid][areaid][lenid]??
      weights[timeid][lenid] = tmpnumber;
    }
  }
}

LogCatches::~LogCatches() {
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
    for (j = 0; j < AgeLengthData.Ncol(i); j++) {
      delete AgeLengthData[i][j];
      delete Proportions[i][j];  //kgf 23/9 98
    }
  delete aggregator;
}

void LogCatches::Reset(const Keeper* const keeper) {
  Likelihood::Reset(keeper);
  timeindex = 0;
}

void LogCatches::Print(ofstream& outfile) const {
}

void LogCatches::LikelihoodPrint(ofstream& outfile) const {
  int i, j, a, y;

  outfile << "\nLogCatch Likelihood\n\tlikelihood " << likelihood
    << "\n\tfunction number " << functionnumber << endl << TAB;
  Likelihood::LikelihoodPrint(outfile);
  outfile << "\tStocknames: " << sep;
  for (i = 0; i < stocknames.Size(); i++)
    outfile << stocknames[i] << sep;
  outfile << "\nAreas : ";
  for (i = 0; i < areas.Nrow(); i++) {
    for (j = 0; j < areas.Ncol(i); j++)
      outfile << areas[i][j] << sep;
    outfile << "\n\t";
  }
  outfile << "\nAges : ";
  for (i = 0; i < ages.Nrow(); i++) {
    for (j = 0; j < ages.Ncol(i); j++)
      outfile << ages[i][j] << sep;
    outfile << "\n\t";
  }
  outfile << "\nLengths : ";
  for (i = 0; i < lengths.Size(); i++)
    outfile << lengths[i] << sep;
  outfile << "\nFleetnames: " << sep;
  for (i = 0; i < fleetnames.Size(); i++)
    outfile << fleetnames[i] << sep;
  outfile << endl;

  //aggregator->Print(outfile);
  outfile << "\tAge-Length Distribution Data:\n";
  for (y = 0; y < AgeLengthData.Nrow(); y++) {
    outfile << "\nyear and step " << Years[y] << sep << Steps[y] << endl;
    for (a = 0; a < AgeLengthData.Ncol(y); a++) {
      outfile << "\nArea : " << a << "\nmeasurements\n";
      for (i = 0; i < AgeLengthData[y][a]->Nrow(); i++) {
        for (j = 0; j < AgeLengthData[y][a]->Ncol(i); j++) {
          outfile.width(printwidth);
          outfile.precision(smallprecision);
          outfile << (*AgeLengthData[y][a])[i][j] << sep;
        }
        outfile << endl;
      }
      outfile << "\nNumber caught according to model\n";
      for (i = 0; i < Proportions[y][a]->Nrow(); i++) {
        for (j = 0; j < Proportions[y][a]->Ncol(i); j++) {
          outfile.width(printwidth);
          outfile.precision(smallprecision);
          outfile << (*Proportions[y][a])[i][j] << sep;
        }
        outfile << endl;
      }
    }
    outfile << "Likelihood values : ";
    for (a = 0; a < AgeLengthData.Ncol(y); a++) {
       outfile.width(printwidth);
       outfile.precision(smallprecision);
       outfile << Likelihoodvalues[y][a] << sep;
    }
    outfile << endl;
  }
}

void LogCatches::SetFleetsAndStocks(Fleetptrvector& Fleets, Stockptrvector& Stocks) {
  int i, j;
  int found = 0;
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
      cerr << "Error: when searching for names of fleets for Catchdistribtuion.\n"
        << "Did not find any name matching " << fleetnames[i] << endl;
      exit(EXIT_FAILURE);
    }
  }

  min_stock_age = 100;
  max_stock_age = 0;
  for (i = 0; i < stocknames.Size(); i++) {
    found = 0;
    for (j = 0; j < Stocks.Size(); j++) {
      if (Stocks[j]->IsEaten()) {
        if (strcasecmp(stocknames[i], Stocks[j]->ReturnPrey()->Name()) == 0) {
          found = 1;
          stocks.resize(1, Stocks[j]);
        }
        if (stocks[stocks.Size() - 1]->Minage() < min_stock_age) //kgf 10/5 99
          min_stock_age = stocks[stocks.Size() - 1]->Minage();
        if (stocks[stocks.Size() - 1]->Maxage() > max_stock_age)
          max_stock_age = stocks[stocks.Size() - 1]->Maxage();
      }
    }
    if (found == 0) {
      cerr << "Error: when searching for names of stocks for Catchdistribution.\n"
        << "Did not find any name matching " << stocknames[i] << endl;
      exit(EXIT_FAILURE);
    }
  }
  LengthGroupDivision* LgrpDiv = new LengthGroupDivision(lengths);
  aggregator = new FleetPreyAggregator(fleets, stocks, LgrpDiv, areas, ages, overconsumption);
}

void LogCatches::AddToLikelihood(const TimeClass* const TimeInfo) {
  int dummy = 1;
  if (AAT.AtCurrentTime(TimeInfo)) {
    switch(functionnumber) {
      case 2:
      case 3:
        aggregator->Sum(TimeInfo, dummy); //mortality model, calculated catch
        likelihood += LogLik(TimeInfo);
        break;
      default:
        cerr << "Function number is not correct, must be 2 or 3\n";
        break;
    }
    timeindex++;
  }
}

double LogCatches::LogLik(const TimeClass* const TimeInfo) {
  //written by kgf 23/11 98 to get a better scaling of the stocks.
  //modified by kgf 27/11 98 to sum first and then take the logarithm

  double totallikelihood = 0.0;
  int nareas, age, length;
  double calc_c_sum = 0.0;
  double obs_c_sum = 0.0;
  double step_val = 0.0;
  int min_age = 0;
  int max_age = 0;

  //Get numbers from aggregator->AgeLengthDist()
  const Agebandmatrixvector* alptr = &aggregator->AgeLengthDist();
  for (nareas = 0; nareas < (*alptr).Size(); nareas++) {
    min_age = max((*alptr)[nareas].Minage(), min_stock_age - 1);
    max_age = min((*alptr)[nareas].Maxage() + 1, max_stock_age);
    Likelihoodvalues[timeindex][nareas] = 0.0;
    calc_c_sum = 0.0;
    obs_c_sum = 0.0;
    if (TimeInfo->CurrentStep() == 1) { //start of a new year
      (*calc_c[nareas]).setElementsTo(0.0); //values on year basis
      (*obs_c[nareas]).setElementsTo(0.0); //values on year basis
    }

    //for (age = (*alptr)[nareas].Minage(); age <= (*alptr)[nareas].Maxage(); age++) {
    for (age = min_age; age < max_age; age++) { //kgf 10/5 99
      for (length = (*alptr)[nareas].Minlength(age);
          length < (*alptr)[nareas].Maxlength(age); length++) {
        (*Proportions[timeindex][nareas])[age][length] =
          (*alptr)[nareas][age][length].N;
        (*calc_c[nareas])[age][length] +=
          (*Proportions[timeindex][nareas])[age][length];
        (*obs_c[nareas])[age][length] +=
          (*AgeLengthData[timeindex][nareas])[age][length];
      }
    }

    if (agg_lev == 0) { //calculate likelihood on all steps
      for (age = min_age; age < max_age; age++) {
        for (length = (*alptr)[nareas].Minlength(age);
            length < (*alptr)[nareas].Maxlength(age); length++) {
          calc_c_sum += (*Proportions[timeindex][nareas])[age][length];
          obs_c_sum += (*AgeLengthData[timeindex][nareas])[age][length];
        }
      }
      assert((calc_c_sum > minp) && (obs_c_sum > minp));
      step_val = log(obs_c_sum / calc_c_sum);
      Likelihoodvalues[timeindex][nareas] += (step_val * step_val);

    } else { //calculate  likelihood on year basis
      if (TimeInfo->CurrentStep() < TimeInfo->StepsInYear())
        Likelihoodvalues[timeindex][nareas] = 0.0;
      else { //last step in year, is to calculate likelihood contribution
        for (age = min_age; age < max_age; age++) {
          for (length = (*alptr)[nareas].Minlength(age);
              length < (*alptr)[nareas].Maxlength(age); length++) {
            calc_c_sum += (*calc_c[nareas])[age][length];
            obs_c_sum +=  (*obs_c[nareas])[age][length];
          }
        }
        assert(calc_c_sum > minp && obs_c_sum > minp);
        step_val = log(obs_c_sum / calc_c_sum);
        Likelihoodvalues[timeindex][nareas] += (step_val * step_val);
      }
    }
    totallikelihood += Likelihoodvalues[timeindex][nareas];

  }
  return totallikelihood;
}

void LogCatches::PrintLikelihoodOnStep(ofstream& catchfile,
  const TimeClass& TimeInfo, int print_type) {

  //written by kgf 26/11 98 to make it possible to print residuals
  //on the time steps the likelihood term is calculated. The calculated
  //values are supposed to be generated before a call to this print function.

  catchfile.setf(ios::fixed);
  int nareas, age, length;
  double step_val = 0.0;
  int min_age = 0;
  int max_age = 0;

  //Get age and length intervals from aggregator->AgeLengthDist()
  const Agebandmatrixvector* alptr = &aggregator->AgeLengthDist();
  for (nareas = 0; nareas < (*alptr).Size(); nareas++) {
    min_age = max((*alptr)[nareas].Minage(), min_stock_age - 1);
    max_age = min((*alptr)[nareas].Maxage() + 1, max_stock_age);
    if ((TimeInfo.CurrentStep() == 1) && (agg_lev == 1)) { //start of a new year
      (*calc_c[nareas]).setElementsTo(0.0);
      (*obs_c[nareas]).setElementsTo(0.0);
    }
    for (age = min_age; age < max_age; age++) {
      for (length = (*alptr)[nareas].Minlength(age);
          length < (*alptr)[nareas].Maxlength(age); length++) {
        (*calc_c[nareas])[age][length] +=      //sum up to year
          (*Proportions[timeindex][nareas])[age][length];
        (*obs_c[nareas])[age][length] +=       //sum up to year
          (*AgeLengthData[timeindex][nareas])[age][length];
      }
    }

    if (agg_lev == 0) { //calculate likelihood on all steps
      if (print_type == 0) { //print log(C/C_hat)
        catchfile << "year " << TimeInfo.CurrentYear() << " step "
          << TimeInfo.CurrentStep() << "\nlog(C/C_hat) by age and length\n";
        for (age = min_age; age < max_age; age++) {
          for (length = (*alptr)[nareas].Minlength(age);
              length < (*alptr)[nareas].Maxlength(age); length++) {
            if (((*Proportions[timeindex][nareas])[age][length] > minp)
                && ((*AgeLengthData[timeindex][nareas])[age][length] > minp))
              step_val = log((*AgeLengthData[timeindex][nareas])[age][length] /
                (*Proportions[timeindex][nareas])[age][length]);
            else
              step_val = -9999;

            catchfile.precision(smallprecision);
            catchfile.width(printwidth);
            if (step_val != -9999)
              catchfile << step_val << sep;
            else
              catchfile << "     _ ";
          }
          catchfile << endl;
        }

      } else if (print_type == 2) { //print log(Sum(C)/Sum(C_hat))^2
        obsCSum = 0;
        calcCSum = 0;
        catchfile << "year " << TimeInfo.CurrentYear() << " step "
          << TimeInfo.CurrentStep() << "\nlog(Sum(C)/Sum(C_hat))^2\n";
        step_val = Likelihoodvalues[timeindex][nareas];
        catchfile.precision(lowprecision);
        catchfile.width(largewidth);
        if (step_val != -9999)
          catchfile << step_val << sep;
        else
          catchfile << "         _ ";
        catchfile << endl;

      } else { //write C and C_hat seperately
        catchfile << "year " << TimeInfo.CurrentYear() << " step "
          << TimeInfo.CurrentStep() << "\nC by age and length\n";
        for (age = min_age; age < max_age; age++) {
          for (length = (*alptr)[nareas].Minlength(age);
              length < (*alptr)[nareas].Maxlength(age); length++) {
            step_val = (*AgeLengthData[timeindex][nareas])[age][length];
            catchfile.precision(lowprecision);
            catchfile.width(printwidth);
            if (step_val != -9999)
              catchfile << step_val << sep;
            else
              catchfile << "      _ ";
          }
          catchfile << endl;
        }
        catchfile << "year " << TimeInfo.CurrentYear() << " step "
          << TimeInfo.CurrentStep() << "\nC_hat by age and length\n";
        for (age = min_age; age < max_age; age++) {
          for (length = (*alptr)[nareas].Minlength(age);
              length < (*alptr)[nareas].Maxlength(age); length++) {
            step_val = (*Proportions[timeindex][nareas])[age][length];
            catchfile.precision(lowprecision);
            catchfile.width(printwidth);
            if (step_val != -9999)
              catchfile << step_val << sep;
            else
              catchfile << "      _ ";
          }
          catchfile << endl;
        }
      }
      catchfile.flush();

    } else { //calculate likelihood on year basis
      if (TimeInfo.CurrentStep() == TimeInfo.StepsInYear()) {
        if (print_type == 0) { //print log(C/C_hat)
          catchfile << "year " << TimeInfo.CurrentYear() << " step "
            << TimeInfo.CurrentStep() << "\nlog(C/C_hat) by age and length\n";
          for (age = min_age; age < max_age; age++) {
            for (length = (*alptr)[nareas].Minlength(age);
                length < (*alptr)[nareas].Maxlength(age); length++) {
              if (((*obs_c[nareas])[age][length] > minp)
                  && ((*calc_c[nareas])[age][length] > minp))
                step_val = log((*obs_c[nareas])[age][length] /
                  (*calc_c[nareas])[age][length]);
              else
                step_val = -9999;
              catchfile.precision(smallprecision);
              catchfile.width(printwidth);
              if (step_val != -9999)
                catchfile << step_val << sep;
              else
                catchfile << "     _ ";
            }
            catchfile << endl;
          }
        } else if (print_type == 2) { //print (C_hat-C)^2/C_hat
          catchfile << "year " << TimeInfo.CurrentYear() << " step "
            << TimeInfo.CurrentStep() << "\nlog(Sum(C)/Sum(C_hat))^2\n";
          step_val = Likelihoodvalues[timeindex][nareas];
          catchfile.precision(lowprecision);
          catchfile.width(largewidth);
          if (step_val != -9999)
            catchfile << step_val << sep;
          else
            catchfile << "         _ ";
          catchfile << endl;

        } else { //write C and C_hat seperately
          catchfile << "year " << TimeInfo.CurrentYear() << " step "
            << TimeInfo.CurrentStep() << "\nC by age and length\n";
          for (age = min_age; age < max_age; age++) {
            for (length = (*alptr)[nareas].Minlength(age);
                length < (*alptr)[nareas].Maxlength(age); length++) {
              step_val = (*obs_c[nareas])[age][length];
              catchfile.precision(lowprecision);
              catchfile.width(printwidth);
              if (step_val != -9999)
                catchfile << step_val << sep;
              else
                catchfile << "      _ ";
            }
            catchfile << endl;
          }
          catchfile << "year " << TimeInfo.CurrentYear() << " step "
            << TimeInfo.CurrentStep() << "\nC_hat by age and length\n";
          for (age = min_age; age < max_age; age++) {
            for (length = (*alptr)[nareas].Minlength(age);
                length < (*alptr)[nareas].Maxlength(age); length++) {
              step_val = (*calc_c[nareas])[age][length];
              catchfile.precision(lowprecision);
              catchfile.width(printwidth);
              if (step_val != -9999)
                catchfile << step_val << sep;
              else
                catchfile << "      _ ";
            }
            catchfile << endl;
          }
        } //end print_type
      }
      catchfile.flush();
    } //end last step in year
  } //end nareas
  //timeindex++;
}

void LogCatches::printHeader(ofstream& catchfile, const PrintInfo& print) {
  timeindex = 0;
  int i;
  if (print.catchprint) {
    catchfile << "\nfleets";
    for (i = 0; i<fleetnames.Size(); i++)
      catchfile << sep << fleetnames[i];
    catchfile << "\nstocks";
    for (i = 0; i<stocknames.Size(); i++)
      catchfile << sep << stocknames[i];

    catchfile << "\n\nminage " << MinAge << " maxage " << MaxAge
      << " minlen " << lengths[0] << " maxlen " << lengths[lengths.Size() - 1]
      << " lenstep " << dl << "\n (as this likelihood calculation sums the\n"
      << " values over length and age, it is not printed by length and age)\n\n";
  }
}

void LogCatches::print(ofstream& catchfile, const TimeClass& time, const PrintInfo& print) {
  if (!AAT.AtCurrentTime(&time))
    return;
  else if (print.catchprint) {
    PrintLikelihoodOnStep(catchfile, time, print.catchprint - 1);
    timeindex++;
  }
}
