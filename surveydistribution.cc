#include "surveydistribution.h"
#include "readfunc.h"
#include "readword.h"
#include "readaggregation.h"
#include "errorhandler.h"
#include "areatime.h"
#include "stock.h"
#include "ecosystem.h"
#include "gadget.h"

extern ErrorHandler handle;

SurveyDistribution::SurveyDistribution(CommentStream& infile, const AreaClass* const Area,
  const TimeClass* const TimeInfo, Keeper* const keeper, double weight, const char* name)
  : Likelihood(SURVEYDISTRIBUTIONLIKELIHOOD, weight) {

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
  readWordAndValue(infile, "datafile", datafilename);

  index = 0;
  stocktype = 0;
  sdname = new char[strlen(name) + 1];
  strcpy(sdname, name);
  fittype = new char[MaxStrLength];
  strncpy(fittype, "", MaxStrLength);
  liketype = new char[MaxStrLength];
  strncpy(liketype, "", MaxStrLength);

  //read in area aggregation from file
  readWordAndValue(infile, "areaaggfile", aggfilename);
  datafile.open(aggfilename, ios::in);
  handle.checkIfFailure(datafile, aggfilename);
  handle.Open(aggfilename);
  numarea = readAggregation(subdata, areas, areaindex);
  handle.Close();
  datafile.close();
  datafile.clear();

  //Check if we read correct input
  if (areas.Nrow() != 1)
    handle.Message("Error - there should be only one area for the surveydistribution");

  //Must change from outer areas to inner areas.
  for (i = 0; i < areas.Nrow(); i++)
    for (j = 0; j < areas.Ncol(i); j++)
      if ((areas[i][j] = Area->InnerArea(areas[i][j])) == -1)
        handle.UndefinedArea(areas[i][j]);

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

  //read in age aggregation from file
  readWordAndValue(infile, "ageaggfile", aggfilename);
  datafile.open(aggfilename, ios::in);
  handle.checkIfFailure(datafile, aggfilename);
  handle.Open(aggfilename);
  numage = readAggregation(subdata, ages, ageindex);
  handle.Close();
  datafile.close();
  datafile.clear();

  //read in the stocknames
  i = 0;
  infile >> text >> ws;
  if (!(strcasecmp(text, "stocknames") == 0))
    handle.Unexpected("stocknames", text);
  infile >> text;
  while (!infile.eof() && !(strcasecmp(text, "fittype") == 0)) {
    infile >> ws;
    stocknames.resize(1);
    stocknames[i] = new char[strlen(text) + 1];
    strcpy(stocknames[i++], text);
    infile >> text;
  }

  infile >> fittype >> ws;
  fitnumber = 0;
  if (strcasecmp(fittype, "linearfit") == 0) {
    fitnumber = 1;
  } else if (strcasecmp(fittype, "powerfit") == 0) {
    fitnumber = 2;

  } else
    handle.Message("Error in surveydistribution - unrecognised fittype", fittype);

  parameters.resize(2, keeper);
  infile >> text >> ws;
  if (strcasecmp(text, "parameters") == 0)
    parameters.read(infile, TimeInfo, keeper);
  else
    handle.Unexpected("parameters", text);

  q_l.resize(LgrpDiv->NoLengthGroups(), 0.0);
  infile >> text >> ws;
  if ((strcasecmp(text, "function") == 0)) {
    infile >> text;
    SuitFuncPtrVector tempsuitfunc;
    if (readSuitFunction(tempsuitfunc, infile, text, TimeInfo, keeper)) {
      suitfunction = tempsuitfunc[0];
      if (suitfunction->usesPredLength())
        suitfunction->setPredLength(0.0);

      for (i = 0; i < LgrpDiv->NoLengthGroups(); i++) {
        if (suitfunction->usesPreyLength())
          suitfunction->setPreyLength(LgrpDiv->meanLength(i));

        q_l[i] = suitfunction->calculate();
      }
    }

  } else if (strcasecmp(text, "suitfile") == 0) {
    //read values from file
    for (i = 0; i < LgrpDiv->NoLengthGroups(); i++)
      infile >> q_l[i];

  } else {
    handle.Message("Error in surveydistribution - unrecognised suitability", text);
  }

  readWordAndVariable(infile, "epsilon", epsilon);
  readWordAndValue(infile, "likelihoodtype", liketype);
  likenumber = 0;
  if (strcasecmp(liketype, "pearson") == 0)
    likenumber = 1;
  else if (strcasecmp(liketype, "multinomial") == 0)
    likenumber = 2;
  else if (strcasecmp(liketype, "gamma") == 0)
    likenumber = 3;
  else if (strcasecmp(liketype, "log") == 0)
    likenumber = 4;
  else
    handle.Unexpected("likelihoodtype", liketype);

  //read the survey distribution data from the datafile
  datafile.open(datafilename, ios::in);
  handle.checkIfFailure(datafile, datafilename);
  handle.Open(datafilename);
  readDistributionData(subdata, TimeInfo, numarea, numage, numlen);
  handle.Close();
  datafile.close();
  datafile.clear();

  //prepare for next likelihood component
  infile >> ws;
  if (!infile.eof()) {
    infile >> text >> ws;
    if (!(strcasecmp(text, "[component]") == 0))
      handle.Unexpected("[component]", text);
  }
}

void SurveyDistribution::readDistributionData(CommentStream& infile,
  const TimeClass* TimeInfo, int numarea, int numage, int numlen) {

  int i;
  int year, step;
  double tmpnumber;
  char tmparea[MaxStrLength], tmpage[MaxStrLength], tmplen[MaxStrLength];
  strncpy(tmparea, "", MaxStrLength);
  strncpy(tmpage, "", MaxStrLength);
  strncpy(tmplen, "", MaxStrLength);
  int keepdata, timeid, areaid, ageid, lenid;
  int count = 0;

  //Check the number of columns in the inputfile
  if (countColumns(infile) != 6)
    handle.Message("Wrong number of columns in inputfile - should be 6");

  while (!infile.eof()) {
    keepdata = 0;
    infile >> year >> step >> tmparea >> tmpage >> tmplen >> tmpnumber >> ws;

    //if tmparea is in areaindex keep data, else dont keep the data
    areaid = -1;
    for (i = 0; i < areaindex.Size(); i++)
      if (strcasecmp(areaindex[i], tmparea) == 0)
        areaid = i;

    if (areaid == -1)
      keepdata = 1;

    //if tmpage is in ageindex keep data, else dont keep the data
    ageid = -1;
    for (i = 0; i < ageindex.Size(); i++)
      if (strcasecmp(ageindex[i], tmpage) == 0)
        ageid = i;

    if (ageid == -1)
      keepdata = 1;

    //if tmplen is in lenindex keep data, else dont keep the data
    lenid = -1;
    for (i = 0; i < lenindex.Size(); i++)
      if (strcasecmp(lenindex[i], tmplen) == 0)
        lenid = i;

    if (ageid == -1)
      keepdata = 1;

    //check if the year and step are in the simulation
    timeid = -1;
    if ((TimeInfo->IsWithinPeriod(year, step)) && (keepdata == 0)) {
      for (i = 0; i < Years.Size(); i++)
        if ((Years[i] == year) && (Steps[i] == step))
          timeid = i;

      if (timeid == -1) {
        Years.resize(1, year);
        Steps.resize(1, step);
        timeid = (Years.Size() - 1);

        obsDistribution.resize(1, new DoubleMatrix(numage, numlen, 0.0));
        modelDistribution.resize(1, new DoubleMatrix(numage, numlen, 0.0));
        likelihoodValues.resize(1, 0.0);
      }

    } else
      keepdata = 1;


    if (keepdata == 0) {
      //survey distribution data is required, so store it
      count++;
      (*obsDistribution[timeid])[ageid][lenid] = tmpnumber;
    }
  }

  AAT.addActions(Years, Steps, TimeInfo);
  if (count == 0)
    handle.logWarning("Warning in surveydistribution - found no data in the data file for", sdname);
  handle.logMessage("Read surveydistribution data file - number of entries", count);
}

SurveyDistribution::~SurveyDistribution() {
  int i;
  for (i = 0; i < stocknames.Size(); i++)
    delete[] stocknames[i];
  for (i = 0; i < areaindex.Size(); i++)
    delete[] areaindex[i];
  for (i = 0; i < ageindex.Size(); i++)
    delete[] ageindex[i];
  for (i = 0; i < lenindex.Size(); i++)
    delete[] lenindex[i];

  if (suitfunction != NULL) {
    delete suitfunction;
    suitfunction = NULL;
  }
  for (i = 0; i < obsDistribution.Size(); i++) {
    delete obsDistribution[i];
    delete modelDistribution[i];
  }

  if (aggregator != 0)
    delete aggregator;
  delete LgrpDiv;
  delete[] sdname;
  delete[] fittype;
  delete[] liketype;
}

void SurveyDistribution::Reset(const Keeper* const keeper) {
  index = 0;
  Likelihood::Reset(keeper);
  handle.logMessage("Reset surveydistribution component", sdname);
}

void SurveyDistribution::Print(ofstream& outfile) const {
  int i;
  outfile << "\nSurvey Distribution " << sdname << " - likelihood value " << likelihood
    << "\n\tFunction " << liketype << "\n\tStock names:";
  for (i = 0; i < stocknames.Size(); i++)
    outfile << sep << stocknames[i];
  outfile << endl;
  aggregator->Print(outfile);
  outfile.flush();
}

void SurveyDistribution::LikelihoodPrint(ofstream& outfile) {
  int i, j, year;
  
  outfile << "\nSurvey Distribution " << sdname << "\n\nLikelihood " << likelihood
    << "\nWeight " << weight << "\nStock names";
  for (i = 0; i < stocknames.Size(); i++)
    outfile << sep << stocknames[i];
  outfile << "\nInternal areas";
  for (i = 0; i < areas.Nrow(); i++) {
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
  outfile << endl;

  outfile << "\nSurvey distribution data:\n";
  for (year = 0; year < Years.Size(); year++) {
    outfile << "\nYear " << Years[year] << " and step " << Steps[year] << "\nObserved index:";
    for (i = 0; i < (*obsDistribution[year]).Nrow(); i++) {
      outfile << endl;
      for (j = 0; j < (*obsDistribution[year]).Ncol(i); j++) {
        outfile.width(smallwidth);
        outfile.precision(smallprecision);
        outfile << sep << (*obsDistribution[year])[i][j];
      }
    }
    outfile << "\nModelled index:";
    for (i = 0; i < (*modelDistribution[year]).Nrow(); i++) {
      outfile << endl;
      for (j = 0; j < (*modelDistribution[year]).Ncol(i); j++) {
        outfile.width(smallwidth);
        outfile.precision(smallprecision);
        outfile << sep << (*modelDistribution[year])[i][j];
      }
    }
    outfile << "\nLikelihood value: ";
    outfile.width(smallwidth);
    outfile.precision(smallprecision);
    outfile << likelihoodValues[year] << endl;
  }
  outfile.flush();
}

void SurveyDistribution::setFleetsAndStocks(FleetPtrVector& Fleets, StockPtrVector& Stocks) {

  int i, j;
  int found = 0;
  StockPtrVector stocks;

  for (i = 0; i < stocknames.Size(); i++) {
    found = 0;
    for (j = 0; j < Stocks.Size(); j++)
      if (strcasecmp(stocknames[i], Stocks[j]->Name()) == 0) {
        found++;
        stocks.resize(1, Stocks[j]);
      }
    if (found == 0)
      handle.logFailure("Error in surveydistribution - failed to match stock", stocknames[i]);

  }

  stocktype = stocks[0]->Type();
  for (i = 0; i < stocks.Size(); i++)
    if (stocks[i]->Type() != stocktype)
      handle.logFailure("Error in surveydistribution - trying to mix stock types");

  aggregator = new StockAggregator(stocks, LgrpDiv, areas, ages);

  //Limits (inclusive) for traversing the matrices.
  mincol = aggregator->getMinCol();
  maxcol = aggregator->getMaxCol();
  minrow = aggregator->getMinRow();
  maxrow = aggregator->getMaxRow();
}

void SurveyDistribution::calcIndex(const AgeBandMatrix* alptr, const TimeClass* const TimeInfo) {
  //written by kgf 13/10 98

  int i, age, len;
  if (suitfunction != NULL) {
    suitfunction->updateConstants(TimeInfo);
    if ((index == 0) || (suitfunction->constantsHaveChanged(TimeInfo))) {
      if (suitfunction->usesPredLength())
        suitfunction->setPredLength(0.0);

      for (i = 0; i < q_l.Size(); i++) {
        if (suitfunction->usesPreyLength())
          suitfunction->setPreyLength(LgrpDiv->meanLength(i));

        q_l[i] = suitfunction->calculate();
      }
    }
  }

  parameters.Update(TimeInfo);
  switch(fitnumber) {
    case 1:
      for (age = 0; age <= alptr->maxAge(); age++)
        for (len = 0; len < LgrpDiv->NoLengthGroups(); len++)
          (*modelDistribution[index])[age][len] = parameters[0] * q_l[len] * ((*alptr)[age][len].N + parameters[1]);
      break;
    case 2:
      for (age = 0; age <= alptr->maxAge(); age++)
        for (len = 0; len < LgrpDiv->NoLengthGroups(); len++)
          (*modelDistribution[index])[age][len] = parameters[0] * q_l[len] * pow((*alptr)[age][len].N, parameters[1]);
      break;
    default:
      handle.logWarning("Warning in surveydistribution - unknown fittype", fittype);
      break;
  }
}

void SurveyDistribution::addLikelihood(const TimeClass* const TimeInfo) {

  if (!(AAT.AtCurrentTime(TimeInfo)))
    return;

  if (stocktype == STOCKTYPE)
    aggregator->Sum();
  else if (stocktype == LENSTOCKTYPE)
    aggregator->MeanSum();  //mortality model, aggregate mean N values
  else
    handle.logFailure("Error in surveydistribution - unknown stocktype", stocktype);

  double l = 0.0;
  handle.logMessage("Calculating likelihood score for surveydistribution component", sdname);

  //Use that the AgeBandMatrixPtrVector aggregator->returnSum returns only one element.
  const AgeBandMatrix* alptr = &(aggregator->returnSum()[0]);
  this->calcIndex(alptr, TimeInfo);
  switch(likenumber) {
    case 1:
      l = calcLikPearson();
      break;
    case 2:
      l = calcLikMultinomial();
      break;
    case 3:
      l = calcLikGamma();
      break;
    case 4:
      l = calcLikLog();
      break;
    default:
      handle.logWarning("Warning in surveydistribution - unknown opttype", liketype);
      break;
  }

  handle.logMessage("The likelihood score for this component on this timestep is", l);
  likelihood += l;
  index++;
}

double SurveyDistribution::calcLikMultinomial() {
  //written by kgf 30/10 98
  //Multinomial formula from H J Skaug

  double total = 0.0, obstotal = 0.0, modtotal = 0.0;
  int age, length;

  for (age = minrow; age <= maxrow; age++) {
    for (length = mincol[age]; length <= maxcol[age]; length++) {
      if ((*obsDistribution[index])[age][length] > epsilon) { //cell will contribute
        total -= (*obsDistribution[index])[age][length] * 
                 log(((*modelDistribution[index])[age][length]) + epsilon);
        obstotal += (*obsDistribution[index])[age][length];
        modtotal += ((*modelDistribution[index])[age][length] + epsilon);
      }
    }
  }

  if ((modtotal <= 0) && (!(isZero(obstotal)))) {
    likelihoodValues[index] = 0.0;
    return 0.0;
  } else {
    total /= obstotal;
    total += log(modtotal);
    likelihoodValues[index] = total;
    return total;
  }
}

double SurveyDistribution::calcLikPearson() {
  //written by kgf 13/10 98

  double total = 0.0, diff = 0.0;
  int age, length;

  for (age = minrow; age <= maxrow; age++) {
    for (length = mincol[age]; length <= maxcol[age]; length++) {
      diff = ((*modelDistribution[index])[age][length] - (*obsDistribution[index])[age][length]);
      diff *= diff;
      diff /= ((*modelDistribution[index])[age][length] + epsilon);
      total += diff;
    }
  }

  likelihoodValues[index] = total;
  return total;
}

double SurveyDistribution::calcLikGamma() {
  //written by kgf 24/5 00
  //The gamma likelihood function as described by
  //Hans J Skaug 15/3 00, at present without internal weighting.
  //-w_i (-x_obs/x_mod - log(x_mod))

  double total = 0.0, temp = 0.0;
  int age, length;

  for (age = minrow; age <= maxrow; age++) {
    for (length = mincol[age]; length <= maxcol[age]; length++) {
      temp = ((*obsDistribution[index])[age][length] /
               ((*modelDistribution[index])[age][length] + epsilon)) +
               log((*modelDistribution[index])[age][length] + epsilon);

      total += temp;
    }
  }

  likelihoodValues[index] = total;
  return total;
}

double SurveyDistribution::calcLikLog() {
  //corrected by kgf 27/11 98 to sum first and then take the logarithm
  double total = 0.0, obstotal = 0.0, modtotal = 0.0;
  int age, length;

  for (age = minrow; age <= maxrow; age++) {
    for (length = mincol[age]; length <= maxcol[age]; length++) {
      modtotal += (*modelDistribution[index])[age][length];
      obstotal += (*obsDistribution[index])[age][length];
    }
  }

  if (!(isZero(modtotal))) {
    total = log(obstotal / modtotal);
    total *= total;
  } else
    total = verybig;
    
  likelihoodValues[index] = total; //kgf 19/1 99
  return total;
}

void SurveyDistribution::PrintLikelihoodHeader(ofstream& surveyfile) {

  int i;
  surveyfile << "Likelihood:       SurveyDistribution\n"
    << "Function:         " << liketype << "\nCalculated every: step\n"
    << "Filter:           default\nEps:              " << epsilon
    << "\nMean indices:    " << "\nName:             " << sdname << "\nStocks:          ";

  for (i = 0; i < stocknames.Size(); i++)
    surveyfile << sep <<stocknames[i];

  surveyfile << "\nages:             min " << ages[minrow][0] << " max "
    << ages[maxrow][0] << "\nLengths:          min " << LgrpDiv->minLength() << " max "
    << LgrpDiv->maxLength() << " dl " << LgrpDiv->dl()  << endl;
}

//Print Observed and survey indices for further processing by external scripts
void SurveyDistribution::PrintLikelihood(ofstream& surveyfile, const TimeClass& TimeInfo) {

  if ((!AAT.AtCurrentTime(&TimeInfo)) || (index == 0))
    return;

  surveyfile.setf(ios::fixed);
  int age, length, printindex;

  //index was increased before this is called, so we subtract 1
  printindex = index - 1;

  surveyfile << "\nTime:    Year " << TimeInfo.CurrentYear() << " Step "
    << TimeInfo.CurrentStep() << "\nName:    " << sdname << "\nArea:    "
    << 0 << "\nObserved:\n";

  for (age = minrow; age <= maxrow; age++) {
    for (length = 0; length < mincol[age]; length++) {
      surveyfile.precision(smallprecision);
      surveyfile.width(smallwidth);
      surveyfile << 0.0 << sep;
    }
    for (length = mincol[age]; length <= maxcol[age]; length++) {
      surveyfile.precision(smallprecision);
      surveyfile.width(smallwidth);
      surveyfile << (*obsDistribution[printindex])[age][length] << sep;
    }
    for (length = maxcol[age] + 1; length < (*obsDistribution[printindex]).Ncol(); length++) {
      surveyfile.precision(smallprecision);
      surveyfile.width(smallwidth);
      surveyfile << 0.0 << sep;
    }
    surveyfile << endl;
  }

  surveyfile << "Modelled:\n";
  for (age = minrow; age <= maxrow; age++) {
    for (length = 0; length < mincol[age]; length++) {
      surveyfile.precision(smallprecision);
      surveyfile.width(smallwidth);
      surveyfile << 0.0 << sep;
    }
    for (length = mincol[age]; length <= maxcol[age]; length++) {
      surveyfile.precision(smallprecision);
      surveyfile.width(smallwidth);
      surveyfile << (*modelDistribution[printindex])[age][length] << sep;
    }

    for (length = maxcol[age] + 1; length < (*obsDistribution[printindex]).Ncol(); length++) {
      surveyfile.precision(smallprecision);
      surveyfile.width(smallwidth);
      surveyfile << 0.0 << sep;
    }
    surveyfile << endl;
  }
  surveyfile.flush();
}

void SurveyDistribution::SummaryPrint(ofstream& outfile) {
  int year, area;

  //JMB - this is nasty hack since there is only one area
  for (year = 0; year < Years.Size(); year++) {
    for (area = 0; area < areaindex.Size(); area++) {
      outfile << setw(lowwidth) << Years[year] << sep << setw(lowwidth)
        << Steps[year] << sep << setw(printwidth) << areaindex[area] << sep
        << setw(largewidth) << sdname << sep << setw(smallwidth)
        << weight << sep << setprecision(largeprecision) << setw(largewidth)
        << likelihoodValues[year] << endl;
    }
  }
  outfile.flush();
}
