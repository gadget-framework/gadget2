#include "catchdistribution.h"
#include "readfunc.h"
#include "readword.h"
#include "readaggregation.h"
#include "errorhandler.h"
#include "areatime.h"
#include "fleet.h"
#include "stock.h"
#include "multinomial.h"
#include "stockprey.h"
#include "ecosystem.h"
#include "gadget.h"

CatchDistribution::CatchDistribution(CommentStream& infile,
  const AreaClass* const Area, const TimeClass* const TimeInfo,
  double likweight, const char* name)
  : Likelihood(CATCHDISTRIBUTIONLIKELIHOOD, likweight) {

  ErrorHandler handle;
  int i, j;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  steps = TimeInfo->StepsInYear();
  first = TimeInfo->FirstStep();
  int numarea = 0, numage = 0, numlen = 0;

  char datafilename[MaxStrLength];
  char aggfilename[MaxStrLength];
  strncpy(datafilename, "", MaxStrLength);
  strncpy(aggfilename, "", MaxStrLength);
  ifstream datafile;
  CommentStream subdata(datafile);

  timeindex = 0;
  cdname = new char[strlen(name) + 1];
  strcpy(cdname, name);
  functionname = new char[MaxStrLength];
  strncpy(functionname, "", MaxStrLength);

  readWordAndValue(infile, "datafile", datafilename);
  readWordAndValue(infile, "function", functionname);

  functionnumber = 0;
  if (strcasecmp(functionname, "multinomial") == 0)
    functionnumber = 1;
  else if (strcasecmp(functionname, "pearson") == 0)
    functionnumber = 2;
  else if (strcasecmp(functionname, "modmultinomial") == 0)
    functionnumber = 3;
  else if (strcasecmp(functionname, "experimental") == 0)
    functionnumber = 4;
  else if (strcasecmp(functionname, "gamma") == 0)
    functionnumber = 5;
  else if (strcasecmp(functionname, "sumofsquares") == 0)
    functionnumber = 6;
  else
    handle.Message("Error in catchdistribution - unrecognised function", functionname);

  infile >> ws;
  char c = infile.peek();
  if ((c == 'a') || (c == 'A')) {
    infile >> text >> ws;
    //JMB - changed to check for aggregation_level or aggregationlevel
    if ((strcasecmp(text, "aggregation_level") == 0) || (strcasecmp(text, "aggregationlevel") == 0))
      infile >> agg_lev >> ws;
    else
      handle.Unexpected("aggregationlevel", text);

  } else
    agg_lev = 0; //default value for agg_lev

  if (agg_lev != 0 && agg_lev != 1)
    handle.Message("Error in catchdistribution - aggregationlevel must be 0 or 1");

  readWordAndVariable(infile, "overconsumption", overconsumption);
  if (overconsumption != 0 && overconsumption != 1)
    handle.Message("Error in catchdistribution - overconsumption must be 0 or 1");

  //JMB - changed to make the reading of minimum probability optional
  infile >> ws;
  c = infile.peek();
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

  LgrpDiv = new LengthGroupDivision(lengths);

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
  ReadDistributionData(subdata, TimeInfo, numarea, numage, numlen);
  handle.Close();
  datafile.close();
  datafile.clear();

  int numrow, numcol;
  numrow = AgeLengthData.Nrow();
  numcol = AgeLengthData.Ncol();
  Likelihoodvalues.AddRows(numrow, numcol, 0.0);
  calc_c.resize(numcol);
  obs_c.resize(numcol);
  for (i = 0; i < numcol; i++) {
    calc_c[i] = new DoubleMatrix(AgeLengthData[0][i]->Nrow(), AgeLengthData[0][i]->Ncol(), 0.0);
    obs_c[i] = new DoubleMatrix(AgeLengthData[0][i]->Nrow(), AgeLengthData[0][i]->Ncol(), 0.0);
  }

  obs_biomass.resize(numcol);
  calc_biomass.resize(numcol);
  agg_obs_biomass.resize(numcol);
  agg_calc_biomass.resize(numcol);
  calc_catch.resize(numcol);
  obs_catch.resize(numcol);

  int y, year;
  for (i = 0; i < numcol; i++) {
    obs_biomass[i] = new DoubleMatrix(numrow, AgeLengthData[0][i]->Nrow(), 0.0);
    calc_biomass[i] = new DoubleMatrix(numrow, AgeLengthData[0][i]->Nrow(), 0.0);
    if (agg_lev) {
      //Find number of years.
      y = 1;
      year = Years[0];
      for (j = 1; j < Years.Size(); j++)
        if (Years[j] != year) {
          y++;
          year = Years[j];
        }

      agg_obs_biomass[i] = new DoubleMatrix(y, AgeLengthData[0][i]->Nrow(), 0.0);
      agg_calc_biomass[i] = new DoubleMatrix(y, AgeLengthData[0][i]->Nrow(), 0.0);
      calc_catch[i] = new DoubleMatrix(y, AgeLengthData[0][i]->Nrow(), 0.0);
      obs_catch[i] = new DoubleMatrix(y, AgeLengthData[0][i]->Nrow(), 0.0);

    } else {
      calc_catch[i] = new DoubleMatrix(numrow, AgeLengthData[0][i]->Nrow(), 0.0);
      obs_catch[i] = new DoubleMatrix(numrow, AgeLengthData[0][i]->Nrow(), 0.0);
    }
  }
}

void CatchDistribution::ReadDistributionData(CommentStream& infile,
  const TimeClass* TimeInfo, int numarea, int numage, int numlen) {

  int i;
  int year, step;
  double tmpnumber;
  char tmparea[MaxStrLength], tmpage[MaxStrLength], tmplen[MaxStrLength];
  strncpy(tmparea, "", MaxStrLength);
  strncpy(tmpage, "", MaxStrLength);
  strncpy(tmplen, "", MaxStrLength);
  int keepdata, timeid, ageid, areaid, lenid;
  int count = 0;
  ErrorHandler handle;

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
  if (countColumns(infile) != 6)
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
        Proportions.AddRows(1, numarea);
        for (i = 0; i < numarea; i++) {
          AgeLengthData[timeid][i] = new DoubleMatrix(numage, numlen, 0.0);
          Proportions[timeid][i] = new DoubleMatrix(numage, numlen, 0.0);
        }
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
      //distribution data is required, so store it
      count++;
      (*AgeLengthData[timeid][areaid])[ageid][lenid] = tmpnumber;
    }
  }
  AAT.AddActions(Years, Steps, TimeInfo);
  if (count == 0)
    cerr << "Warning in catchdistribution - found no data in the data file for " << cdname << endl;
}

void CatchDistribution::aggregateBiomass() {
  assert(agg_lev == 1);
  int aggt = -1;
  int a, t, ar;
  for (ar = 0; ar < areas.Nrow(); ar++)
    for (t = first; t < obs_biomass[ar]->Nrow() + first; t++) {
      if ((t - 1) % steps == 0) {
        aggt++;
        for (a = minrow; a <= maxrow; a++) {
          (*agg_obs_biomass[ar])[aggt][a] = 0.0;
          (*agg_calc_biomass[ar])[aggt][a] = 0.0;
        }
      }
      for (a = minrow; a <= maxrow; a++) {
        (*agg_obs_biomass[ar])[aggt][a] += (*obs_biomass[ar])[t - first][a];
        (*agg_calc_biomass[ar])[aggt][a] += (*calc_biomass[ar])[t - first][a];
      }
    }
}

void CatchDistribution::calcBiomass(int t) {
  //modified by JMB to remove the weights[][] component

  int ar, a, l;
  for (ar = 0; ar < areas.Nrow(); ar++)
    for (a = minrow; a <= maxrow; a++) {
      (*calc_biomass[ar])[t][a] = 0.0;
      (*obs_biomass[ar])[t][a] = 0.0;
      for (l = mincol[a]; l <= maxcol[a]; l++) {
        (*calc_biomass[ar])[t][a] += (*Proportions[t][ar])[a][l];
        (*obs_biomass[ar])[t][a] += (*AgeLengthData[t][ar])[a][l];
      }
    }

  int time = t;
  if (agg_lev)
    time = (t + first - 1) / steps;

  for (ar = 0; ar < areas.Nrow(); ar++) {
    if (!agg_lev || (agg_lev && ((t + first - 1) % steps == 0 || t == 0)))
      for (a = minrow; a <= maxrow; a++) {
        (*calc_catch[ar])[time][a] = 0.0;
        (*obs_catch[ar])[time][a] = 0.0;
      }

    for (a = minrow; a <= maxrow; a++)
      for (l = mincol[a]; l <= maxcol[a]; l++) {
        (*calc_catch[ar])[time][a] += (*Proportions[t][ar])[a][l];
        (*obs_catch[ar])[time][a] += (*AgeLengthData[t][ar])[a][l];
      }
  }
  /*JMB code removed from here - see RemovedCode.txt for details*/
}

CatchDistribution::~CatchDistribution() {
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
  for (i = 0; i < calc_c.Size(); i++) {
    delete calc_c[i];
    delete obs_c[i];
  }
  for (i = 0; i < calc_biomass.Size(); i++) {
    delete calc_biomass[i];
    delete obs_biomass[i];
    delete calc_catch[i];
    delete obs_catch[i];
    if (agg_lev) {
      delete agg_calc_biomass[i];
      delete agg_obs_biomass[i];
    }
  }
  delete aggregator;
  delete LgrpDiv;
  delete[] cdname;
  delete[] functionname;
}

void CatchDistribution::Reset(const Keeper* const keeper) {
  Likelihood::Reset(keeper);
  timeindex = 0;
}

void CatchDistribution::Print(ofstream& outfile) const {
  int i;

  outfile << "\nCatch distribution " << cdname << "\nlikelihood " << likelihood
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

void CatchDistribution::LikelihoodPrint(ofstream& outfile) {
  int i, j, y, a;

  outfile << "\nCatch Distribution\n\nLikelihood " << likelihood << "\nFunction "
    << functionname << "\nWeight " << weight << "\nStock names:";
  for (i = 0; i < stocknames.Size(); i++)
    outfile << sep << stocknames[i];
  outfile << "\nAreas:";
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

  //aggregator->Print(outfile);
  outfile << "Age-Length distribution data:";
  for (y = 0; y < AgeLengthData.Nrow(); y++) {
    outfile << "\nYear " << Years[y] << " and step " << Steps[y];
    for (a = 0; a < AgeLengthData.Ncol(y); a++) {
      outfile << "\nArea: " << a << "\nMeasurements";
      for (i = 0; i < AgeLengthData[y][a]->Nrow(); i++) {
        outfile << endl;
        for (j = 0; j < AgeLengthData[y][a]->Ncol(i); j++) {
          outfile.width(printwidth);
          outfile.precision(lowprecision);
          outfile << sep << (*AgeLengthData[y][a])[i][j];
        }
      }
      outfile << "\nNumber caught according to model";
      for (i = 0; i < Proportions[y][a]->Nrow(); i++) {
        outfile << endl;
        for (j = 0; j < Proportions[y][a]->Ncol(i); j++) {
          outfile.width(printwidth);
          outfile.precision(smallprecision);
          outfile << sep << (*Proportions[y][a])[i][j];
        }
      }
    }
    outfile << "\nLikelihood values:";
    for (a = 0; a < AgeLengthData.Ncol(y); a++) {
       outfile.width(printwidth);
       outfile.precision(smallprecision);
       outfile << sep << Likelihoodvalues[y][a];
    }
    outfile << endl;
  }
  outfile.flush();
}

void CatchDistribution::SetFleetsAndStocks(FleetPtrVector& Fleets, StockPtrVector& Stocks) {
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
      cerr << "Error: when searching for names of fleets for catchdistribution.\n"
        << "Did not find any name matching " << fleetnames[i] << endl;
      exit(EXIT_FAILURE);
    }
  }

  min_stock_age = 100;
  max_stock_age = 0;
  for (i = 0; i < stocknames.Size(); i++) {
    found = 0;
    for (j = 0; j < Stocks.Size(); j++) {
      if (Stocks[j]->IsEaten())
        if (strcasecmp(stocknames[i], Stocks[j]->ReturnPrey()->Name()) == 0) {
          found = 1;
          stocks.resize(1, Stocks[j]);
          if (stocks[stocks.Size() - 1]->Minage() < min_stock_age) //kgf 3/5 99
            min_stock_age = stocks[stocks.Size() - 1]->Minage();
          if (stocks[stocks.Size() - 1]->Maxage() > max_stock_age)
            max_stock_age = stocks[stocks.Size() - 1]->Maxage();
        }
    }
    if (found == 0) {
      cerr << "Error: when searching for names of stocks for catchdistribution.\n"
        << "Did not find any name matching " << stocknames[i] << endl;
      exit(EXIT_FAILURE);
    }
  }
  aggregator = new FleetPreyAggregator(fleets, stocks, LgrpDiv, areas, ages, overconsumption);

  //Limits (inclusive) for traversing the matrices where required
  if ((functionnumber != 1) && (functionnumber != 6)) {
    mincol = aggregator->getMinCol();
    maxcol = aggregator->getMaxCol();
    minrow = aggregator->getMinRow();
    maxrow = aggregator->getMaxRow();
  }
}

void CatchDistribution::AddToLikelihood(const TimeClass* const TimeInfo) {

  if (AAT.AtCurrentTime(TimeInfo)) {
    switch(functionnumber) {
      case 1:
        aggregator->Sum(TimeInfo);
        likelihood += LikMultinomial();
        break;
      case 2:
        aggregator->Sum(TimeInfo, 1); //mortality model, calculated catch
        likelihood += LikPearson(TimeInfo);
        break;
      case 3:
        aggregator->Sum(TimeInfo, 1); //mortality model, calculated catch
        likelihood += ModifiedMultinomial(TimeInfo);
        break;
      case 4:
        aggregator->Sum(TimeInfo, 1); //mortality model, calculated catch
        likelihood += ExperimentalLik(TimeInfo);
        break;
      case 5:
        aggregator->Sum(TimeInfo, 1); //mortality model, calculated catch
        likelihood += GammaLik(TimeInfo);
        break;
      case 6:
        aggregator->Sum(TimeInfo);
        likelihood += LikSumSquares();
        break;
      default:
        cerr << "Error in catchdistribution - unknown function " << functionname << endl;
        break;
    }
    //JMB if (weights.Nrow() > 0)
    //  calcBiomass(timeindex);
    timeindex++;
  }
}

double CatchDistribution::LikMultinomial() {
  int a, age, len, nareas;

  //Get numbers from aggregator->AgeLengthDist()
  const AgeBandMatrixPtrVector* alptr = &aggregator->AgeLengthDist();
  DoubleMatrixPtrVector Dist(alptr->Size(), NULL);
  for (nareas = 0; nareas < areas.Nrow(); nareas++) {
    Dist[nareas] = new DoubleMatrix(aggregator->NoAgeGroups(), aggregator->NoLengthGroups(), 0.0);

    for (age = (*alptr)[nareas].Minage(); age <= (*alptr)[nareas].Maxage(); age++)
      for (len = (*alptr)[nareas].Minlength(age); len < (*alptr)[nareas].Maxlength(age); len++)
        (*Dist[nareas])[age][len] = ((*alptr)[nareas][age][len]).N;
  }

  //The object MN does most of the work, accumulating likelihood
  Multinomial MN(epsilon);
  for (nareas = 0; nareas < Dist.Size(); nareas++) {
    if (AgeLengthData[timeindex][nareas]->Nrow() == 1) {
      //If there is only one agegroup, we calculate loglikelihood
      //based on the length group division
      MN.CalcLogLikelihood((*AgeLengthData[timeindex][nareas])[0], (*Dist[nareas])[0]);
      for (len = 0; len < AgeLengthData[timeindex][nareas]->Ncol(0); len++)
        (*Proportions[timeindex][nareas])[0][len] = (*Dist[nareas])[0][len];

    } else {
      //Calculate loglikelihood based on age distribution within each length
      //group. To be able to use the Multinomial class, we must organize the
      //data such that we get vectors with the age group distribution.
      for (len = 0; len < Dist[nareas]->Ncol(0); len++) {
        DoubleVector dist(Dist[nareas]->Nrow());
        DoubleVector data(Dist[nareas]->Nrow());
        for (a = 0; a < Dist[nareas]->Nrow(); a++) {
          dist[a] = (*Dist[nareas])[a][len];
          data[a] = (*AgeLengthData[timeindex][nareas])[a][len];
        }
        MN.CalcLogLikelihood(data, dist);
        for (a = 0; a < dist.Size(); a++)
          (*Proportions[timeindex][nareas])[a][len] = dist[a];
      }
    }
    delete Dist[nareas];
  }
  return MN.ReturnLogLikelihood();
}

double CatchDistribution::LikPearson(const TimeClass* const TimeInfo) {
  /* written by Hoskuldur Bjornsson 29/8 98
   * corrected by kgf 16/9 98
   * modified by kgf 11/11 98 to make it possible to sum up catches
   * and calculated catches on year basis.
   * Modified 3/5 99 by kgf to check the age intervals of the stock
   * and the catch data, and make use of the ages that are common
   * for the stock and the catch data.*/

  double totallikelihood = 0.0;
  int age, length, min_age, max_age, nareas;

  //Get numbers from aggregator->AgeLengthDist()
  const AgeBandMatrixPtrVector* alptr = &aggregator->AgeLengthDist();
  for (nareas = 0; nareas < (*alptr).Size(); nareas++) {
    min_age = max((*alptr)[nareas].Minage(), min_stock_age - 1);
    max_age = min((*alptr)[nareas].Maxage() + 1, max_stock_age);
    Likelihoodvalues[timeindex][nareas] = 0.0;
    if (TimeInfo->CurrentStep() == 1) { //start of a new year
      (*calc_c[nareas]).setElementsTo(0.0);
      (*obs_c[nareas]).setElementsTo(0.0);
    }

    for (age = minrow; age <= maxrow; age++) { //test, mnaa
      for (length = mincol[age]; length <= maxcol[age]; length++) { //test, mnaa
        (*Proportions[timeindex][nareas])[age][length] =
          (*alptr)[nareas][age][length].N;
        (*calc_c[nareas])[age][length] +=
          (*Proportions[timeindex][nareas])[age][length];
        (*obs_c[nareas])[age][length] +=
          (*AgeLengthData[timeindex][nareas])[age][length];
      }
    }

    if (agg_lev == 0) { //calculate likelihood on all steps
      for (age = minrow; age <= maxrow; age++) { //test, mnaa
        for (length = mincol[age]; length <= maxcol[age]; length++) { //test, mnaa
          Likelihoodvalues[timeindex][nareas] +=
            ((*Proportions[timeindex][nareas])[age][length] -
            (*AgeLengthData[timeindex][nareas])[age][length]) *
            ((*Proportions[timeindex][nareas])[age][length] -
            (*AgeLengthData[timeindex][nareas])[age][length]) /
            absolute(((*Proportions[timeindex][nareas])[age][length] + epsilon));
        }
      }
      totallikelihood += Likelihoodvalues[timeindex][nareas];

    } else { //calculate likelihood on year basis
      if (TimeInfo->CurrentStep() < TimeInfo->StepsInYear())
        Likelihoodvalues[timeindex][nareas] = 0.0;
      else { //last step in year, is to calc likelihood contribution
        for (age = minrow; age <= maxrow; age++) { //test, mnaa
          for (length = mincol[age]; length <= maxcol[age]; length++) { //test, mnaa
            Likelihoodvalues[timeindex][nareas] +=
              ((*calc_c[nareas])[age][length] - (*obs_c[nareas])[age][length]) *
              ((*calc_c[nareas])[age][length] - (*obs_c[nareas])[age][length]) /
              absolute(((*calc_c[nareas])[age][length] + epsilon));
          }
        }
        totallikelihood += Likelihoodvalues[timeindex][nareas];
      }
    }
  } //end nareas
  return totallikelihood;
}

double CatchDistribution::GammaLik(const TimeClass* const TimeInfo) {
  //written kgf 24/5 00
  //Formula by Hans J Skaug, 15/3 00 No weighting at present.
  //This function is scale independent.

  double totallikelihood = 0.0;
  int age, length, min_age, max_age, nareas;

  //Get numbers from aggregator->AgeLengthDist()
  const AgeBandMatrixPtrVector* alptr = &aggregator->AgeLengthDist();
  for (nareas = 0; nareas < (*alptr).Size(); nareas++) {
    min_age = max((*alptr)[nareas].Minage(), min_stock_age - 1);
    max_age = min((*alptr)[nareas].Maxage() + 1, max_stock_age);
    Likelihoodvalues[timeindex][nareas] = 0.0;
    if (TimeInfo->CurrentStep() == 1) { //start of a new year
      (*calc_c[nareas]).setElementsTo(0.0);
      (*obs_c[nareas]).setElementsTo(0.0);
    }

    for (age = minrow; age <= maxrow; age++) {
      for (length = mincol[age]; length <= maxcol[age]; length++) {
        (*Proportions[timeindex][nareas])[age][length] =
          (*alptr)[nareas][age][length].N;
        (*calc_c[nareas])[age][length] +=
          (*Proportions[timeindex][nareas])[age][length];
        (*obs_c[nareas])[age][length] +=
          (*AgeLengthData[timeindex][nareas])[age][length];
      }
    }

    if (agg_lev == 0) { //calculate likelihood on all steps
      for (age = minrow; age <= maxrow; age++) {
        for (length = mincol[age]; length <= maxcol[age]; length++) {
          Likelihoodvalues[timeindex][nareas] +=
            (*AgeLengthData[timeindex][nareas])[age][length] /
            ((*Proportions[timeindex][nareas])[age][length] + epsilon) +
            log((*Proportions[timeindex][nareas])[age][length] + epsilon);
        }
      }
      totallikelihood += Likelihoodvalues[timeindex][nareas];

    } else { //calculate  likelihood on year basis
      if (TimeInfo->CurrentStep() < TimeInfo->StepsInYear())
        Likelihoodvalues[timeindex][nareas] = 0.0;
      else { //last step in year, is to calc likelihood contribution
        for (age = minrow; age <= maxrow; age++) {
          for (length = mincol[age]; length <= maxcol[age]; length++) {
            Likelihoodvalues[timeindex][nareas] +=
              (*obs_c[nareas])[age][length] / ((*calc_c[nareas])[age][length] + epsilon) +
              log((*calc_c[nareas])[age][length] + epsilon);
          }
        }
        totallikelihood += Likelihoodvalues[timeindex][nareas];
      }
    }
  } //end nareas
  return totallikelihood;
}

double CatchDistribution::ExperimentalLik(const TimeClass* const TimeInfo) {
  //written by kgf 6/3 00 to experiment with  different likelihoodfunctions
  //At the moment, it is (C_obs -C_hat)^2/(C_obs + C_hat)^2

  double totallikelihood = 0.0;
  int age, length, min_age, max_age, nareas;
  double num = 0.0;
  double denom = 0.0;
  double frac = 0.0;

  //Get numbers from aggregator->AgeLengthDist()
  const AgeBandMatrixPtrVector* alptr = &aggregator->AgeLengthDist();
  for (nareas = 0; nareas < (*alptr).Size(); nareas++) {
    min_age = max((*alptr)[nareas].Minage(), min_stock_age - 1);
    max_age = min((*alptr)[nareas].Maxage() + 1, max_stock_age);
    Likelihoodvalues[timeindex][nareas] = 0.0;
    if (TimeInfo->CurrentStep() == 1) { //start of a new year
      (*calc_c[nareas]).setElementsTo(0.0);
      (*obs_c[nareas]).setElementsTo(0.0);
    }

    for (age = minrow; age <= maxrow; age++) {
      for (length = mincol[age]; length <= maxcol[age]; length++) {
        (*Proportions[timeindex][nareas])[age][length] =
          (*alptr)[nareas][age][length].N;
        (*calc_c[nareas])[age][length] +=
          (*Proportions[timeindex][nareas])[age][length];
        (*obs_c[nareas])[age][length] +=
          (*AgeLengthData[timeindex][nareas])[age][length];
      }
    }

    if (agg_lev == 0) { //calculate likelihood on all steps
      for (age = minrow; age <= maxrow; age++) {
        for (length = mincol[age]; length <= maxcol[age]; length++) {
          num = (*Proportions[timeindex][nareas])[age][length] -
            (*AgeLengthData[timeindex][nareas])[age][length];
          denom = (*Proportions[timeindex][nareas])[age][length] +
            (*AgeLengthData[timeindex][nareas])[age][length] + epsilon;
          if (denom > 0.) //kgf 26/6 00
            frac = num / denom;
          else
            frac = 0.0;
          Likelihoodvalues[timeindex][nareas] += (frac * frac);
        }
      }
      totallikelihood += Likelihoodvalues[timeindex][nareas];

    } else { //calculate  likelihood on year basis
      if (TimeInfo->CurrentStep() < TimeInfo->StepsInYear())
        Likelihoodvalues[timeindex][nareas] = 0.0;
      else { //last step in year, is to calc likelihood contribution
        for (age = minrow; age <= maxrow; age++) {
          for (length = mincol[age]; length <= maxcol[age]; length++) {
            num = (*calc_c[nareas])[age][length] - (*obs_c[nareas])[age][length];
            denom = (*calc_c[nareas])[age][length] + (*obs_c[nareas])[age][length] + epsilon;
            if (denom > 0.0) //kgf 26/6 00
              frac = num / denom;
            else
              frac = 0.0;
            Likelihoodvalues[timeindex][nareas] += (frac * frac);
          }
        }
        totallikelihood += Likelihoodvalues[timeindex][nareas];
      }
    }
  } //end nareas
  return totallikelihood;
}

double CatchDistribution::ModifiedMultinomial(const TimeClass* const TimeInfo) {
  //written by kgf 12/11 98 to make it possible to sum up catches
  //and calculated catches on year basis.
  //Multinomial distribution, formula according to H J Skaug
  //Modified 3/5 99 to make sure that only ages in the stock
  //contributes to the likelihood sum.

  double c_hat_sum = 0.0;
  double c_sum = 0.0;
  double min_val = 0.0;
  double totallik = 0.0;
  int age, length, min_age, max_age, nareas;

  //Get numbers from aggregator->AgeLengthDist()
  const AgeBandMatrixPtrVector* alptr = &aggregator->AgeLengthDist();
  for (nareas = 0; nareas < (*alptr).Size(); nareas++) {
    Likelihoodvalues[timeindex][nareas] = 0.0;
    min_age = max((*alptr)[nareas].Minage(), min_stock_age - 1);
    max_age = min((*alptr)[nareas].Maxage() + 1, max_stock_age);
    c_hat_sum = 0.0;
    c_sum = 0.0;
    if (TimeInfo->CurrentStep() == 1) { //start of a new year
      (*calc_c[nareas]).setElementsTo(0.0);
      (*obs_c[nareas]).setElementsTo(0.0);
    }

    for (age = min_age; age < max_age; age++) {
      for (length = (*alptr)[nareas].Minlength(age);
          length < (*alptr)[nareas].Maxlength(age); length++) {
        (*Proportions[timeindex][nareas])[age][length] =
          (*alptr)[nareas][age][length].N;
        (*calc_c[nareas])[age][length] +=      //sum up to year
          (*Proportions[timeindex][nareas])[age][length];
        (*obs_c[nareas])[age][length] +=       //sum up to year
          (*AgeLengthData[timeindex][nareas])[age][length];
      }
    }

    if (agg_lev == 0) { //calculate likelihood on all steps
      for (age = min_age; age < max_age; age++) {
        for (length = (*alptr)[nareas].Minlength(age);
            length < (*alptr)[nareas].Maxlength(age); length++) {
          Likelihoodvalues[timeindex][nareas] -=
            log((*Proportions[timeindex][nareas])[age][length] + epsilon) * //kgf 27/4 00
              (*AgeLengthData[timeindex][nareas])[age][length];
          c_hat_sum += (*Proportions[timeindex][nareas])[age][length];
          c_sum += (*AgeLengthData[timeindex][nareas])[age][length];
        }
      }

      if (c_sum <= 0.0)
        min_val = 0.0;
      else
        min_val = c_hat_sum / c_sum;
      if (min_val <= 0.0)
        Likelihoodvalues[timeindex][nareas] = 0.0;
      else {
        Likelihoodvalues[timeindex][nareas] /= c_sum;
        Likelihoodvalues[timeindex][nareas] += log(c_hat_sum);
      }

    } else { //calculate  likelihood on year basis
      if (TimeInfo->CurrentStep() < TimeInfo->StepsInYear())
        Likelihoodvalues[timeindex][nareas] = 0.0;
      else { //last step in year, is to calculate likelihood contribution
        for (age = min_age; age < max_age; age++) {
          for (length = (*alptr)[nareas].Minlength(age);
              length < (*alptr)[nareas].Maxlength(age); length++) {
            Likelihoodvalues[timeindex][nareas] -=
              log((*calc_c[nareas])[age][length] + epsilon) *  //kgf 27/4 00
                (*obs_c[nareas])[age][length];
            c_hat_sum += (*calc_c[nareas])[age][length];
            c_sum += (*obs_c[nareas])[age][length];
          }
        }

        if (c_sum <= 0.0)
          min_val = 0.0;
        else
          min_val = c_hat_sum / c_sum;
        if (min_val <= 0.0)
          Likelihoodvalues[timeindex][nareas] = 0.0;
        else {
          Likelihoodvalues[timeindex][nareas] /= c_sum;
          Likelihoodvalues[timeindex][nareas] += log(c_hat_sum);
        }
      }
    }
    totallik += Likelihoodvalues[timeindex][nareas];
  }
  return totallik;
}

double CatchDistribution::LikSumSquares() {

  double totallikelihood = 0.0, temp = 0.0;
  double totalmodel, totaldata;
  int age, len, nareas;

  const AgeBandMatrixPtrVector* alptr = &aggregator->AgeLengthDist();
  for (nareas = 0; nareas < areas.Nrow(); nareas++) {
    totalmodel = 0.0;
    totaldata = 0.0;
    for (age = (*alptr)[nareas].Minage(); age <= (*alptr)[nareas].Maxage(); age++) {
      for (len = (*alptr)[nareas].Minlength(age); len < (*alptr)[nareas].Maxlength(age); len++) {
        (*Proportions[timeindex][nareas])[age][len] = ((*alptr)[nareas][age][len]).N;
        totalmodel += (*Proportions[timeindex][nareas])[age][len];
        totaldata += (*AgeLengthData[timeindex][nareas])[age][len];
      }
    }

    for (age = (*alptr)[nareas].Minage(); age <= (*alptr)[nareas].Maxage(); age++) {
      for (len = (*alptr)[nareas].Minlength(age); len < (*alptr)[nareas].Maxlength(age); len++) {
        if ((isZero(totaldata)) && (isZero(totalmodel)))
          temp = 0.0;
        else if (isZero(totaldata))
          temp = (*Proportions[timeindex][nareas])[age][len] / totalmodel;
        else if (isZero(totalmodel))
          temp = (*AgeLengthData[timeindex][nareas])[age][len] / totaldata;
        else
          temp = (((*AgeLengthData[timeindex][nareas])[age][len] / totaldata)
            - ((*Proportions[timeindex][nareas])[age][len] / totalmodel));

        totallikelihood += (temp * temp);
      }
    }
  }
  return totallikelihood;
}

void CatchDistribution::PrintLikelihoodHeader(ofstream& catchfile) {

  //cannot print if functionnumber is 1 or 6, since havent got mincol etc
  if ((functionnumber == 1) || (functionnumber == 6)) {
    catchfile << "Likelihood:       catchdistribution - " << cdname << endl
      << "Cannot print likelihood information for function " << functionname << endl;
    return;
  }

  int i;
  int mina = ages[minrow][0];
  int maxa = ages[maxrow][0];
  catchfile << "Likelihood:       catchdistribution - " << cdname << "\nFunction:         "
    << functionname << "\nCalculated every: ";
  if (agg_lev == 0)
    catchfile << "step\n";
  else if (agg_lev == 1)
    catchfile << "year\n";

  catchfile << "Filter:           default\nEpsilon:          "
    << epsilon << "\nFleets:          ";
  for (i = 0; i < fleetnames.Size(); i++)
    catchfile << sep << fleetnames[i];
  catchfile << "\nStocks:          ";
  for (i = 0; i < stocknames.Size(); i++)
    catchfile << sep << stocknames[i];
  catchfile << "\nAges:             min " << mina << " max " << maxa
    << "\nLengths:          min " << LgrpDiv->Minlength(0) << " max "
    << LgrpDiv->Maxlength(LgrpDiv->NoLengthGroups() - 1) << " dl " << LgrpDiv->dl() << endl;
}

//Print Observed and modelled catch for further processing by external scripts
void CatchDistribution::PrintLikelihood(ofstream& catchfile, const TimeClass& TimeInfo) {

  if (!AAT.AtCurrentTime(&TimeInfo))
    return;

  //cannot print if functionnumber is 1 or 6, since havent got mincol etc
  if ((functionnumber == 1) || (functionnumber == 6))
    return;

  catchfile.setf(ios::fixed);
  int i;
  int age, length, min_age, max_age, nareas;
  int time = timeindex - 1; //timeindex was increased before this is called.

  //Get age and length intervals from aggregator->AgeLengthDist()
  const AgeBandMatrixPtrVector* alptr = &aggregator->AgeLengthDist();
  catchfile << "\nTime:    Year " << TimeInfo.CurrentYear()
    << " Step " << TimeInfo.CurrentStep() << "\nName:   ";
  for (i = 0; i < fleetnames.Size(); i++)
    catchfile << sep << fleetnames[i];
  catchfile << endl;

  for (nareas = 0; nareas < (*alptr).Size(); nareas++) {
    catchfile << "Area:    " << nareas << endl;
    min_age = max((*alptr)[nareas].Minage(), min_stock_age - 1);
    max_age = min((*alptr)[nareas].Maxage() + 1, max_stock_age);

    catchfile << "Observed:\n";
    for (age = minrow; age <= maxrow; age++) {
      for (length = 0; length < mincol[age]; length++) {
        catchfile.precision(lowprecision);
        catchfile.width(largewidth);
        catchfile << 0.0 << sep;
      }
      for (length = mincol[age]; length <= maxcol[age]; length++) {
        catchfile.precision(smallprecision);
        catchfile.width(largewidth);
        catchfile << (*AgeLengthData[time][nareas])[age][length] << sep;
      }
      for (length = maxcol[age] + 1; length < (*alptr)[nareas].Maxlength(age); length++) {
        catchfile.precision(lowprecision);
        catchfile.width(largewidth);
        catchfile << 0.0 << sep;
      }
      catchfile << endl;
    }

    catchfile << "Modelled:\n";
    for (age = minrow; age <= maxrow; age++) {
      for (length = 0; length < mincol[age]; length++) {
        catchfile.precision(lowprecision);
        catchfile.width(largewidth);
        catchfile << 0.0 << sep;
      }
      for (length = mincol[age]; length <= maxcol[age]; length++) {
        catchfile.precision(smallprecision);
        catchfile.width(largewidth);
        catchfile << (*Proportions[time][nareas])[age][length] << sep;
      }
      for (length = maxcol[age] + 1; length < (*alptr)[nareas].Maxlength(age); length++) {
        catchfile.precision(lowprecision);
        catchfile.width(largewidth);
        catchfile << 0.0 << sep;
      }
      catchfile << endl;
    }
  }
  catchfile.flush();
}

const AgeBandMatrixPtrVector& CatchDistribution::getModCatch(const TimeClass* const TimeInfo) {
  aggregator->Sum(TimeInfo, 1); //mortality model, calculated catch
  return aggregator->AgeLengthDist();
}

IntVector CatchDistribution::getSteps() {
  //written by kgf 11/2 99
  IntVector step(Steps);
  return step;
}

IntVector CatchDistribution::getYears() {
  //written by kgf 11/2 99
  IntVector years(Years);
  return years;
}

int CatchDistribution::getAreas() {
  return areas.Nrow();
}

int CatchDistribution::getTimeSteps() {
  return AgeLengthData.Nrow();
}
