#include "sionstep.h"
#include "areatime.h"
#include "errorhandler.h"
#include "readfunc.h"
#include "readword.h"
#include "loglinearregression.h"
#include "gadget.h"

extern ErrorHandler handle;

SIOnStep::~SIOnStep() {
  delete[] siname;
}

SIOnStep::SIOnStep(CommentStream& infile, const char* datafilename,
  const CharPtrVector& areaindex, const TimeClass* const TimeInfo,
  const IntMatrix& areas, const CharPtrVector& colindex,
  const char* name) : Areas(areas) {

  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);

  error = 0;
  timeindex = 0;
  slope = 0.0;
  intercept = 0.0;

  siname = new char[strlen(name) + 1];
  strcpy(siname, name);

  //read the fittype
  infile >> text;
  if (strcasecmp(text, "linearfit") == 0)
    fittype = LINEARFIT;
  else if (strcasecmp(text, "loglinearfit") == 0)
    fittype = LOGLINEARFIT;
  else if (strcasecmp(text, "fixedslopelinearfit") == 0)
    fittype = FIXEDSLOPELINEARFIT;
  else if (strcasecmp(text, "fixedslopeloglinearfit") == 0)
    fittype = FIXEDSLOPELOGLINEARFIT;
  else if (strcasecmp(text, "fixedinterceptlinearfit") == 0)
    fittype = FIXEDINTERCEPTLINEARFIT;
  else if (strcasecmp(text, "fixedinterceptloglinearfit") == 0)
    fittype = FIXEDINTERCEPTLOGLINEARFIT;
  else if (strcasecmp(text, "fixedlinearfit") == 0)
    fittype = FIXEDLINEARFIT;
  else if (strcasecmp(text, "fixedloglinearfit") == 0)
    fittype = FIXEDLOGLINEARFIT;
  else
    handle.Message("Error in surveyindex - unrecognised fittype", text);

  switch(fittype) {
    case LINEARFIT:
    case LOGLINEARFIT:
      break;
    case FIXEDSLOPELINEARFIT:
    case FIXEDSLOPELOGLINEARFIT:
      readWordAndVariable(infile, "slope", slope);
      break;
    case FIXEDINTERCEPTLINEARFIT:
    case FIXEDINTERCEPTLOGLINEARFIT:
      readWordAndVariable(infile, "intercept", intercept);
      break;
    case FIXEDLINEARFIT:
    case FIXEDLOGLINEARFIT:
      readWordAndVariable(infile, "slope", slope);
      readWordAndVariable(infile, "intercept", intercept);
      break;
    default:
      handle.Message("Error in surveyindex - unrecognised fittype");
      break;
  }

  //JMB - check that the slope of the regression line is positive
  if (slope < 0)
    handle.Message("Error in surveyindex - slope of the regression line must be positive");

  //read the survey indices data from the datafile
  ifstream datafile;
  CommentStream subdata(datafile);
  datafile.open(datafilename, ios::in);
  handle.checkIfFailure(datafile, datafilename);
  handle.Open(datafilename);
  readSIData(subdata, areaindex, colindex, TimeInfo);
  handle.Close();
  datafile.close();
  datafile.clear();

  //resize to hold the model information
  modelIndex.AddRows(obsIndex.Nrow(), obsIndex.Ncol(), 0.0);
  slopes.resize(obsIndex.Ncol());
  intercepts.resize(obsIndex.Ncol());
  sse.resize(obsIndex.Ncol());
}

void SIOnStep::readSIData(CommentStream& infile, const CharPtrVector& areaindex,
  const CharPtrVector& colindex, const TimeClass* const TimeInfo) {

  int i;
  int year, step;
  double tmpnumber;
  char tmparea[MaxStrLength], tmplabel[MaxStrLength];
  strncpy(tmparea, "", MaxStrLength);
  strncpy(tmplabel, "", MaxStrLength);
  int keepdata, timeid, colid, areaid;
  int count = 0;

  //Check the number of columns in the inputfile
  if (countColumns(infile) != 5)
    handle.Message("Wrong number of columns in inputfile - should be 5");

  while (!infile.eof()) {
    keepdata = 0;
    infile >> year >> step >> tmparea >> tmplabel >> tmpnumber >> ws;

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

    //if tmplabel is in colindex find colid, else dont keep the data
    colid = -1;
    for (i = 0; i < colindex.Size(); i++)
      if (strcasecmp(colindex[i], tmplabel) == 0)
        colid = i;

    if (colid == -1)
      keepdata = 1;

    if (keepdata == 0) {
      //survey indices data is required, so store it
      count++;
      if (timeid == -1) {
        Years.resize(1, year);
        Steps.resize(1, step);
        timeid = (Years.Size() - 1);
        obsIndex.AddRows(1, colindex.Size(), 0.0);
      }
      obsIndex[timeid][colid] = tmpnumber;
    }
  }
  AAT.addActions(Years, Steps, TimeInfo);
  if (count == 0)
    handle.logWarning("Warning in surveyindex - found no data in the data file for", this->SIName());
  handle.logMessage("Read surveyindex data file - number of entries", count);
}

void SIOnStep::Reset(const Keeper* const keeper) {
  error = 0;
  timeindex = 0;
  int i, j;
  for (i = 0; i < modelIndex.Nrow(); i++)
    for (j = 0; j < modelIndex.Ncol(i); j++)
      modelIndex[i][j] = 0.0;
  handle.logMessage("Reset surveyindex component", this->SIName());
}

void SIOnStep::Print(ofstream& outfile) const {
  int i, j;
  outfile << "\tSurvey indices\n";
  for (i = 0; i < Years.Size(); i++) {
    outfile << TAB << Years[i] << sep << Steps[i] << sep;
    for (j = 0; j < obsIndex.Ncol(i); j++) {
      outfile.width(smallwidth);
      outfile.precision(smallprecision);
      outfile << obsIndex[i][j] << sep;
    }
    outfile << endl;
  }
  outfile.flush();
}

void SIOnStep::LikelihoodPrint(ofstream& outfile) {
  int i, j;
  outfile << "Survey indices\n";
  for (i = 0; i < Years.Size(); i++) {
    outfile << TAB << Years[i] << sep << Steps[i] << sep;
    for (j = 0; j < obsIndex.Ncol(i); j++) {
      outfile.width(smallwidth);
      outfile.precision(smallprecision);
      outfile << obsIndex[i][j] << sep;
    }
    outfile << endl;
  }
  outfile << "Modelled indices\n";
  for (i = 0; i < Years.Size(); i++) {
    outfile << TAB << Years[i] << sep << Steps[i] << sep;
    for (j = 0; j < modelIndex.Ncol(i); j++) {
      outfile.width(smallwidth);
      outfile.precision(smallprecision);
      outfile << modelIndex[i][j] << sep;
    }
    outfile << endl;
  }

  if (error != 0)
    outfile << "Error " << error << endl;

  outfile << "Regression information\nintercept ";
  for (j = 0; j < intercepts.Size(); j++) {
    outfile.width(smallwidth);
    outfile.precision(smallprecision);
    outfile << intercepts[j] << sep;
  }
  outfile << "\nslope     ";
  for (j = 0; j < slopes.Size(); j++) {
    outfile.width(smallwidth);
    outfile.precision(smallprecision);
    outfile << slopes[j] << sep;
  }
  outfile << "\nsse       ";
  for (j = 0; j < sse.Size(); j++) {
    outfile.width(smallwidth);
    outfile.precision(smallprecision);
    outfile << sse[j] << sep;
  }
  outfile.flush();
}

int SIOnStep::isToSum(const TimeClass* const TimeInfo) const {
  return AAT.AtCurrentTime(TimeInfo);
}

double SIOnStep::calcRegression() {
  if (timeindex < 2)
    return 0.0;

  double score = 0.0;
  int col, index;
  for (col = 0; col < obsIndex.Ncol(); col++) {
    //Let LLR figure out what to do in the case of zero stock size.
    DoubleVector indices(timeindex);
    DoubleVector stocksize(timeindex);
    for (index = 0; index < timeindex; index++) {
      indices[index] = obsIndex[index][col];
      stocksize[index] = modelIndex[index][col];
    }
    //Now fit the log of the abundance indices as a function of stock size.
    score += this->fitRegression(stocksize, indices, col);
  }

  handle.logMessage("Calculating likelihood score for surveyindex component", this->SIName());
  handle.logMessage("The likelihood score from the regression line for this component is", score);
  return score;
}

void SIOnStep::keepNumbers(const DoubleVector& numbers) {
  int i;
  for (i = 0; i < numbers.Size(); i++)
    modelIndex[timeindex][i] = numbers[i];
  timeindex++;
}

double SIOnStep::fitRegression(const DoubleVector& stocksize, const DoubleVector& indices, int col) {

  LogLinearRegression LLR;
  LinearRegression LR;

  //fit the data to the (log) linear regression curve
  switch(fittype) {
    case LOGLINEARFIT:
      LLR.Fit(stocksize, indices);
      break;
    case FIXEDSLOPELOGLINEARFIT:
      LLR.Fit(stocksize, indices, slope);
      break;
    case FIXEDINTERCEPTLOGLINEARFIT:
      LLR.Fit(intercept, stocksize, indices);
      break;
    case FIXEDLOGLINEARFIT:
      LLR.Fit(stocksize, indices, slope, intercept);
      break;
    case LINEARFIT:
      LR.Fit(stocksize, indices);
      break;
    case FIXEDSLOPELINEARFIT:
      LR.Fit(stocksize, indices, slope);
      break;
    case FIXEDINTERCEPTLINEARFIT:
      LR.Fit(intercept, stocksize, indices);
      break;
    case FIXEDLINEARFIT:
      LR.Fit(stocksize, indices, slope, intercept);
      break;
    default:
      handle.logWarning("Warning in surveyindex - unrecognised fittype", fittype);
      break;
  }

  //and then store the results
  switch(fittype) {
    case LOGLINEARFIT:
    case FIXEDSLOPELOGLINEARFIT:
    case FIXEDINTERCEPTLOGLINEARFIT:
    case FIXEDLOGLINEARFIT:
      slopes[col] = LLR.slope();
      intercepts[col] = LLR.intersection();
      sse[col] = LLR.SSE();
      if (LLR.Error())
        this->setError();
      return LLR.SSE();
      break;
    case LINEARFIT:
    case FIXEDSLOPELINEARFIT:
    case FIXEDINTERCEPTLINEARFIT:
    case FIXEDLINEARFIT:
      slopes[col] = LR.slope();
      intercepts[col] = LR.intersection();
      sse[col] = LR.SSE();
      if (LR.Error())
        this->setError();
      return LR.SSE();
      break;
    default:
      handle.logWarning("Warning in surveyindex - unrecognised fittype", fittype);
      break;
  }
  return 0.0;
}
