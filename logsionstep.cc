#include "areatime.h"
#include "logsionstep.h"
#include "stockaggregator.h"
#include "errorhandler.h"
#include "readfunc.h"
#include "readword.h"
#include "commentstream.h"
#include "sionstep.h"
#include "suitfunc.h"
#include "gadget.h"

/*  LogSIOnStep
 *
 *  Purpose:  Constructor
 *
 *  In: CommentStream& infile      :file to read survey indices from
 *  intvector& areas       :areas
 *  doublevector&   lengths    :lengths
 *  intmatrix& ages        :ages
 *  TimeClass* TimeInfo    :time defenitions
 */
LogSIOnStep::LogSIOnStep(CommentStream& infile,
  const intvector& areas, const doublevector& lengths,
  const intmatrix& ages, const TimeClass* const TimeInfo, Keeper* const keeper,
  const charptrvector& lenindex, const charptrvector& ageindex,
  const char* arealabel, const char* datafilename)
  : SIOnStep(infile, datafilename, arealabel, TimeInfo, lenindex, ageindex),  //reads fit type and years/steps
   aggregator(0), LgrpDiv(0), Areas(areas), Ages(ages) {

  keeper->AddString("LogSIOnStep");
  LgrpDiv = new LengthGroupDivision(lengths);
  if (LgrpDiv->Error())
    LengthGroupPrintError(lengths, " survey indices by length and age");

  int i;
  ErrorHandler handle;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);

//read in q_y, b and q_l data
  infile >> text;
  if (!(strcasecmp(text, "yearfactor") == 0))
    handle.Unexpected("yearfactor", text);

  int firsty = 0;
  while (Years[0] != YearsInFile[firsty])
    firsty++;

  int lasty = YearsInFile.Size() - 1;
  while (Years[Years.Size()-1] != YearsInFile[lasty])
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
        if (suitfunction->usesPreyLength()) {
          // AJ march 2002 Using index i not lgrp must check if OK
          suitfunction->setPreyLength(LgrpDiv->Meanlength(i));
        }
        q_l[i] = suitfunction->calculate();
      }
    }

  } else if (strcasecmp(text, "suitfile") == 0) {
    //read values from file
    for (i = 0; i < LgrpDiv->NoLengthGroups(); i++)
      infile >> q_l[i];

  } else {
    cerr << "Error while reading suitabilty - unknown format\n";
    exit(EXIT_FAILURE);
  }
//end of code to read in q_y, b and q_l data

  ReadWordAndVariable(infile, "eps_ind", eps_ind);
  ReadWordAndVariable(infile, "mean_fact", mean_fact);
  ReadWordAndVariable(infile, "max_fact", max_fact);

  //read the opttype
  infile >> text;
  if (!(strcasecmp(text, "likelihoodtype") == 0))
    handle.Unexpected("likelihoodtype", text);

  infile >> text >> ws;
  if (strcasecmp(text, "pearson") == 0)
    opttype = pearson;
  else if (strcasecmp(text, "multinomial") == 0)
    opttype = multinomial;
  else
    handle.Unexpected("opttype", text);

  //first resize indexMatrix to store the data
  for (i = 0; i < q_y.Size(); i++)
    indexMatrix.resize(1, new doublematrix(Ages.Nrow(), LgrpDiv->NoLengthGroups(), 0.0));

  //then read the survey indices data from the datafile
  ifstream datafile;
  CommentStream subdata(datafile);

  datafile.open(datafilename);
  CheckIfFailure(datafile, datafilename);
  handle.Open(datafilename);
  ReadLogSurveyData(subdata, arealabel, lenindex, ageindex, TimeInfo);
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

void LogSIOnStep::ReadLogSurveyData(CommentStream& infile, const char* arealabel,
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
  if (!(CheckColumns(infile, 6)))
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

LogSIOnStep::~LogSIOnStep() {
  if (suitfunction != NULL) {
    delete suitfunction;
    suitfunction = NULL;
  }
  delete aggregator;
  delete LgrpDiv;
}

void LogSIOnStep::SetStocks(const Stockptrvector& Stocks) {
  intmatrix areas(1, Areas.Size());
  int i;
  for (i = 0; i < Areas.Size(); i++)
    areas[0][i] = Areas[i];
  aggregator = new StockAggregator(Stocks, LgrpDiv, areas, Ages);

  //Limits (inclusive) for traversing the matrices.
  mincol = aggregator->getMinCol();
  maxcol = aggregator->getMaxCol();
  minrow = aggregator->getMinRow();
  maxrow = aggregator->getMaxRow();
}

void LogSIOnStep::Sum(const class TimeClass* const TimeInfo) {
  //written by kgf 24/11 98
  if (!(this->IsToSum(TimeInfo)))
    return;
  FitType ftype = this->getFitType();
  aggregator->MeanSum();
  //Use Agebandmatrixvector aggregator->ReturnMeanSum returns only one element
  const Agebandmatrix* alptr = &(aggregator->ReturnMeanSum()[0]);
  this->calcIndex(alptr, ftype);
  likelihood += LogLik();
  index++;
}

void LogSIOnStep::calcIndex(const Agebandmatrix* alptr, FitType ftype) {
  //written by kgf 13/10 98
  int age, len;
  int maxage = alptr->Maxage();
  int maxlen = LgrpDiv->NoLengthGroups();
  double q_year = q_y[index];
  switch (ftype) {
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
      cerr << "Error in SIByAgeAndLength:calcIndex - unknown fittype\n";
      break;
  }

  max_val_on_step[index] = 0.0;
  for (age = 0; age <= maxage; age++)
    for (len = 0; len < maxlen; len++)
      if (max_val_on_step[index] < (*calc_index[index])[age][len]) {
         max_val_on_step[index] = (*calc_index[index])[age][len];
         l_index[index] = len;
         a_index[index] = age;
      }
}

double LogSIOnStep::LogLik() {
  //corrected by kgf 27/11 98 to sum first and then take the logarithm
  double step_val = 0.0;
  double obs_i_sum = 0.0;
  double calc_i_sum = 0.0;
  int age, length;

  for (age = minrow; age <= maxrow; age++)
    for (length = mincol[age]; length <= maxcol[age]; length++) {
      calc_i_sum += (*calc_index[index])[age][length];
      obs_i_sum += (*indexMatrix[index])[age][length];
    }

  step_val = log(obs_i_sum / calc_i_sum);
  step_val *= step_val;
  lik_val_on_step[index] = step_val; //kgf 19/1 99
  return step_val;
}

void LogSIOnStep::Reset(const Keeper* const keeper) {
  //written by kgf 15/10 98
  int i;
  likelihood = 0.0;
  index = 0;
  for (i = 0; i < LgrpDiv->NoLengthGroups(); i++) //Nakken's method
    b_vec[i] = b[0] * exp(-b[1] * LgrpDiv->Meanlength(i)); //kgf 8/5 00

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

  for (i = 0; i < Years.Size(); i ++) {
    max_val_on_step[i] = 0.0;
    l_index[i] = 0;
    a_index[i] = 0;
  }
}

void LogSIOnStep::LikelihoodPrint(ofstream& outfile) const {
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

void LogSIOnStep::printMoreLikInfo(ofstream& outfile) const {
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
