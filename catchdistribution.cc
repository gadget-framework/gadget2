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
#include "ludecomposition.h"
#include "gadget.h"

CatchDistribution::CatchDistribution(CommentStream& infile, const AreaClass* const Area,
  const TimeClass* const TimeInfo, Keeper* const keeper, double w, const char* name)
  : Likelihood(CATCHDISTRIBUTIONLIKELIHOOD, w) {

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
  cdname = new char[strlen(name) + 1];
  strcpy(cdname, name);
  functionname = new char[MaxStrLength];
  strncpy(functionname, "", MaxStrLength);

  readWordAndValue(infile, "datafile", datafilename);
  readWordAndValue(infile, "function", functionname);

  functionnumber = 0;
  if (strcasecmp(functionname, "multinomial") == 0) {
    functionnumber = 1;
  } else if (strcasecmp(functionname, "pearson") == 0) {
    functionnumber = 2;
  } else if (strcasecmp(functionname, "gamma") == 0) {
    functionnumber = 3;
  } else if (strcasecmp(functionname, "sumofsquares") == 0) {
    functionnumber = 4;
  } else if (strcasecmp(functionname, "mvn-normal1") == 0) {
    functionnumber = 5;
    infile >> text >> ws;
    if (strcasecmp(text, "sigma") == 0)
      infile >> sigma >> ws;
    else
      handle.Unexpected("sigma", text);
    sigma.Inform(keeper);

    infile >> text >> ws;
    if (strcasecmp(text, "rho") == 0)
      infile >> rho >> ws;
    else
      handle.Unexpected("rho", text);
    rho.Inform(keeper);

  } else if (strcasecmp(functionname, "mvn-notused1") == 0) {
    functionnumber = 6;

    infile >> text >> ws;
    if (strcasecmp(text, "sigma") == 0)
      infile >> sigma >> ws;
    else
      handle.Unexpected("sigma", text);
    sigma.Inform(keeper);

    infile >> text >> ws;
    if (strcasecmp(text, "a") == 0)
      infile >> a >> ws;
    else
      handle.Unexpected("a", text);
    a.Inform(keeper);

    infile >> text >> ws;
    if (strcasecmp(text, "b") == 0)
      infile >> b >> ws;
    else
      handle.Unexpected("b", text);
    b.Inform(keeper);

  } else if (strcasecmp(functionname, "mvn-notused2") == 0) {
    functionnumber = 7;

    infile >> text >> ws;
    if (strcasecmp(text, "sigma") == 0)
      infile >> sigma >> ws;
    else
      handle.Unexpected("sigma", text);
    sigma.Inform(keeper);

    infile >> text >> ws;
    if (strcasecmp(text, "a") == 0)
      infile >> a >> ws;
    else
      handle.Unexpected("a", text);
    a.Inform(keeper);

    infile >> text >> ws;
    if (strcasecmp(text, "b") == 0)
      infile >> b >> ws;
    else
      handle.Unexpected("b", text);
    b.Inform(keeper);

  } else if (strcasecmp(functionname, "mvn-normal2") == 0) {
    functionnumber = 8;

    infile >> text >> ws;
    if (strcasecmp(text, "sigma") == 0)
      infile >> sigma >> ws;
    else
      handle.Unexpected("sigma", text);
    sigma.Inform(keeper);

    infile >> text >> ws;
    if (strcasecmp(text, "a") == 0)
      infile >> a >> ws;
    else
      handle.Unexpected("a", text);
    a.Inform(keeper);

    infile >> text >> ws;
    if (strcasecmp(text, "b") == 0)
      infile >> b >> ws;
    else
      handle.Unexpected("b", text);
    b.Inform(keeper);

  } else if (strcasecmp(functionname, "mvlogistic") == 0) {
    functionnumber = 9;

    infile >> text >> ws;
    if (strcasecmp(text, "tau") == 0)
      infile >> tau >> ws;
    else
      handle.Unexpected("tau", text);
    tau.Inform(keeper);

  } else
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
  ReadDistributionData(subdata, TimeInfo, numarea, numage, numlen);
  handle.Close();
  datafile.close();
  datafile.clear();

  Likelihoodvalues.AddRows(AgeLengthData.Nrow(), numarea, 0.0);
  calc_c.resize(numarea);
  obs_c.resize(numarea);
  for (i = 0; i < numarea; i++) {
    calc_c[i] = new DoubleMatrix(numage, numlen, 0.0);
    obs_c[i] = new DoubleMatrix(numage, numlen, 0.0);
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
          AgeLengthData[timeid][i] = new DoubleMatrix(numage, numlen, 0.0);
          Proportions[timeid][i] = new DoubleMatrix(numage, numlen, 0.0);
        }
      }

    } else {
      //dont keep the data
      keepdata = 1;
    }

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
      delete Proportions[i][j];
    }
  for (i = 0; i < calc_c.Size(); i++) {
    delete calc_c[i];
    delete obs_c[i];
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

  outfile << "\nCatch Distribution " << cdname << " - likelihood value " << likelihood
    << "\n\tFunction " << functionname << "\n\tStock names:";
  for (i = 0; i < stocknames.Size(); i++)
    outfile << sep << stocknames[i];
  outfile << "\n\tFleet names:";
  for (i = 0; i < fleetnames.Size(); i++)
    outfile << sep << fleetnames[i];
  outfile << endl;

  switch(functionnumber) {
    case 1:
    case 2:
    case 3:
    case 4:
      break;
    case 5:
      outfile << "\tMultivariate normal distribution parameters: sigma " << sigma << " rho " << rho << endl;
      break;
    case 6:
    case 7:
    case 8:
      outfile << "\tMultivariate normal distribution parameters: sigma " << sigma << " a " << a << " b " << b << endl;
      break;
    case 9:
      outfile << "\tMultivariate logistic distribution parameter: tau " << tau << endl;
      break;
    default:
      cerr << "Error in catchdistribution - unknown function " << functionname << endl;
      break;
  }

  aggregator->Print(outfile);
  outfile.flush();
}

void CatchDistribution::LikelihoodPrint(ofstream& outfile) {
  int i, j, year, area;

  outfile << "\nCatch Distribution\n\nLikelihood " << likelihood << "\nFunction "
    << functionname << "\nWeight " << weight << "\nStock names:";
  for (i = 0; i < stocknames.Size(); i++)
    outfile << sep << stocknames[i];
  outfile << "\nInner areas:";
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

  switch(functionnumber) {
    case 1:
    case 2:
    case 3:
    case 4:
      break;
    case 5:
      outfile << "Multivariate normal distribution parameters: sigma " << sigma << " rho " << rho << endl;
      break;
    case 6:
    case 7:
    case 8:
      outfile << "Multivariate normal distribution parameters: sigma " << sigma << " a " << a << " b " << b << endl;
      break;
    case 9:
      outfile << "Multivariate logistic distribution parameter: tau " << tau << endl;
      break;
    default:
      cerr << "Error in catchdistribution - unknown function " << functionname << endl;
      break;
  }

  //aggregator->Print(outfile);
  outfile << "\nAge length distribution data:\n";
  for (year = 0; year < AgeLengthData.Nrow(); year++) {
    outfile << "\nYear " << Years[year] << " and step " << Steps[year];
    for (area = 0; area < AgeLengthData.Ncol(year); area++) {
      outfile << "\nInner area: " << area << "\nMeasurements";
      for (i = 0; i < AgeLengthData[year][area]->Nrow(); i++) {
        outfile << endl;
        for (j = 0; j < AgeLengthData[year][area]->Ncol(i); j++) {
          outfile.width(smallwidth);
          outfile.precision(smallprecision);
          outfile << sep << (*AgeLengthData[year][area])[i][j];
        }
      }
      outfile << "\nNumber caught according to model";
      for (i = 0; i < Proportions[year][area]->Nrow(); i++) {
        outfile << endl;
        for (j = 0; j < Proportions[year][area]->Ncol(i); j++) {
          outfile.width(smallwidth);
          outfile.precision(smallprecision);
          outfile << sep << (*Proportions[year][area])[i][j];
        }
      }
    }
    outfile << "\nLikelihood values:";
    for (area = 0; area < AgeLengthData.Ncol(year); area++) {
       outfile.width(smallwidth);
       outfile.precision(smallprecision);
       outfile << sep << Likelihoodvalues[year][area];
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
  mincol = aggregator->getMinCol();
  maxcol = aggregator->getMaxCol();
  minrow = aggregator->getMinRow();
  maxrow = aggregator->getMaxRow();
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
        likelihood += LikGamma(TimeInfo);
        break;
      case 4:
        aggregator->Sum(TimeInfo);
        likelihood += LikSumSquares();
        break;
      case 5:
      case 6:
      case 7:
      case 8:
        aggregator->Sum(TimeInfo);
        likelihood += LikMVNormal();
        break;
      case 9:
        aggregator->Sum(TimeInfo);
        likelihood += LikMVLogistic();
        break;
      default:
        cerr << "Error in catchdistribution - unknown function " << functionname << endl;
        break;
    }
    timeindex++;
  }
}

double CatchDistribution::LikMultinomial() {
  int area, age, len, nareas;

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
    Likelihoodvalues[timeindex][nareas] = 0.0;
    if (AgeLengthData[timeindex][nareas]->Nrow() == 1) {
      //If there is only one agegroup, we calculate loglikelihood
      //based on the length group division
      Likelihoodvalues[timeindex][nareas] += MN.CalcLogLikelihood((*AgeLengthData[timeindex][nareas])[0], (*Dist[nareas])[0]);
      for (len = 0; len < AgeLengthData[timeindex][nareas]->Ncol(0); len++)
        (*Proportions[timeindex][nareas])[0][len] = (*Dist[nareas])[0][len];

    } else {
      //Calculate loglikelihood based on age distribution within each length
      //group. To be able to use the Multinomial class, we must organize the
      //data such that we get vectors with the age group distribution.
      for (len = 0; len < Dist[nareas]->Ncol(0); len++) {
        DoubleVector dist(Dist[nareas]->Nrow());
        DoubleVector data(Dist[nareas]->Nrow());
        for (area = 0; area < Dist[nareas]->Nrow(); area++) {
          dist[area] = (*Dist[nareas])[area][len];
          data[area] = (*AgeLengthData[timeindex][nareas])[area][len];
        }
        Likelihoodvalues[timeindex][nareas] += MN.CalcLogLikelihood(data, dist);
        for (area = 0; area < dist.Size(); area++)
          (*Proportions[timeindex][nareas])[area][len] = dist[area];
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
  }
  return totallikelihood;
}

double CatchDistribution::LikGamma(const TimeClass* const TimeInfo) {
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
  }
  return totallikelihood;
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

double CatchDistribution::LikMVNormal() {

  double totallikelihood = 0.0;
  double sumdata = 0.0, sumdist = 0.0;
  int age, len, nareas;
  int i, j, p;

  const AgeBandMatrixPtrVector* alptr= &aggregator->AgeLengthDist();
  p = aggregator->NoLengthGroups();
  DoubleVector corr(p, 1.0);
  DoubleVector diff(p, 0.0);
  DoubleMatrix correlation(p, p, 1.0);

  for (nareas = 0; nareas < areas.Nrow(); nareas++) {
    for (age = (*alptr)[nareas].Minage(); age <= (*alptr)[nareas].Maxage(); age++) {
      for (len = (*alptr)[nareas].Minlength(age); len < (*alptr)[nareas].Maxlength(age); len++) {
        (*Proportions[timeindex][nareas])[age][len] = ((*alptr)[nareas][age][len]).N;
        sumdata += (*AgeLengthData[timeindex][nareas])[age][len];
        sumdist += (*alptr)[nareas][age][len].N;
      }
    }
  }
  if (isZero(sumdata))
    sumdata = verybig;
  else
    sumdata = 1 / sumdata;
  if (isZero(sumdist))
    sumdist = verybig;
  else
    sumdist = 1 / sumdist;

  //create correlation matrix
  if (functionnumber == 5) {         //pattern: \rho_{ij} = \rho^{|i - j|}
    assert(abs(rho) < 1);
    for (j = 1; j < p; j++)
      corr[j] = rho * corr[j - 1];
    for (i = 0; i < p; i++)
      for (j = 0; j < p; j++)
        correlation[i][j] = corr[abs(i - j)];

  } else if (functionnumber == 6) {  //pattern: \rho_{ij} = a + b * \rho_{|i - j|}
    assert(abs(a + b) < 1);
    for (i = 1; i < p; i++)
      for (j = i; j < p; j++)
        corr[j] = a + (b * corr[j]);
    for (i = 0; i < p; i++)
      for (j = 0; j < p; j++)
        correlation[i][j] = corr[abs(i - j)];

  } else if (functionnumber == 7) {  //pattern: \rho_{ij} = a + b * k
    assert(abs(a + b * p) < 1);
    for (i = 0; i < p; i++)
      for (j = 0; j < p; j++) {
        if (i == j)
          correlation[i][j] = 1.0;
        else
          correlation[i][j] = a + (b * abs(i - j));
      }

  } else if (functionnumber == 8) { //pattern: \rho_{ij} = a * \rho_{|i - j| - 1} + b * \rho_{|i - j| - 2}
    assert(abs(a + b) < 1);
    correlation[0][0] = 1.0;
    correlation[0][1] = a;
    correlation[1][0] = a;
    for (i = 2; i < p; i++) {
      correlation[i][i] = 1.0;
      for (j = 0; j < i; j++) {
        correlation[i][j] = (a * correlation[i - 1][j]) + (b * correlation[i - 2][j]);
        correlation[j][i] = correlation[i][j];
      }
    }
  } else
    cerr << "Error in catchdistribution - unknown function " << functionname << endl;

  LUDecomposition LU = LUDecomposition(correlation);
  for (nareas = 0; nareas < areas.Nrow(); nareas++) {
    for (age = (*alptr)[nareas].Minage(); age <= (*alptr)[nareas].Maxage(); age++) {
      for (len = 0; len < diff.Size(); len++)
        diff[len] = 0.0;

      for (len = (*alptr)[nareas].Minlength(age); len < (*alptr)[nareas].Maxlength(age); len++)
        diff[len] = ((*AgeLengthData[timeindex][nareas])[age][len] * sumdata) - (((*alptr)[nareas][age][len]).N * sumdist);

      totallikelihood += diff * LU.Solve(diff);
    }
  }
  totallikelihood = (totallikelihood / (sigma * sigma)) + ((LU.LogDet() + 2 * log(sigma) * p) * alptr->Size());
  return abs(totallikelihood);
}

double CatchDistribution::LikMVLogistic() {

  double totallikelihood = 0.0;
  double sumdata = 0.0, sumdist = 0.0, sumnu = 0.0;
  int age, len, nareas, p;

  const AgeBandMatrixPtrVector* alptr= &aggregator->AgeLengthDist();
  p = aggregator->NoLengthGroups();
  DoubleVector nu(p, 0.0);

  for (nareas = 0; nareas < areas.Nrow(); nareas++) {
    for (age = (*alptr)[nareas].Minage(); age <= (*alptr)[nareas].Maxage(); age++) {
      for (len = 0; len < nu.Size(); len++)
        nu[len] = 0.0;

      sumdata = 0.0;
      sumdist = 0.0;
      for (len = (*alptr)[nareas].Minlength(age); len < (*alptr)[nareas].Maxlength(age); len++) {
        sumdata += (*AgeLengthData[timeindex][nareas])[age][len];
        sumdist += (*alptr)[nareas][age][len].N;
      }
      if (isZero(sumdata))
        sumdata = verybig;
      else
        sumdata = 1 / sumdata;
      if (isZero(sumdist))
        sumdist = verybig;
      else
        sumdist = 1 / sumdist;

      sumnu = 0.0;
      for (len = (*alptr)[nareas].Minlength(age); len < (*alptr)[nareas].Maxlength(age); len++) {
        nu[len] = log(((*AgeLengthData[timeindex][nareas])[age][len] * sumdata) + verysmall)
                    - log(((*alptr)[nareas][age][len].N * sumdist) + verysmall);

        sumnu += nu[len];
      }
      sumnu = sumnu / p;

      for (len = (*alptr)[nareas].Minlength(age); len < (*alptr)[nareas].Maxlength(age); len++)
        totallikelihood += (nu[len] - sumnu) * (nu[len] - sumnu);
    }
  }
  totallikelihood = (totallikelihood / (tau * tau)) + (log(tau) * (p - 1));
  return totallikelihood;
}

void CatchDistribution::PrintLikelihoodHeader(ofstream& catchfile) {

  int i;
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
  catchfile << "\nAges:             min " << ages[minrow][0] << " max " << ages[maxrow][0]
    << "\nLengths:          min " << LgrpDiv->Minlength(0) << " max "
    << LgrpDiv->Maxlength(LgrpDiv->NoLengthGroups() - 1) << " dl " << LgrpDiv->dl() << endl;
}

//Print Observed and modelled catch for further processing by external scripts
void CatchDistribution::PrintLikelihood(ofstream& catchfile, const TimeClass& TimeInfo) {

  if (!AAT.AtCurrentTime(&TimeInfo))
    return;

  catchfile.setf(ios::fixed);
  int i;
  int age, length, nareas;
  int time = timeindex - 1; //timeindex was increased before this is called.

  //Get age and length intervals from aggregator->AgeLengthDist()
  const AgeBandMatrixPtrVector* alptr = &aggregator->AgeLengthDist();
  catchfile << "\nTime:    Year " << TimeInfo.CurrentYear()
    << " Step " << TimeInfo.CurrentStep() << "\nFleets: ";
  for (i = 0; i < fleetnames.Size(); i++)
    catchfile << sep << fleetnames[i];
  catchfile << endl;

  switch(functionnumber) {
    case 1:
    case 2:
    case 3:
    case 4:
      break;
    case 5:
      catchfile << "Multivariate normal distribution parameters: sigma " << sigma << " rho " << rho << endl;
      break;
    case 6:
    case 7:
    case 8:
      catchfile << "Multivariate normal distribution parameters: sigma " << sigma << " a " << a << " b " << b << endl;
      break;
    case 9:
      catchfile << "Multivariate logistic distribution parameter: tau " << tau << endl;
      break;
    default:
      cerr << "Error in catchdistribution - unknown function " << functionname << endl;
      break;
  }

  for (nareas = 0; nareas < (*alptr).Size(); nareas++) {
    catchfile << "Inner area:   " << nareas  << "\nObserved:\n";
    for (age = minrow; age <= maxrow; age++) {
      for (length = 0; length < mincol[age]; length++) {
        catchfile.precision(smallprecision);
        catchfile.width(smallwidth);
        catchfile << 0.0 << sep;
      }
      for (length = mincol[age]; length <= maxcol[age]; length++) {
        catchfile.precision(smallprecision);
        catchfile.width(smallwidth);
        catchfile << (*AgeLengthData[time][nareas])[age][length] << sep;
      }
      for (length = maxcol[age] + 1; length < (*alptr)[nareas].Maxlength(age); length++) {
        catchfile.precision(smallprecision);
        catchfile.width(smallwidth);
        catchfile << 0.0 << sep;
      }
      catchfile << endl;
    }

    catchfile << "Modelled:\n";
    for (age = minrow; age <= maxrow; age++) {
      for (length = 0; length < mincol[age]; length++) {
        catchfile.precision(smallprecision);
        catchfile.width(smallwidth);
        catchfile << 0.0 << sep;
      }
      for (length = mincol[age]; length <= maxcol[age]; length++) {
        catchfile.precision(smallprecision);
        catchfile.width(smallwidth);
        catchfile << (*Proportions[time][nareas])[age][length] << sep;
      }
      for (length = maxcol[age] + 1; length < (*alptr)[nareas].Maxlength(age); length++) {
        catchfile.precision(smallprecision);
        catchfile.width(smallwidth);
        catchfile << 0.0 << sep;
      }
      catchfile << endl;
    }
  }
  catchfile.flush();
}
