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

extern ErrorHandler handle;

CatchDistribution::CatchDistribution(CommentStream& infile, const AreaClass* const Area,
  const TimeClass* const TimeInfo, Keeper* const keeper, double weight, const char* name)
  : Likelihood(CATCHDISTRIBUTIONLIKELIHOOD, weight, name) {

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
  illegal = 0;
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
  } else if (strcasecmp(functionname, "mvn") == 0) {
    functionnumber = 5;

    readWordAndVariable(infile, "lag", lag);
    readWordAndFormula(infile, "sigma", sigma);
    sigma.Inform(keeper);

    for (i = 0; i < lag; i++) {
      params.resize(1, keeper);
      infile >> text >> ws;
      if (strcasecmp(text, "param") == 0) {
        infile >> params[i] >> ws;
        params[i].Inform(keeper);
      } else
        handle.Unexpected("param", text);
    }

  } else if (strcasecmp(functionname, "mvlogistic") == 0) {
    functionnumber = 6;

    readWordAndFormula(infile, "sigma", sigma);
    sigma.Inform(keeper);

  } else if (strcasecmp(functionname, "log") == 0) {
    //JMB moved the logcatch function to here instead of it being a seperate class
    functionnumber = 7;

  } else
    handle.Message("Error in catchdistribution - unrecognised function", functionname);

  infile >> ws;
  char c = infile.peek();
  if ((c == 'a') || (c == 'A')) {
    infile >> text >> ws;
    //JMB - changed to check for aggregation_level or aggregationlevel
    if ((strcasecmp(text, "aggregation_level") == 0) || (strcasecmp(text, "aggregationlevel") == 0))
      infile >> yearly >> ws;
    else
      handle.Unexpected("overconsumption", text);

  } else
    yearly = 0; //default value for yearly

  if (yearly != 0 && yearly != 1)
    handle.Message("Error in catchdistribution - aggregationlevel must be 0 or 1");

  if (yearly == 1) {
    switch(functionnumber) {
      case 2:
      case 3:
      case 7:
        break;
      case 1:
      case 4:
      case 5:
      case 6:
        handle.logWarning("Warning in catchdistribution - yearly aggregation is ignored for function", functionname);
        break;
      default:
        handle.logWarning("Warning in catchdistribution - unrecognised function", functionname);
        break;
    }
  }

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

  LgrpDiv = new LengthGroupDivision(lengths);

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
  readDistributionData(subdata, TimeInfo, numarea, numage, numlen);
  handle.Close();
  datafile.close();
  datafile.clear();

  likelihoodValues.AddRows(obsDistribution.Nrow(), numarea, 0.0);
  calc_c.resize(numarea);
  obs_c.resize(numarea);
  for (i = 0; i < numarea; i++) {
    calc_c[i] = new DoubleMatrix(numage, numlen, 0.0);
    obs_c[i] = new DoubleMatrix(numage, numlen, 0.0);
  }
}

void CatchDistribution::readDistributionData(CommentStream& infile,
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

  //Find start of distribution data in datafile
  infile >> ws;
  char c = infile.peek();
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
    if ((TimeInfo->isWithinPeriod(year, step)) && (keepdata == 0)) {
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
          obsDistribution[timeid][i] = new DoubleMatrix(numage, numlen, 0.0);
          modelDistribution[timeid][i] = new DoubleMatrix(numage, numlen, 0.0);
        }
      }

    } else {
      //dont keep the data
      keepdata = 1;
    }

    if (keepdata == 0) {
      //distribution data is required, so store it
      count++;
      (*obsDistribution[timeid][areaid])[ageid][lenid] = tmpnumber;
    }
  }

  AAT.addActions(Years, Steps, TimeInfo);
  if (count == 0)
    handle.logWarning("Warning in catchdistribution - found no data in the data file for", this->Name());
  handle.logMessage("Read catchdistribution data file - number of entries", count);
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
  for (i = 0; i < obsDistribution.Nrow(); i++)
    for (j = 0; j < obsDistribution.Ncol(i); j++) {
      delete obsDistribution[i][j];
      delete modelDistribution[i][j];
    }
  for (i = 0; i < calc_c.Size(); i++) {
    delete calc_c[i];
    delete obs_c[i];
  }
  delete aggregator;
  delete LgrpDiv;
  delete[] functionname;
}

void CatchDistribution::Reset(const Keeper* const keeper) {
  Likelihood::Reset(keeper);
  timeindex = 0;
  illegal = 0;
  handle.logMessage("Reset catchdistribution component", this->Name());
}

void CatchDistribution::Print(ofstream& outfile) const {
  int i;

  outfile << "\nCatch Distribution " << this->Name() << " - likelihood value " << likelihood
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
    case 7:
      break;
    case 5:
      outfile << "\tMultivariate normal distribution parameters: sigma " << sigma;
      for (i = 0; i < lag; i++)
        outfile << " param" << i + 1 << " " << params[i];
      outfile << endl;
      break;
    case 6:
      outfile << "\tMultivariate logistic distribution parameter: sigma " << sigma << endl;
      break;
    default:
      handle.logWarning("Warning in catchdistribution - unrecognised function", functionname);
      break;
  }

  aggregator->Print(outfile);
  outfile.flush();
}

void CatchDistribution::LikelihoodPrint(ofstream& outfile, const TimeClass* const TimeInfo) {

  if (!AAT.AtCurrentTime(TimeInfo))
    return;

  int t, area, age, len;
  t = timeindex - 1; //timeindex was increased before this is called

  if ((t >= Years.Size()) || t < 0)
    handle.logFailure("Error in catchdistribution - invalid timestep", t);

  for (area = 0; area < modelDistribution.Ncol(t); area++) {
    for (age = 0; age < modelDistribution[t][area]->Nrow(); age++) {
      for (len = 0; len < modelDistribution[t][area]->Ncol(age); len++) {
        outfile << setw(lowwidth) << Years[t] << sep << setw(lowwidth)
          << Steps[t] << sep << setw(printwidth) << areaindex[area] << sep
          << setw(printwidth) << ageindex[age] << sep << setw(printwidth)
          << lenindex[len] << sep << setprecision(largeprecision) << setw(largewidth)
          << (*modelDistribution[t][area])[age][len] << endl;
      }
    }
  }
}

void CatchDistribution::setFleetsAndStocks(FleetPtrVector& Fleets, StockPtrVector& Stocks) {
  int i, j, found;
  FleetPtrVector fleets;
  StockPtrVector stocks;

  for (i = 0; i < fleetnames.Size(); i++) {
    found = 0;
    for (j = 0; j < Fleets.Size(); j++)
      if (strcasecmp(fleetnames[i], Fleets[j]->Name()) == 0) {
        found ++;
        fleets.resize(1, Fleets[j]);
      }

    if (found == 0)
      handle.logFailure("Error in catchdistribution - unrecognised fleet", fleetnames[i]);
  }

  for (i = 0; i < stocknames.Size(); i++) {
    found = 0;
    for (j = 0; j < Stocks.Size(); j++) {
      if (Stocks[j]->isEaten())
        if (strcasecmp(stocknames[i], Stocks[j]->returnPrey()->Name()) == 0) {
          found++;
          stocks.resize(1, Stocks[j]);
        }
    }
    if (found == 0)
      handle.logFailure("Error in catchdistribution - unrecognised stock", stocknames[i]);
  }

  aggregator = new FleetPreyAggregator(fleets, stocks, LgrpDiv, areas, ages, overconsumption);
  //Limits (inclusive) for traversing the matrices where required
  mincol = aggregator->getMinCol();
  maxcol = aggregator->getMaxCol();
  minrow = aggregator->getMinRow();
  maxrow = aggregator->getMaxRow();
}

void CatchDistribution::addLikelihood(const TimeClass* const TimeInfo) {

  double l = 0.0;
  if (AAT.AtCurrentTime(TimeInfo)) {
    aggregator->Sum(TimeInfo);
    switch(functionnumber) {
      case 1:
        l = calcLikMultinomial();
        break;
      case 2:
        l = calcLikPearson(TimeInfo);
        break;
      case 3:
        l = calcLikGamma(TimeInfo);
        break;
      case 4:
        l = calcLikSumSquares();
        break;
      case 5:
        if (timeindex == 0) {
          Correlation();
          if (illegal == 1 || LU.isIllegal() == 1) {
            handle.logWarning("Warning in catchdistribution - multivariate normal out of bounds");
            l = verybig;
          }
        }
        if (illegal != 1 && LU.isIllegal() != 1)
          l = calcLikMVNormal();
        break;
      case 6:
        l = calcLikMVLogistic();
        break;
      case 7:
        l = calcLikLog(TimeInfo);
        break;
      default:
        handle.logWarning("Warning in catchdistribution - unrecognised function", functionname);
        break;
    }
    if ((yearly == 0) || (TimeInfo->CurrentStep() == TimeInfo->StepsInYear())) {
      likelihood += l;
      handle.logMessage("Calculating likelihood score for catchdistribution component", this->Name());
      handle.logMessage("The likelihood score for this component on this timestep is", l);
    }
    timeindex++;
  }
}

double CatchDistribution::calcLikMultinomial() {

  //The object MN does most of the work, accumulating likelihood
  Multinomial MN(epsilon);
  int area, age, len;
  const AgeBandMatrixPtrVector* alptr = &aggregator->returnSum();

  for (area = 0; area < areas.Nrow(); area++) {
    likelihoodValues[timeindex][area] = 0.0;
    for (age = (*alptr)[area].minAge(); age <= (*alptr)[area].maxAge(); age++)
      for (len = (*alptr)[area].minLength(age); len < (*alptr)[area].maxLength(age); len++)
        (*modelDistribution[timeindex][area])[age][len] = ((*alptr)[area][age][len]).N;

    for (age = (*alptr)[area].minAge(); age <= (*alptr)[area].maxAge(); age++)
      likelihoodValues[timeindex][area] +=
        MN.calcLogLikelihood((*obsDistribution[timeindex][area])[age],
          (*modelDistribution[timeindex][area])[age]);

  }
  return MN.returnLogLikelihood();
}

double CatchDistribution::calcLikPearson(const TimeClass* const TimeInfo) {
  /* written by Hoskuldur Bjornsson 29/8 98
   * corrected by kgf 16/9 98
   * modified by kgf 11/11 98 to make it possible to sum up catches
   * and calculated catches on year basis.
   * Modified 3/5 99 by kgf to check the age intervals of the stock
   * and the catch data, and make use of the ages that are common
   * for the stock and the catch data.*/

  double totallikelihood = 0.0;
  int age, length, area;

  const AgeBandMatrixPtrVector* alptr = &aggregator->returnSum();
  for (area = 0; area < (*alptr).Size(); area++) {
    likelihoodValues[timeindex][area] = 0.0;
    if (TimeInfo->CurrentStep() == 1) { //start of a new year
      (*calc_c[area]).setElementsTo(0.0);
      (*obs_c[area]).setElementsTo(0.0);
    }

    for (age = minrow; age <= maxrow; age++) { //test, mnaa
      for (length = mincol[age]; length <= maxcol[age]; length++) { //test, mnaa
        (*modelDistribution[timeindex][area])[age][length] = (*alptr)[area][age][length].N;
        (*calc_c[area])[age][length] += (*modelDistribution[timeindex][area])[age][length];
        (*obs_c[area])[age][length] += (*obsDistribution[timeindex][area])[age][length];
      }
    }

    if (yearly == 0) { //calculate likelihood on all steps
      for (age = minrow; age <= maxrow; age++) { //test, mnaa
        for (length = mincol[age]; length <= maxcol[age]; length++) { //test, mnaa
          likelihoodValues[timeindex][area] +=
            ((*modelDistribution[timeindex][area])[age][length] -
            (*obsDistribution[timeindex][area])[age][length]) *
            ((*modelDistribution[timeindex][area])[age][length] -
            (*obsDistribution[timeindex][area])[age][length]) /
            absolute(((*modelDistribution[timeindex][area])[age][length] + epsilon));
        }
      }
      totallikelihood += likelihoodValues[timeindex][area];

    } else { //calculate likelihood on year basis
      if (TimeInfo->CurrentStep() < TimeInfo->StepsInYear())
        likelihoodValues[timeindex][area] = 0.0;
      else { //last step in year, is to calc likelihood contribution
        for (age = minrow; age <= maxrow; age++) { //test, mnaa
          for (length = mincol[age]; length <= maxcol[age]; length++) { //test, mnaa
            likelihoodValues[timeindex][area] +=
              ((*calc_c[area])[age][length] - (*obs_c[area])[age][length]) *
              ((*calc_c[area])[age][length] - (*obs_c[area])[age][length]) /
              absolute(((*calc_c[area])[age][length] + epsilon));
          }
        }
        totallikelihood += likelihoodValues[timeindex][area];
      }
    }
  }
  return totallikelihood;
}

double CatchDistribution::calcLikGamma(const TimeClass* const TimeInfo) {
  //written kgf 24/5 00
  //Formula by Hans J Skaug, 15/3 00 No weighting at present.
  //This function is scale independent.

  double totallikelihood = 0.0;
  int age, length, area;

  const AgeBandMatrixPtrVector* alptr = &aggregator->returnSum();
  for (area = 0; area < (*alptr).Size(); area++) {
    likelihoodValues[timeindex][area] = 0.0;
    if (TimeInfo->CurrentStep() == 1) { //start of a new year
      (*calc_c[area]).setElementsTo(0.0);
      (*obs_c[area]).setElementsTo(0.0);
    }

    for (age = minrow; age <= maxrow; age++) {
      for (length = mincol[age]; length <= maxcol[age]; length++) {
        (*modelDistribution[timeindex][area])[age][length] = (*alptr)[area][age][length].N;
        (*calc_c[area])[age][length] += (*modelDistribution[timeindex][area])[age][length];
        (*obs_c[area])[age][length] += (*obsDistribution[timeindex][area])[age][length];
      }
    }

    if (yearly == 0) { //calculate likelihood on all steps
      for (age = minrow; age <= maxrow; age++) {
        for (length = mincol[age]; length <= maxcol[age]; length++) {
          likelihoodValues[timeindex][area] +=
            (*obsDistribution[timeindex][area])[age][length] /
            ((*modelDistribution[timeindex][area])[age][length] + epsilon) +
            log((*modelDistribution[timeindex][area])[age][length] + epsilon);
        }
      }
      totallikelihood += likelihoodValues[timeindex][area];

    } else { //calculate likelihood on year basis
      if (TimeInfo->CurrentStep() < TimeInfo->StepsInYear())
        likelihoodValues[timeindex][area] = 0.0;
      else { //last step in year, is to calc likelihood contribution
        for (age = minrow; age <= maxrow; age++) {
          for (length = mincol[age]; length <= maxcol[age]; length++) {
            likelihoodValues[timeindex][area] +=
              (*obs_c[area])[age][length] / ((*calc_c[area])[age][length] + epsilon) +
              log((*calc_c[area])[age][length] + epsilon);
          }
        }
        totallikelihood += likelihoodValues[timeindex][area];
      }
    }
  }
  return totallikelihood;
}

double CatchDistribution::calcLikLog(const TimeClass* const TimeInfo) {
  //written by kgf 23/11 98 to get a better scaling of the stocks.
  //modified by kgf 27/11 98 to sum first and then take the logarithm

  double totallikelihood = 0.0;
  int area, age, length;
  double totalmodel, totaldata, ratio;

  const AgeBandMatrixPtrVector* alptr = &aggregator->returnSum();
  for (area = 0; area < (*alptr).Size(); area++) {
    likelihoodValues[timeindex][area] = 0.0;
    totalmodel = 0.0;
    totaldata = 0.0;
    if (TimeInfo->CurrentStep() == 1) { //start of a new year
      (*calc_c[area]).setElementsTo(0.0);
      (*obs_c[area]).setElementsTo(0.0);
    }

    for (age = minrow; age <= maxrow; age++) {
      for (length = mincol[age]; length <= maxcol[age]; length++) {
        (*modelDistribution[timeindex][area])[age][length] = (*alptr)[area][age][length].N;
        (*calc_c[area])[age][length] += (*modelDistribution[timeindex][area])[age][length];
        (*obs_c[area])[age][length] += (*obsDistribution[timeindex][area])[age][length];
      }
    }

    if (yearly == 0) { //calculate likelihood on all steps
      for (age = minrow; age <= maxrow; age++) {
        for (length = mincol[age]; length <= maxcol[age]; length++) {
          totalmodel += (*modelDistribution[timeindex][area])[age][length];
          totaldata += (*obsDistribution[timeindex][area])[age][length];
        }
      }
      ratio = log(totaldata / totalmodel);
      likelihoodValues[timeindex][area] += (ratio * ratio);

    } else { //calculate likelihood on year basis
      if (TimeInfo->CurrentStep() < TimeInfo->StepsInYear())
        likelihoodValues[timeindex][area] = 0.0;
      else { //last step in year, is to calculate likelihood contribution
        for (age = minrow; age <= maxrow; age++) {
          for (length = mincol[age]; length <= maxcol[age]; length++) {
            totalmodel += (*calc_c[area])[age][length];
            totaldata += (*obs_c[area])[age][length];
          }
        }
        ratio = log(totaldata / totalmodel);
        likelihoodValues[timeindex][area] += (ratio * ratio);
      }
    }
    totallikelihood += likelihoodValues[timeindex][area];
  }
  return totallikelihood;
}

double CatchDistribution::calcLikSumSquares() {

  double totallikelihood = 0.0, temp = 0.0;
  double totalmodel, totaldata;
  int age, len, area;

  const AgeBandMatrixPtrVector* alptr = &aggregator->returnSum();
  for (area = 0; area < areas.Nrow(); area++) {
    totalmodel = 0.0;
    totaldata = 0.0;
    likelihoodValues[timeindex][area] = 0.0;
    for (age = (*alptr)[area].minAge(); age <= (*alptr)[area].maxAge(); age++) {
      for (len = (*alptr)[area].minLength(age); len < (*alptr)[area].maxLength(age); len++) {
        (*modelDistribution[timeindex][area])[age][len] = ((*alptr)[area][age][len]).N;
        totalmodel += (*modelDistribution[timeindex][area])[age][len];
        totaldata += (*obsDistribution[timeindex][area])[age][len];
      }
    }

    for (age = (*alptr)[area].minAge(); age <= (*alptr)[area].maxAge(); age++) {
      for (len = (*alptr)[area].minLength(age); len < (*alptr)[area].maxLength(age); len++) {
        if ((isZero(totaldata)) && (isZero(totalmodel)))
          temp = 0.0;
        else if (isZero(totaldata))
          temp = (*modelDistribution[timeindex][area])[age][len] / totalmodel;
        else if (isZero(totalmodel))
          temp = (*obsDistribution[timeindex][area])[age][len] / totaldata;
        else
          temp = (((*obsDistribution[timeindex][area])[age][len] / totaldata)
            - ((*modelDistribution[timeindex][area])[age][len] / totalmodel));

        likelihoodValues[timeindex][area] += (temp * temp);
      }
    }
    totallikelihood += likelihoodValues[timeindex][area];
  }
  return totallikelihood;
}

void CatchDistribution::Correlation() {
  int i, j, l, p;
  p = aggregator->numLengthGroups();
  DoubleMatrix correlation(p, p, 0.0);

  for (i = 0; i < lag; i++)
    if (abs(params[i] - 1.0) >= 1.0)
      illegal = 1;

  if (illegal == 0) {
    for (i = 0; i < p; i++) {
      for (j = 0; j <= i; j++) {
        for (l = 1; l <= lag; l++) {
          if ((i - l) >= 0) {
            correlation[i][j] += (params[l - 1] - 1.0) * correlation[i - l][j];
            correlation[j][i] += (params[l - 1] - 1.0) * correlation[i - l][j];
          }
        }
      }
      correlation[i][i] += sigma * sigma;
    }
    LU = LUDecomposition(correlation);
  }
}


double CatchDistribution::calcLikMVNormal() {

  double totallikelihood = 0.0;
  double sumdata, sumdist;
  int age, len, area;
  int i, j, p;

  const AgeBandMatrixPtrVector* alptr = &aggregator->returnSum();
  DoubleVector diff(aggregator->numLengthGroups(), 0.0);

  for (area = 0; area < areas.Nrow(); area++) {
    sumdata = 0.0;
    sumdist = 0.0;
    for (age = (*alptr)[area].minAge(); age <= (*alptr)[area].maxAge(); age++) {
      for (len = (*alptr)[area].minLength(age); len < (*alptr)[area].maxLength(age); len++) {
        (*modelDistribution[timeindex][area])[age][len] = ((*alptr)[area][age][len]).N;
        sumdata += (*obsDistribution[timeindex][area])[age][len];
        sumdist += (*modelDistribution[timeindex][area])[age][len];
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

    for (age = (*alptr)[area].minAge(); age <= (*alptr)[area].maxAge(); age++) {
      for (len = 0; len < diff.Size(); len++)
        diff[len] = 0.0;

      for (len = (*alptr)[area].minLength(age); len < (*alptr)[area].maxLength(age); len++)
        diff[len] = ((*obsDistribution[timeindex][area])[age][len] * sumdata)
                    - ((*modelDistribution[timeindex][area])[age][len] * sumdist);

      totallikelihood += diff * LU.Solve(diff);
    }
  }

  if (isZero(sigma)) {
    handle.logWarning("Warning in catchdistribution - multivariate normal sigma is zero");
    return verybig;
  }

  totallikelihood += LU.LogDet() * alptr->Size();
  return totallikelihood;
}

double CatchDistribution::calcLikMVLogistic() {

  double totallikelihood = 0.0;
  double sumdata = 0.0, sumdist = 0.0, sumnu = 0.0;
  int age, len, area, p;

  const AgeBandMatrixPtrVector* alptr = &aggregator->returnSum();
  p = aggregator->numLengthGroups();
  DoubleVector nu(p, 0.0);

  for (area = 0; area < areas.Nrow(); area++) {
    for (age = (*alptr)[area].minAge(); age <= (*alptr)[area].maxAge(); age++) {
      for (len = 0; len < nu.Size(); len++)
        nu[len] = 0.0;

      sumdata = 0.0;
      sumdist = 0.0;
      for (len = (*alptr)[area].minLength(age); len < (*alptr)[area].maxLength(age); len++) {
        (*modelDistribution[timeindex][area])[age][len] = ((*alptr)[area][age][len]).N;
        sumdata += (*obsDistribution[timeindex][area])[age][len];
        sumdist += (*modelDistribution[timeindex][area])[age][len];
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
      for (len = (*alptr)[area].minLength(age); len < (*alptr)[area].maxLength(age); len++) {
        nu[len] = log(((*obsDistribution[timeindex][area])[age][len] * sumdata) + verysmall)
                  - log(((*modelDistribution[timeindex][area])[age][len] * sumdist) + verysmall);

        sumnu += nu[len];
      }
      sumnu = sumnu / p;

      for (len = (*alptr)[area].minLength(age); len < (*alptr)[area].maxLength(age); len++)
        totallikelihood += (nu[len] - sumnu) * (nu[len] - sumnu);
    }
  }

  if (isZero(sigma)) {
    handle.logWarning("Warning in catchdistribution - multivariate logistic sigma is zero");
    return verybig;
  }

  totallikelihood = (totallikelihood / (sigma * sigma)) + (log(sigma) * (p - 1));
  return totallikelihood;
}

void CatchDistribution::SummaryPrint(ofstream& outfile) {
  int year, area;

  for (year = 0; year < likelihoodValues.Nrow(); year++) {
    for (area = 0; area < likelihoodValues.Ncol(year); area++) {
      if (yearly == 0) {
        outfile << setw(lowwidth) << Years[year] << sep << setw(lowwidth)
          << Steps[year] << sep << setw(printwidth) << areaindex[area] << sep
          << setw(largewidth) << this->Name() << sep << setw(smallwidth) << weight
          << sep << setprecision(largeprecision) << setw(largewidth)
          << likelihoodValues[year][area] << endl;
      } else {
        if (isZero(likelihoodValues[year][area])) {
          // assume that this isnt the last step for that year and ignore
        } else {
          outfile << setw(lowwidth) << Years[year] << "  all "
            << setw(printwidth) << areaindex[area] << sep
            << setw(largewidth) << this->Name() << sep << setprecision(smallprecision)
            << setw(smallwidth) << weight << sep << setprecision(largeprecision)
            << setw(largewidth) << likelihoodValues[year][area] << endl;
        }
      }
    }
  }
  outfile.flush();
}
