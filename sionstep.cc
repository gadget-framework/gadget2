#include "sionstep.h"
#include "areatime.h"
#include "errorhandler.h"
#include "readfunc.h"
#include "readword.h"
#include "loglinearregression.h"
#include "gadget.h"

extern ErrorHandler handle;

SIOnStep::~SIOnStep() {
  int i;
  delete[] siname;
  for (i = 0; i < areaindex.Size(); i++)
    delete[] areaindex[i];
  for (i = 0; i < colindex.Size(); i++)
    delete[] colindex[i];
}

SIOnStep::SIOnStep(CommentStream& infile, const char* datafilename,
  const CharPtrVector& aindex, const TimeClass* const TimeInfo,
  const IntMatrix& areas, const CharPtrVector& charindex,
  const char* name) : Areas(areas), alptr(0) {

  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);

  timeindex = 0;
  slope = 0.0;
  intercept = 0.0;

  siname = new char[strlen(name) + 1];
  strcpy(siname, name);

  int i;
  for (i = 0; i < aindex.Size(); i++) {
    areaindex.resize(1);
    areaindex[i] = new char[strlen(aindex[i]) + 1];
    strcpy(areaindex[i], aindex[i]);
  }
  for (i = 0; i < charindex.Size(); i++) {
    colindex.resize(1);
    colindex[i] = new char[strlen(charindex[i]) + 1];
    strcpy(colindex[i], charindex[i]);
  }

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
    handle.logFileMessage(LOGFAIL, "Error in surveyindex - unrecognised fittype", text);

  switch (fittype) {
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
      handle.logFileMessage(LOGFAIL, "Error in surveyindex - unrecognised fittype");
      break;
  }

  //JMB - check that the slope of the regression line is positive
  if (slope < 0)
    handle.logFileMessage(LOGFAIL, "Error in surveyindex - slope of the regression line must be positive");

  //read the survey indices data from the datafile
  ifstream datafile;
  CommentStream subdata(datafile);
  datafile.open(datafilename, ios::in);
  handle.checkIfFailure(datafile, datafilename);
  handle.Open(datafilename);
  readSIData(subdata, TimeInfo);
  handle.Close();
  datafile.close();
  datafile.clear();

  //resize to hold the model information
  modelIndex.AddRows(obsIndex.Nrow(), this->numIndex(), 0.0);
  slopes.resize(this->numIndex(), 0.0);
  intercepts.resize(this->numIndex(), 0.0);
  sse.resize(this->numIndex(), 0.0);
}

void SIOnStep::readSIData(CommentStream& infile, const TimeClass* const TimeInfo) {

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
    handle.logFileMessage(LOGFAIL, "Wrong number of columns in inputfile - should be 5");

  while (!infile.eof()) {
    keepdata = 0;
    infile >> year >> step >> tmparea >> tmplabel >> tmpnumber >> ws;

    //check if the year and step are in the simulation
    timeid = -1;
    if (TimeInfo->isWithinPeriod(year, step)) {
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
        obsIndex.AddRows(1, this->numIndex(), 0.0);
      }
      obsIndex[timeid][colid] = tmpnumber;
    }
  }
  AAT.addActions(Years, Steps, TimeInfo);
  if ((handle.getLogLevel() >= LOGMESSAGE) && (count == 0))
    handle.logMessage(LOGWARN, "Warning in surveyindex - found no data in the data file for", this->getSIName());
  if (handle.getLogLevel() >= LOGMESSAGE)
    handle.logMessage(LOGMESSAGE, "Read surveyindex data file - number of entries", count);
}

void SIOnStep::Reset(const Keeper* const keeper) {
  timeindex = 0;
  int i, j;
  for (i = 0; i < modelIndex.Nrow(); i++)
    for (j = 0; j < modelIndex.Ncol(i); j++)
      modelIndex[i][j] = 0.0;
  if (handle.getLogLevel() >= LOGMESSAGE)
    handle.logMessage(LOGMESSAGE, "Reset surveyindex component", this->getSIName());
}

void SIOnStep::Print(ofstream& outfile) const {
  int i, j, t;

  t = timeindex - 1;
  if (t < 0)
    t = 0;

  for (i = 0; i < Areas.Nrow(); i++) {
    outfile << "\tInternal areas";
    for (j = 0; j < Areas.Ncol(i); j++)
      outfile << sep << Areas[i][j];
  }
  outfile << endl;

  for (i = 0; i < this->numIndex(); i++)
    outfile << TAB << setw(smallwidth) << modelIndex[t][i];
  outfile << endl;
  outfile.flush();
}

void SIOnStep::LikelihoodPrint(ofstream& outfile, const TimeClass* const TimeInfo) {

  int t, area, i;

  if (AAT.atCurrentTime(TimeInfo)) {
    t = timeindex - 1; //timeindex was increased before this is called
    if ((t >= Years.Size()) || t < 0)
      handle.logMessage(LOGFAIL, "Error in surveyindex - invalid timestep", t);

    //JMB - this is nasty hack since there is only one area
    area = 0;
    for (i = 0; i < this->numIndex(); i++) {
      outfile << setw(lowwidth) << Years[t] << sep << setw(lowwidth)
        << Steps[t] << sep << setw(printwidth) << areaindex[area] << sep
        << setw(printwidth) << colindex[i] << sep << setprecision(largeprecision)
        << setw(largewidth) << modelIndex[t][i] << endl;
    }
  }

  //JMB - this is nasty hack to output the regression information
  if (TimeInfo->getTime() == TimeInfo->numTotalSteps()) {
    outfile << "; Regression information\n";
    for (i = 0; i < this->numIndex(); i++)
      outfile << "; " << colindex[i] << " intercept " << intercepts[i]
        << " slope " << slopes[i] << " sse " << sse[i] << endl;
  }
}

double SIOnStep::calcSSE() {
  if (timeindex < 2)
    return 0.0;

  double score = 0.0;
  int i, t;
  DoubleVector indices(timeindex);
  DoubleVector stocksize(timeindex);

  for (i = 0; i < this->numIndex(); i++) {
    //Let LLR figure out what to do in the case of zero stock size.
    for (t = 0; t < timeindex; t++) {
      indices[t] = obsIndex[t][i];
      stocksize[t] = modelIndex[t][i];
    }
    //Now fit the log of the abundance indices as a function of stock size.
    score += this->calcRegression(stocksize, indices, i);
  }

  if (handle.getLogLevel() >= LOGMESSAGE) {
    handle.logMessage(LOGMESSAGE, "Calculating likelihood score for surveyindex component", this->getSIName());
    handle.logMessage(LOGMESSAGE, "The likelihood score from the regression line for this component is", score);
  }

  return score;
}

double SIOnStep::calcRegression(const DoubleVector& stocksize, const DoubleVector& indices, int i) {

  LogLinearRegression LLR;
  LinearRegression LR;

  //fit the data to the (log) linear regression curve
  switch (fittype) {
    case LOGLINEARFIT:
      LLR.calcFit(stocksize, indices);
      break;
    case FIXEDSLOPELOGLINEARFIT:
      LLR.calcFit(stocksize, indices, slope);
      break;
    case FIXEDINTERCEPTLOGLINEARFIT:
      LLR.calcFit(intercept, stocksize, indices);
      break;
    case FIXEDLOGLINEARFIT:
      LLR.calcFit(stocksize, indices, slope, intercept);
      break;
    case LINEARFIT:
      LR.calcFit(stocksize, indices);
      break;
    case FIXEDSLOPELINEARFIT:
      LR.calcFit(stocksize, indices, slope);
      break;
    case FIXEDINTERCEPTLINEARFIT:
      LR.calcFit(intercept, stocksize, indices);
      break;
    case FIXEDLINEARFIT:
      LR.calcFit(stocksize, indices, slope, intercept);
      break;
    default:
      handle.logMessage(LOGWARN, "Warning in surveyindex - unrecognised fittype", fittype);
      break;
  }

  //and then store the results
  switch (fittype) {
    case LOGLINEARFIT:
    case FIXEDSLOPELOGLINEARFIT:
    case FIXEDINTERCEPTLOGLINEARFIT:
    case FIXEDLOGLINEARFIT:
      slopes[i] = LLR.getSlope();
      intercepts[i] = LLR.getIntersection();
      sse[i] = LLR.getSSE();
      return LLR.getSSE();
      break;
    case LINEARFIT:
    case FIXEDSLOPELINEARFIT:
    case FIXEDINTERCEPTLINEARFIT:
    case FIXEDLINEARFIT:
      slopes[i] = LR.getSlope();
      intercepts[i] = LR.getIntersection();
      sse[i] = LR.getSSE();
      return LR.getSSE();
      break;
    default:
      handle.logMessage(LOGWARN, "Warning in surveyindex - unrecognised fittype", fittype);
      break;
  }
  return 0.0;
}
