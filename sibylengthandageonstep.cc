#include "areatime.h"
#include "sibylengthandageonstep.h"
#include "stockaggregator.h"
#include "errorhandler.h"
#include "readfunc.h"
#include "readword.h"
#include "sionstep.h"
#include "suitfunc.h"
#include "stock.h"
#include "stockptrvector.h"
#include "commentstream.h"
#include "gadget.h"

extern ErrorHandler handle;

/*  SIByLengthAndAgeOnStep
 *
 *  Purpose:  Constructor
 *
 *  In:  CommentStream& infile   :file to read survey indices from
 *       IntMatrix& areas        :areas
 *       DoubleVector& lengths   :lengths
 *       IntMatrix& ages         :ages
 *       TimeClass* TimeInfo     :time defenitions
 */
SIByLengthAndAgeOnStep::SIByLengthAndAgeOnStep(CommentStream& infile,
  const IntMatrix& areas, const DoubleVector& lengths,
  const IntMatrix& ages, const TimeClass* const TimeInfo, Keeper* const keeper,
  const CharPtrVector& lenindex, const CharPtrVector& ageindex,
  const CharPtrVector& areaindex, const char* datafilename, const char* name)
  : SIOnStep(infile, datafilename, areaindex, TimeInfo, 1, areas, ageindex, lenindex, name), Ages(ages) {

  keeper->addString("sibylengthandageonstep");
  LgrpDiv = new LengthGroupDivision(lengths);
  if (LgrpDiv->Error())
    handle.Message("Error in surveyindex - failed to create length group");

  int i;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  suitfunction = NULL;
  stocktype = 0;
  index = 0;

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
    handle.Message("Error in surveyindex - unrecognised suitability", text);
  }

  readWordAndVariable(infile, "epsilon", epsilon);
  //read the likelihoodtype
  readWordAndValue(infile, "likelihoodtype", text);
  if (strcasecmp(text, "pearson") == 0)
    opttype = PEARSONOPTTYPE;
  else if (strcasecmp(text, "multinomial") == 0)
    opttype = MULTINOMIALOPTTYPE;
  else if (strcasecmp(text, "gamma") == 0)
    opttype = GAMMAOPTTYPE;
  else if (strcasecmp(text, "log") == 0)
    opttype = LOGFUNCOPTTYPE;
  else
    handle.Unexpected("likelihoodtype", text);

  //first resize objects to store the data
  int numtime = Years.Size();
  for (i = 0; i < numtime; i++) {
    indexMatrix.resize(1, new DoubleMatrix(Ages.Nrow(), LgrpDiv->NoLengthGroups(), 0.0));
    calc_index.resize(1, new DoubleMatrix(Ages.Nrow(), LgrpDiv->NoLengthGroups(), 0.0));
  }
  lik_val_on_step.resize(numtime, 0.0);
  max_val_on_step.resize(numtime, 0.0);
  l_index.resize(numtime, 0);
  a_index.resize(numtime, 0);

  //then read the survey indices data from the datafile
  ifstream datafile;
  CommentStream subdata(datafile);

  datafile.open(datafilename, ios::in);
  handle.checkIfFailure(datafile, datafilename);
  handle.Open(datafilename);
  readSurveyData(subdata, areaindex, lenindex, ageindex, TimeInfo, name);
  handle.Close();
  datafile.close();
  datafile.clear();

  keeper->clearLast();
}

void SIByLengthAndAgeOnStep::readSurveyData(CommentStream& infile, const CharPtrVector& areaindex,
  const CharPtrVector& lenindex, const CharPtrVector& ageindex, const TimeClass* TimeInfo, const char* name) {

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

    //check if the year and step are in the simulation
    timeid = -1;
    if (TimeInfo->IsWithinPeriod(year, step)) {
      for (i = 0; i < Years.Size(); i++)
        if ((Years[i] == year) && (Steps[i] == step))
          timeid = i;
    } else
      keepdata = 1;

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

    if (keepdata == 0) {
      //survey indices data is required, so store it
      count++;
      (*indexMatrix[timeid])[ageid][lenid] = tmpnumber;
    }
  }
  if (count == 0)
    handle.logWarning("Warning in surveyindex - found no data in the data file for", name);
  handle.logMessage("Read surveyindex data file - number of entries", count);
}

SIByLengthAndAgeOnStep::~SIByLengthAndAgeOnStep() {
  int i;
  if (suitfunction != NULL) {
    delete suitfunction;
    suitfunction = NULL;
  }
  for (i = 0; i < indexMatrix.Size(); i++) {
    delete indexMatrix[i];
    delete calc_index[i];
  }
  for (i = 0; i < stocknames.Size(); i++)
    delete[] stocknames[i];
  if (aggregator != 0)
    delete aggregator;
  delete LgrpDiv;
}

void SIByLengthAndAgeOnStep::setFleetsAndStocks(FleetPtrVector& Fleets, StockPtrVector& Stocks) {
  int i, j;

  aggregator = new StockAggregator(Stocks, LgrpDiv, Areas, Ages);

  for (i = 0; i < Stocks.Size(); i++) {
    stocknames.resize(1);
    j = stocknames.Size() - 1;
    stocknames[j] = new char[strlen(Stocks[i]->Name()) + 1];
    strcpy(stocknames[j], Stocks[i]->Name());
  }

  stocktype = Stocks[0]->Type();
  for (i = 0; i < Stocks.Size(); i++)
    if (Stocks[i]->Type() != stocktype)
      handle.logFailure("Error in surveyindex - trying to mix stock types");

  //Limits (inclusive) for traversing the matrices.
  mincol = aggregator->getMinCol();
  maxcol = aggregator->getMaxCol();
  minrow = aggregator->getMinRow();
  maxrow = aggregator->getMaxRow();
}

void SIByLengthAndAgeOnStep::Sum(const TimeClass* const TimeInfo) {
  //written by kgf 13/10 98
  if (!(this->IsToSum(TimeInfo)))
    return;

  if (stocktype == STOCKTYPE)
    aggregator->Sum();
  else if (stocktype == LENSTOCKTYPE)
    aggregator->MeanSum();  //mortality model, aggregate mean N values
  else
    handle.logFailure("Error in surveyindex - unknown stocktype", stocktype);

  double l = 0.0;
  handle.logMessage("Calculating likelihood score for surveyindex component", this->SIName());

  //Use that the AgeBandMatrixPtrVector aggregator->returnSum returns only one element.
  const AgeBandMatrix* alptr = &(aggregator->returnSum()[0]);
  this->calcIndex(alptr, TimeInfo);
  switch(opttype) {
    case PEARSONOPTTYPE:
      l = calcLikPearson();
      break;
    case MULTINOMIALOPTTYPE:
      l = calcLikMultinomial();
      break;
    case GAMMAOPTTYPE:
      l = calcLikGamma();
      break;
    case LOGFUNCOPTTYPE:
      l = calcLikLog();
      break;
    default:
      handle.logWarning("Warning in surveyindex - unknown opttype", opttype);
      break;
  }

  handle.logMessage("The likelihood score for this component on this timestep is", l);
  likelihood += l;
  index++;
}

void SIByLengthAndAgeOnStep::calcIndex(const AgeBandMatrix* alptr, const TimeClass* const TimeInfo) {
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
  switch(this->getFitType()) {
    case LINEARFIT:
      for (age = 0; age <= alptr->maxAge(); age++)
        for (len = 0; len < LgrpDiv->NoLengthGroups(); len++)
          (*calc_index[index])[age][len] = parameters[0] * q_l[len] * ((*alptr)[age][len].N + parameters[1]);
      break;
    case POWERFIT:
      for (age = 0; age <= alptr->maxAge(); age++)
        for (len = 0; len < LgrpDiv->NoLengthGroups(); len++)
          (*calc_index[index])[age][len] = parameters[0] * q_l[len] * pow((*alptr)[age][len].N, parameters[1]);
      break;
    default:
      handle.logWarning("Warning in surveyindex - unknown fittype", this->getFitType());
      break;
  }
}

double SIByLengthAndAgeOnStep::calcLikMultinomial() {
  //written by kgf 30/10 98
  //Multinomial formula from H J Skaug

  double step_lik = 0.0;
  double i_sum = 0.0;
  double i_hat_sum = 0.0;
  double step_val = 0.0;

  int age, length;
  for (age = minrow; age <= maxrow; age++) {
    for (length = mincol[age]; length <= maxcol[age]; length++) {
      if ((*indexMatrix[index])[age][length] > epsilon) { //cell will contribute
        (*calc_index[index])[age][length] += epsilon;     //kgf 13/4 00, ref Skaug
        step_val = (*indexMatrix[index])[age][length] * log((*calc_index[index])[age][length]);
        step_lik -= step_val;
        i_sum += (*indexMatrix[index])[age][length];
        i_hat_sum += (*calc_index[index])[age][length];

        if (absolute(step_val) > absolute(max_val_on_step[index])) {
          max_val_on_step[index] = step_val;
          a_index[index] = age;
          l_index[index] = length;
        }
      }
    }
  }

  if (i_hat_sum <= 0) {
    lik_val_on_step[index] = 0.0;
    return 0.0;
  } else {
    step_lik /= i_sum;
    step_lik += log(i_hat_sum);
    lik_val_on_step[index] = step_lik;
    return step_lik;
  }
}

double SIByLengthAndAgeOnStep::calcLikPearson() {
  //written by kgf 13/10 98

  double step_lik = 0.0;
  double diff = 0.0;
  int age, length;

  for (age = minrow; age <= maxrow; age++) {
    for (length = mincol[age]; length <= maxcol[age]; length++) {
      diff = ((*calc_index[index])[age][length] - (*indexMatrix[index])[age][length]);
      diff *= diff;
      diff /= ((*calc_index[index])[age][length] + epsilon);
      step_lik += diff;
    }
  }

  lik_val_on_step[index] = step_lik;
  return step_lik;
}

double SIByLengthAndAgeOnStep::calcLikGamma() {
  //written by kgf 24/5 00
  //The gamma likelihood function as described by
  //Hans J Skaug 15/3 00, at present without internal weighting.
  //-w_i (-x_obs/x_mod - log(x_mod))

  double step_lik = 0.0;
  double cell_lik = 0.0;
  int age, length;

  for (age = minrow; age <= maxrow; age++) {
    for (length = mincol[age]; length <= maxcol[age]; length++) {
      cell_lik = (*indexMatrix[index])[age][length] / ((*calc_index[index])[age][length] + epsilon);
      cell_lik += log((*calc_index[index])[age][length] + epsilon);
      step_lik += cell_lik;
    }
  }

  lik_val_on_step[index] = step_lik;
  return step_lik;
}

double SIByLengthAndAgeOnStep::calcLikLog() {
  //corrected by kgf 27/11 98 to sum first and then take the logarithm
  double step_val = 0.0;
  double obs_i_sum = 0.0;
  double calc_i_sum = 0.0;
  int age, length;

  for (age = minrow; age <= maxrow; age++) {
    for (length = mincol[age]; length <= maxcol[age]; length++) {
      calc_i_sum += (*calc_index[index])[age][length];
      obs_i_sum += (*indexMatrix[index])[age][length];

      if (max_val_on_step[index] < (*calc_index[index])[age][length]) {
        max_val_on_step[index] = (*calc_index[index])[age][length];
        a_index[index] = age;
        l_index[index] = length;
      }
    }
  }

  step_val = log(obs_i_sum / calc_i_sum);
  step_val *= step_val;
  lik_val_on_step[index] = step_val; //kgf 19/1 99
  return step_val;
}

//Print observed and modeled survey indices for further processing by external scripts
void SIByLengthAndAgeOnStep::PrintLikelihood(ofstream& surveyfile, const TimeClass& TimeInfo, const char* name) {

  if (!AAT.AtCurrentTime(&TimeInfo))
    return;

  surveyfile.setf(ios::fixed);
  int age, length;

  //index was increased before this is called, so we subtract 1.
  DoubleMatrix& calcI = *calc_index[index - 1];
  DoubleMatrix& obsI = *indexMatrix[index - 1];

  surveyfile << "\nTime:    Year " << TimeInfo.CurrentYear() << " Step "
    << TimeInfo.CurrentStep() << "\nName:    " << name << "\nArea:    "
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
      surveyfile << obsI[age][length] << sep;
    }
    for (length = maxcol[age] + 1; length < obsI.Ncol(); length++) {
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
      surveyfile << calcI[age][length] << sep;
    }

    for (length = maxcol[age] + 1; length < obsI.Ncol(); length++) {
      surveyfile.precision(smallprecision);
      surveyfile.width(smallwidth);
      surveyfile << 0.0 << sep;
    }
    surveyfile << endl;
  }
  surveyfile.flush();
}

void SIByLengthAndAgeOnStep::PrintLikelihoodHeader(ofstream& surveyfile, const char* name) {
  int i;
  surveyfile << "Likelihood:       SurveyIndicesByAgeAndLengthOnStep\n"
    << "Function:         " << opttype << "\nCalculated every: step\n"
    << "Filter:           default\nEps:              " << epsilon
    << "\nMean indices:    " << "\nName:             "
    << name << "\nStocks:          ";
  for (i = 0; i < stocknames.Size(); i++)
    surveyfile << sep <<stocknames[i];

  surveyfile << "\nAges:             min " << Ages[minrow][0] << " max "
    << Ages[maxrow][0] << "\nLengths:          min " << LgrpDiv->minLength() << " max "
    << LgrpDiv->maxLength() << " dl " << LgrpDiv->dl()  << endl;
}

void SIByLengthAndAgeOnStep::Reset(const Keeper* const keeper) {

  likelihood = 0.0;
  index = 0;
  int i;
  for (i = 0; i < Years.Size(); i++) {
    max_val_on_step[i] = 0.0;
    lik_val_on_step[i] = 0.0;
    l_index[i] = 0;
    a_index[i] = 0;
  }
}

void SIByLengthAndAgeOnStep::LikelihoodPrint(ofstream& outfile) {
  int i;
  outfile << "Likelihood component on step\n";
  for (i = 0; i < Years.Size(); i++) {
    outfile << TAB << Years[i] << sep << Steps[i] << sep;
    outfile.width(smallwidth);
    outfile.precision(smallprecision);
    outfile << lik_val_on_step[i] << endl;
  }
  outfile << "Maximum contribution on step, age and length\n";
  for (i = 0; i < Years.Size(); i++) {
    outfile << TAB << Years[i] << sep << Steps[i] << sep;
    outfile.width(smallwidth);
    outfile.precision(smallprecision);
    outfile << max_val_on_step[i] << sep << a_index[i] << sep << l_index[i] << endl;
    }
  outfile << "Total likelihood component " << likelihood << endl;
}

void SIByLengthAndAgeOnStep::SummaryPrint(ofstream& outfile, double weight) {
  int year, area;

  //JMB - this is nasty hack since there is only one area
  for (year = 0; year < Years.Size(); year++) {
    for (area = 0; area < areanames.Size(); area++) {
      outfile << setw(lowwidth) << Years[year] << sep << setw(lowwidth)
        << Steps[year] << sep << setw(printwidth) << areanames[area] << sep
        << setw(largewidth) << this->SIName() << sep << setw(smallwidth)
        << weight << sep << setprecision(largeprecision) << setw(largewidth)
        << lik_val_on_step[year] << endl;
    }
  }
  outfile.flush();
}
