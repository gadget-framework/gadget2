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

/*  SIByLengthAndAgeOnStep
 *
 *  Purpose:  Constructor
 *
 *  In:  CommentStream& infile   :file to read survey indices from
 *       intvector& areas        :areas
 *       doublevector& lengths   :lengths
 *       intmatrix& ages         :ages
 *       TimeClass* TimeInfo     :time defenitions
 */
SIByLengthAndAgeOnStep::SIByLengthAndAgeOnStep(CommentStream& infile,
  const intvector& areas, const doublevector& lengths,
  const intmatrix& ages, const TimeClass* const TimeInfo, Keeper* const keeper,
  const charptrvector& lenindex, const charptrvector& ageindex,
  const char* arealabel, const char* datafilename)
  : SIOnStep(infile, datafilename, arealabel, TimeInfo, 1, ageindex, lenindex),  //reads fit type and years/steps
   aggregator(0), LgrpDiv(0), Areas(areas), Ages(ages) {

  keeper->AddString("SIByLengthAndAgeOnStep");
  LgrpDiv = new LengthGroupDivision(lengths);
  if (LgrpDiv->Error())
    LengthGroupPrintError(lengths, "survey indices by length and age");

  int i;
  ErrorHandler handle;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  suitfunction = NULL;

//read in q_y, b and q_l data
  infile >> text;
  if (!(strcasecmp(text, "yearfactor") == 0))
    handle.Unexpected("yearfactor", text);

  int firsty = 0;
  while (Years[0] != YearsInFile[firsty])
    firsty++;

  int lasty = YearsInFile.Size() - 1;
  while (Years[Years.Size() - 1] != YearsInFile[lasty])
    lasty--;

  char dummy[MaxStrLength];
  strncpy(dummy, "", MaxStrLength);

  //Skip the first yearfactors that are outside the simulation.
  for (i = 0; i < firsty; i++)
    infile >> dummy;

  //Read the yearfactors we will keep.
  q_y.resize(lasty + 1 - firsty, keeper);
  infile >> q_y;

  //Skip the remaining yearfactor
  for (i = lasty; i < YearsInFile.Size() - 1; i++)
    infile >> dummy;

  q_y.Inform(keeper);
  b.resize(2, keeper);
  infile >> b; //power or linear term
  b.Inform(keeper);

  q_l.resize(LgrpDiv->NoLengthGroups());
  infile >> text >> ws;
  if ((strcasecmp(text, "function") == 0)) {
    infile >> text;
    SuitfuncPtrvector tempsuitfunc;
    if (readSuitFunction(tempsuitfunc, infile, text, TimeInfo, keeper)) {
      suitfunction = tempsuitfunc[0];
      if (suitfunction->usesPredLength())
        suitfunction->setPredLength(0.0);

      for (i = 0; i < LgrpDiv->NoLengthGroups(); i++) {
        if (suitfunction->usesPreyLength())
          suitfunction->setPreyLength(LgrpDiv->Meanlength(i));

        q_l[i] = suitfunction->calculate();
      }
    }

  } else if (strcasecmp(text, "suitfile") == 0) {
    //read values from file
    for (i = 0; i < LgrpDiv->NoLengthGroups(); i++)
      infile >> q_l[i];

  } else {
    handle.Message("Error in SIByLengthAndAge - unrecognised suitability", text);
  }
//end of code to read in q_y, b and q_l data

  ReadWordAndVariable(infile, "eps_ind", eps_ind);
  ReadWordAndVariable(infile, "mean_fact", mean_fact);
  ReadWordAndVariable(infile, "max_fact", max_fact);

  //read the likelihoodtype
  ReadWordAndValue(infile, "likelihoodtype", text);
  if (strcasecmp(text, "pearson") == 0)
    opttype = pearson;
  else if (strcasecmp(text, "multinomial") == 0)
    opttype = multinomial;
  else if (strcasecmp(text, "experimental") == 0)
    opttype = experimental;
  else if (strcasecmp(text, "gamma") == 0)
    opttype = gamma;
  else if (strcasecmp(text, "log") == 0)
    opttype = logfunc;
  else
    handle.Unexpected("likelihoodtype", text);

  //first resize indexMatrix to store the data
  for (i = 0; i < q_y.Size(); i++)
    indexMatrix.resize(1, new doublematrix(Ages.Nrow(), LgrpDiv->NoLengthGroups(), 0.0));

  //then read the survey indices data from the datafile
  ifstream datafile;
  CommentStream subdata(datafile);

  datafile.open(datafilename);
  CheckIfFailure(datafile, datafilename);
  handle.Open(datafilename);
  ReadSurveyData(subdata, arealabel, lenindex, ageindex, TimeInfo);
  handle.Close();
  datafile.close();
  datafile.clear();

  index = 0;
  for (i = 0; i < indexMatrix.Size(); i++)
    calc_index.resize(1, new doublematrix(Ages.Nrow(), LgrpDiv->NoLengthGroups(), 0.0));
  lik_val_on_step.resize(Years.Size(), 0.0);
  max_val_on_step.resize(Years.Size(), 0.0);
  l_index.resize(Years.Size(), 0);
  a_index.resize(Years.Size(), 0);
  b_vec.resize(LgrpDiv->NoLengthGroups());
  for (i = 0; i < LgrpDiv->NoLengthGroups(); i++) //Nakken's method
    b_vec[i] = b[0] * exp(-b[1] * LgrpDiv->Meanlength(i));

  keeper->ClearLast();
}

void SIByLengthAndAgeOnStep::ReadSurveyData(CommentStream& infile, const char* arealabel,
  const charptrvector& lenindex, const charptrvector& ageindex, const TimeClass* TimeInfo) {

  int i;
  int year, step;
  double tmpnumber;
  char tmparea[MaxStrLength], tmpage[MaxStrLength], tmplen[MaxStrLength];
  strncpy(tmparea, "", MaxStrLength);
  strncpy(tmpage, "", MaxStrLength);
  strncpy(tmplen, "", MaxStrLength);
  int keepdata, timeid, ageid, lenid;
  ErrorHandler handle;

  //Check the number of columns in the inputfile
  if (CountColumns(infile) != 6)
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

    //if tmparea matches strlabel keep data, else dont keep the data
    if (!(strcasecmp(arealabel, tmparea) == 0))
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
      (*indexMatrix[timeid])[ageid][lenid] = tmpnumber;
    }
  }
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

void SIByLengthAndAgeOnStep::SetStocks(const Stockptrvector& Stocks) {
  int i, j;
  intmatrix areas(1, Areas.Size());
  for (i = 0; i < Areas.Size(); i++)
    areas[0][i] = Areas[i];

  aggregator = new StockAggregator(Stocks, LgrpDiv, areas, Ages);

  for (i = 0; i < Stocks.Size(); i++) {
    stocknames.resize(1);
    j = stocknames.Size() - 1;
    stocknames[j] = new char[strlen(Stocks[i]->Name()) + 1];
    strcpy(stocknames[j], Stocks[i]->Name());
  }

  //Limits (inclusive) for traversing the matrices.
  mincol = aggregator->getMinCol();
  maxcol = aggregator->getMaxCol();
  minrow = aggregator->getMinRow();
  maxrow = aggregator->getMaxRow();
}

void SIByLengthAndAgeOnStep::Sum(const class TimeClass* const TimeInfo) {
  //written by kgf 13/10 98
  if (!(this->IsToSum(TimeInfo)))
    return;
  FitType ftype = this->getFitType();
  aggregator->MeanSum(); //aggregate mean N values in present time step
  //Use that the Agebandmatrixvector aggregator->ReturnMeanSum returns only one element.
  const Agebandmatrix* alptr = &(aggregator->ReturnMeanSum()[0]);
  this->calcIndex(alptr, ftype);
  switch(opttype) {
    case 0:
      likelihood += calcLikPearson();
      break;
    case 1:
      likelihood += calcLikMultinomial();
      break;
    case 2:
      likelihood += calcLikExperimental();
      break;
    case 3:
      likelihood += calcLikGamma();
      break;
    case 4:
      likelihood += calcLikLog();
      break;
    default:
      cerr << "Error in SIByLengthAndAge::Sum - unknown opttype\n";
      break;
  }
  index++;
}

void SIByLengthAndAgeOnStep::calcIndex(const Agebandmatrix* alptr, FitType ftype) {
  //written by kgf 13/10 98
  int age, len;
  int maxage = alptr->Maxage();
  int maxlen = LgrpDiv->NoLengthGroups();
  double q_year = q_y[index];
  switch(ftype) {
    case LinearFit:
      for (age = 0; age <= maxage; age++)
        for (len = 0; len < maxlen; len++)
          (*calc_index[index])[age][len] = b_vec[len] + q_year * q_l[len] * (*alptr)[age][len].N;
      break;
    case PowerFit:
      for (age = 0; age <= maxage; age++)
        for (len = 0; len < maxlen; len++)
          (*calc_index[index])[age][len] = q_year * q_l[len] * pow((*alptr)[age][len].N, b_vec[len]);
      break;
    default:
      cerr << "Error in SIByLengthAndAge::calcIndex - unknown fittype\n";
      break;
  }
}

double SIByLengthAndAgeOnStep::calcLikMultinomial() {
  //written by kgf 30/10 98
  //Multinomial formula from H J Skaug

  double step_lik = 0.0;
  double i_sum = 0.0;
  double i_hat_sum = 0.0;
  double min_val = 0.0;
  double step_val = 0.0;
  max_val_on_step[index] = 0.0;
  lik_val_on_step[index] = 0.0;
  a_index[index] = 0;
  l_index[index] = 0;
  int age, length;

  for (age = minrow; age <= maxrow; age++) {
    for (length = mincol[age]; length <= maxcol[age]; length++) {
      if ((*indexMatrix[index])[age][length] > eps_ind) { //cell will contribute
        (*calc_index[index])[age][length] += eps_ind;     //kgf 13/4 00, ref Skaug
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

  min_val = i_hat_sum / i_sum;
  if (min_val <= 0) {
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
  max_val_on_step[index] = 0.0;
  lik_val_on_step[index] = 0.0;
  a_index[index] = 0;
  l_index[index] = 0;
  int age, length;

  for (age = minrow; age <= maxrow; age++) {
    for (length = mincol[age]; length <= maxcol[age]; length++) {
      diff = ((*calc_index[index])[age][length] - (*indexMatrix[index])[age][length]);
      diff *= diff;
      diff /= ((*calc_index[index])[age][length] + eps_ind);
      step_lik += diff;
    }
  }

  lik_val_on_step[index] = step_lik;
  return step_lik;
}

double SIByLengthAndAgeOnStep::calcLikExperimental() {
  //written by kgf 6/3 00
  //The purpose of this function is to try different likelihood formulations
  //At the moment it is (I_obs - I_hat)^2/(I_obs+I_hat)^2

  double step_lik = 0.0;
  double num = 0.0;
  double denom = 0.0;
  double frac = 0.0;
  max_val_on_step[index] = 0.0;
  lik_val_on_step[index] = 0.0;
  a_index[index] = 0;
  l_index[index] = 0;
  int age, length;

  for (age = minrow; age <= maxrow; age++) {
    for (length = mincol[age]; length <= maxcol[age]; length++) {
      num = ((*calc_index[index])[age][length] - (*indexMatrix[index])[age][length]);
      denom = (*calc_index[index])[age][length] + (*indexMatrix[index])[age][length] + eps_ind;
      if (denom > 0) //kgf 26/6 00
        frac= num / denom;
      else
        frac = 0.0;
      step_lik += (frac*frac);
      if (frac > max_val_on_step[index]) {
        max_val_on_step[index] = frac;
        l_index[index] = length;
        a_index[index] = age;
      }
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
  max_val_on_step[index] = 0.0;
  a_index[index] = 0;
  l_index[index] = 0;
  int age, length;

  for (age = minrow; age <= maxrow; age++) {
    for (length = mincol[age]; length <= maxcol[age]; length++) {
      cell_lik = (*indexMatrix[index])[age][length] / ((*calc_index[index])[age][length] + eps_ind);
      cell_lik += log((*calc_index[index])[age][length] + eps_ind);
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
  max_val_on_step[index] = 0.0;
  a_index[index] = 0;
  l_index[index] = 0;
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

void SIByLengthAndAgeOnStep::PrintLikelihoodOnStep(ofstream& surveyfile, int print_type) {
  //written by kgf 21/11 98
  //Print unormed survey indices, eiter residuals or absolute values

  surveyfile.setf(ios::fixed);
  int age, length;
  double step_val = 0.0;
  char pre = 0;
  double diff = 0.0;
  doublematrix& calcI = *calc_index[index];
  doublematrix& obsI = *indexMatrix[index];

  if (print_type == 0) {                       //print log(I/I_hat)
    surveyfile << "log(I/I_hat) by age and length\n";
    for (age = minrow; age <= maxrow; age++) {
      for (length = mincol[age]; length <= maxcol[age]; length++) {
        if (calcI[age][length] > 0) {
          diff = (calcI[age][length] - obsI[age][length]);
          step_val = log(obsI[age][length]) - log(calcI[age][length]);
          pre = 0;
          if (absolute(diff) / (calcI[age][length] + eps_ind) >= max_fact)
            pre = '-'; //pre = '!'; removed mnaa 10.01.00
        } else
          pre = '-';
        surveyfile.precision(smallprecision);
        surveyfile.width(printwidth);
        if (pre == 0)
          surveyfile << step_val << sep;
        else if (pre == '-')
          surveyfile << "      _ ";
        else
          surveyfile << step_val << pre;
      }
      surveyfile << endl;
    }

  } else if (print_type == 2) {                //print (I_hat-I)^2/I_hat
    surveyfile << "(I_hat-I)^2/I_hat by age and length\n";
    for (age = minrow; age <= maxrow; age++) {
      for (length = mincol[age]; length <= maxcol[age]; length++) {
        if (calcI[age][length] > 0 || obsI[age][length] > 0) {
          diff = (calcI[age][length] - obsI[age][length]);
          step_val = (diff * diff) / (calcI[age][length] + eps_ind);
          pre = 0;
          if (absolute(diff) / (calcI[age][length] + eps_ind) >= max_fact)
            pre = '-'; //pre = '!'; removed mnaa 10.01.00
        } else
          pre = '-';
        surveyfile.precision(lowprecision);
        surveyfile.width(largewidth);
        if (pre == 0)
          surveyfile << step_val << sep;
        else if (pre == '-')
          surveyfile << "           _ ";
        else
          surveyfile << step_val << pre;
      }
      surveyfile << endl;
    }

  } else {                                  //print I and I_hat seperatly
    surveyfile << "I by age and length\n";
    for (age = minrow; age <= maxrow; age++) {
      for (length = mincol[age]; length <= maxcol[age]; length++) {
        step_val = obsI[age][length];
        surveyfile.precision(lowprecision);
        surveyfile.width(largewidth);
        surveyfile << step_val << sep;
      }
      surveyfile << endl;
    }
    surveyfile << "I_hat by age and length\n";
    for (age = minrow; age <= maxrow; age++) {
      for (length = mincol[age]; length <= maxcol[age]; length++) {
        step_val = calcI[age][length];
        surveyfile.precision(lowprecision);
        surveyfile.width(largewidth);
        surveyfile << step_val << sep;
      }
      surveyfile << endl;
    }
  }
  surveyfile.flush();
}

//Print observed and modeled survey indices for further processing by external scripts
void SIByLengthAndAgeOnStep::PrintLikelihood(ofstream& surveyfile, const TimeClass& TimeInfo, const char* name) {

  if (!AAT.AtCurrentTime(&TimeInfo))
    return;

  surveyfile.setf(ios::fixed);
  int age, length;

  //index was increased before this is called, so we subtract 1.
  doublematrix& calcI = *calc_index[index - 1];
  doublematrix& obsI = *indexMatrix[index - 1];

  surveyfile << "\nTime:    Year " << TimeInfo.CurrentYear() << " Step "
    << TimeInfo.CurrentStep() << "\nName:    " << name << "\nArea:    "
    << 0 << "\nObserved:\n";

  for (age = minrow; age <= maxrow; age++) {
    for (length = 0; length < mincol[age]; length++) {
      surveyfile.precision(lowprecision);
      surveyfile.width(largewidth);
      surveyfile << 0.0 << sep;
    }
    for (length = mincol[age]; length <= maxcol[age]; length++) {
      surveyfile.precision(smallprecision);
      surveyfile.width(largewidth);
      surveyfile << obsI[age][length] << sep;
    }
    for (length = maxcol[age] + 1; length < obsI.Ncol(); length++) {
      surveyfile.precision(lowprecision);
      surveyfile.width(largewidth);
      surveyfile << 0.0 << sep;
    }
    surveyfile << endl;
  }

  surveyfile << "Modeled:\n";
  for (age = minrow; age <= maxrow; age++) {
    for (length = 0; length < mincol[age]; length++) {
      surveyfile.precision(lowprecision);
      surveyfile.width(largewidth);
      surveyfile << 0.0 << sep;
    }
    for (length = mincol[age]; length <= maxcol[age]; length++) {
      surveyfile.precision(smallprecision);
      surveyfile.width(largewidth);
      surveyfile << calcI[age][length] << sep;
    }

    for (length = maxcol[age] + 1; length < obsI.Ncol(); length++) {
      surveyfile.precision(lowprecision);
      surveyfile.width(largewidth);
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
    << "Filter:           default\nEps:              " << eps_ind
    << "\nMean fact:        " << mean_fact << "\nMax fact:         "
    << max_fact << "\nMean indices:    " << "\nName:             "
    << name << "\nStocks:          ";
  for (i = 0; i < stocknames.Size(); i++)
    surveyfile << sep <<stocknames[i];

  surveyfile << "\nAges:             min " << Ages[minrow][0] << " max "
    << Ages[maxrow][0] << "\nLengths:          min " << LgrpDiv->Minlength(0) << " max "
    << LgrpDiv->Maxlength(LgrpDiv->NoLengthGroups() - 1) << " dl " << LgrpDiv->dl()  << endl;
}

void SIByLengthAndAgeOnStep::Reset(const Keeper* const keeper) {
  int i;
  likelihood = 0.0;
  index = 0;
  for (i = 0; i < LgrpDiv->NoLengthGroups(); i++) //Nakken's method
    b_vec[i] = b[0] * exp(-b[1] * LgrpDiv->Meanlength(i));

  if (suitfunction != NULL) {
    //Fix this! Wont work if parameters are read from timevariable file.
    suitfunction->updateConstants((TimeClass*)0);
    if (suitfunction->usesPredLength())
      suitfunction->setPredLength(0.0);

    for (i = 0; i < q_l.Size(); i++) {
      if (suitfunction->usesPreyLength())
        suitfunction->setPreyLength(LgrpDiv->Meanlength(i));

      q_l[i] = suitfunction->calculate();
    }
  }

  for (i = 0; i < Years.Size(); i++) {
    max_val_on_step[i] = 0;
    l_index[i] = 0;
    a_index[i] = 0;
  }
}

void SIByLengthAndAgeOnStep::LikelihoodPrint(ofstream& outfile) const {
  int i;
  outfile << "Likelihood component on step\n";
  for (i = 0; i < Years.Size(); i++) {
    outfile << TAB << Years[i] << sep << Steps[i] << sep;
    outfile.width(printwidth);
    outfile.precision(smallprecision);
    outfile << lik_val_on_step[i] << endl;
  }
  outfile << "Maximum contribution on step, age and length\n";
  for (i = 0; i < Years.Size(); i++) {
    outfile << TAB << Years[i] << sep << Steps[i] << sep;
    outfile.width(printwidth);
    outfile.precision(smallprecision);
    outfile << max_val_on_step[i] << sep << a_index[i] << sep << l_index[i] << endl;
    }
  outfile << "Total likelihood component " << likelihood << endl;
}

void SIByLengthAndAgeOnStep::printMoreLikInfo(ofstream& outfile) const {
  //written by kgf 18/11 98 to write likelihood information on each time step
  int i;
  outfile << "Likelihood component in year\n";
  for (i = 0; i < Years.Size(); i++)
    outfile << TAB << Years[i] << sep;
  outfile << endl;
  for (i = 0; i < Years.Size(); i++)
    outfile << TAB << setiosflags(ios::showpoint)
      << setprecision(printprecision) << lik_val_on_step[i] << sep;
  outfile << endl;
}

void SIByLengthAndAgeOnStep::printHeader(ofstream& surveyfile, const PrintInfo& print, const char* name) {
  int i;
  if (print.surveyprint) {
    surveyfile << "\nsurvey " << name << "\nstocks";
    for (i = 0; i < stocknames.Size(); i++)
      surveyfile << sep << stocknames[i];

    surveyfile << "\nminage " << Ages[0][0] << " maxage " << Ages[Ages.Nrow() - 1][0]
      << " minlen " << LgrpDiv->Minlength(0) << " maxlen "
      << LgrpDiv->Maxlength(LgrpDiv->NoLengthGroups() - 1) << " lenstep "
      << LgrpDiv->dl() << endl;
  }
}

void SIByLengthAndAgeOnStep::print(ofstream& surveyfile,
  const TimeClass& time, const PrintInfo& print) {

  if (!AAT.AtCurrentTime(&time))
    return;

  if (print.surveyprint) {
    surveyfile << "year " << time.CurrentYear() << " step "
      << time.CurrentStep() << endl;
    PrintLikelihoodOnStep(surveyfile, print.surveyprint - 1);
    index++;
  }
}
