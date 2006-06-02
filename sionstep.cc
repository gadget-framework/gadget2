#include "sionstep.h"
#include "areatime.h"
#include "errorhandler.h"
#include "readfunc.h"
#include "readword.h"
#include "gadget.h"

extern ErrorHandler handle;

SIOnStep::~SIOnStep() {
  int i;
  delete[] siname;
  if (LR != 0)
    delete LR;
  for (i = 0; i < areaindex.Size(); i++)
    delete[] areaindex[i];
  for (i = 0; i < colindex.Size(); i++)
    delete[] colindex[i];
}

SIOnStep::SIOnStep(CommentStream& infile, const char* datafilename,
  const CharPtrVector& aindex, const TimeClass* const TimeInfo,
  const IntMatrix& areas, const CharPtrVector& charindex,
  const char* name, int bio) : Areas(areas), alptr(0), biomass(bio) {

  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);

  timeindex = 0;
  slope = 0.0;
  intercept = 0.0;

  siname = new char[strlen(name) + 1];
  strcpy(siname, name);

  int i;
  for (i = 0; i < aindex.Size(); i++) {
    areaindex.resize(new char[strlen(aindex[i]) + 1]);
    strcpy(areaindex[i], aindex[i]);
  }
  for (i = 0; i < charindex.Size(); i++) {
    colindex.resize(new char[strlen(charindex[i]) + 1]);
    strcpy(colindex[i], charindex[i]);
  }

  //read the fittype
  infile >> text;
  if (strcasecmp(text, "linearfit") == 0) {
    fittype = LINEARFIT;
    LR = new LinearRegression(FREE);
  } else if (strcasecmp(text, "loglinearfit") == 0) {
    fittype = LOGLINEARFIT;
    LR = new LogLinearRegression(FREE);
  } else if (strcasecmp(text, "fixedslopelinearfit") == 0) {
    fittype = FIXEDSLOPELINEARFIT;
    LR = new LinearRegression(FIXEDSLOPE);
  } else if (strcasecmp(text, "fixedslopeloglinearfit") == 0) {
    fittype = FIXEDSLOPELOGLINEARFIT;
    LR = new LogLinearRegression(FIXEDSLOPE);
  } else if (strcasecmp(text, "fixedinterceptlinearfit") == 0) {
    fittype = FIXEDINTERCEPTLINEARFIT;
    LR = new LinearRegression(FIXEDINTERCEPT);
  } else if (strcasecmp(text, "fixedinterceptloglinearfit") == 0) {
    fittype = FIXEDINTERCEPTLOGLINEARFIT;
    LR = new LogLinearRegression(FIXEDINTERCEPT);
  } else if (strcasecmp(text, "fixedlinearfit") == 0) {
    fittype = FIXEDLINEARFIT;
    LR = new LinearRegression(FIXED);
  } else if (strcasecmp(text, "fixedloglinearfit") == 0) {
    fittype = FIXEDLOGLINEARFIT;
    LR = new LogLinearRegression(FIXED);
  } else
    handle.logFileMessage(LOGFAIL, "\nError in surveyindex - unrecognised fittype", text);

  switch (fittype) {
    case LINEARFIT:
    case LOGLINEARFIT:
      break;
    case FIXEDSLOPELINEARFIT:
    case FIXEDSLOPELOGLINEARFIT:
      readWordAndVariable(infile, "slope", slope);
      LR->setSlope(slope);
      break;
    case FIXEDINTERCEPTLINEARFIT:
    case FIXEDINTERCEPTLOGLINEARFIT:
      readWordAndVariable(infile, "intercept", intercept);
      LR->setIntercept(intercept);
      break;
    case FIXEDLINEARFIT:
    case FIXEDLOGLINEARFIT:
      readWordAndVariable(infile, "slope", slope);
      readWordAndVariable(infile, "intercept", intercept);
      LR->setSlope(slope);
      LR->setIntercept(intercept);
      break;
    default:
      handle.logFileMessage(LOGFAIL, "\nError in surveyindex - unrecognised fittype", text);
      break;
  }

  //JMB - check that the slope of the regression line is positive
  if (slope < 0.0)
    handle.logFileMessage(LOGFAIL, "\nError in surveyindex - slope of the regression line must be positive", slope);

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
  slopes.resize(this->numIndex(), slope);
  intercepts.resize(this->numIndex(), intercept);
  sse.resize(this->numIndex(), 0.0);
  stocksize.resize(obsIndex.Nrow(), 0.0);
  indices.resize(obsIndex.Nrow(), 0.0);
}

void SIOnStep::readSIData(CommentStream& infile, const TimeClass* const TimeInfo) {

  double tmpnumber;
  char tmparea[MaxStrLength], tmplabel[MaxStrLength];
  strncpy(tmparea, "", MaxStrLength);
  strncpy(tmplabel, "", MaxStrLength);
  int keepdata, timeid, colid, areaid;
  int i, year, step, count;

  //Check the number of columns in the inputfile
  infile >> ws;
  if (countColumns(infile) != 5)
    handle.logFileMessage(LOGFAIL, "wrong number of columns in inputfile - should be 5");

  year = step = count = 0;
  while (!infile.eof()) {
    keepdata = 0;
    infile >> year >> step >> tmparea >> tmplabel >> tmpnumber >> ws;

    //crude check to see if something has gone wrong and avoid infinite loops
    if (strlen(tmparea) == 0)
      handle.logFileMessage(LOGFAIL, "failed to read data from file");

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
        obsIndex.AddRows(1, colindex.Size(), 0.0);
      }
      obsIndex[timeid][colid] = tmpnumber;
    }
  }

  AAT.addActions(Years, Steps, TimeInfo);
  if (count == 0)
    handle.logMessage(LOGWARN, "Warning in surveyindex - found no data in the data file for", this->getSIName());

  if (Steps.Size() > 0) {
    //JMB - to be comparable, this should only take place on the same step in each year
    step = Steps[0];
    timeid = 0;
    for (i = 1; i < Steps.Size(); i++)
      if (Steps[i] != step)
        timeid++;

    if (timeid != 0)
      handle.logMessage(LOGWARN, "Warning in surveyindex - differing timesteps for", this->getSIName());
  }
  handle.logMessage(LOGMESSAGE, "Read surveyindex data file - number of entries", count);
}

void SIOnStep::Reset(const Keeper* const keeper) {
  int i, j;
  for (i = 0; i < modelIndex.Nrow(); i++)
    for (j = 0; j < modelIndex.Ncol(i); j++)
      modelIndex[i][j] = 0.0;
  for (i = 0; i < sse.Size(); i++)
    sse[i] = 0.0;
  if (handle.getLogLevel() >= LOGMESSAGE)
    handle.logMessage(LOGMESSAGE, "Reset surveyindex component", this->getSIName());
}

void SIOnStep::printLikelihood(ofstream& outfile, const TimeClass* const TimeInfo) {

  int i, area;
  if (AAT.atCurrentTime(TimeInfo)) {
    timeindex = -1;
    for (i = 0; i < Years.Size(); i++)
      if ((Years[i] == TimeInfo->getYear()) && (Steps[i] == TimeInfo->getStep()))
        timeindex = i;
    if (timeindex == -1)
      handle.logMessage(LOGFAIL, "Error in surveyindex - invalid timestep");

    //JMB - this is nasty hack since there is only one area
    area = 0;
    for (i = 0; i < colindex.Size(); i++) {
      outfile << setw(lowwidth) << Years[timeindex] << sep << setw(lowwidth)
        << Steps[timeindex] << sep << setw(printwidth) << areaindex[area] << sep
        << setw(printwidth) << colindex[i] << sep << setprecision(largeprecision)
        << setw(largewidth) << modelIndex[timeindex][i] << endl;
    }
  }

  //JMB - this is nasty hack to output the regression information
  if (TimeInfo->getTime() == TimeInfo->numTotalSteps()) {
    outfile << "; Regression information\n";
    for (i = 0; i < colindex.Size(); i++)
      outfile << "; " << colindex[i] << " intercept " << intercepts[i]
        << " slope " << slopes[i] << " sse " << sse[i] << endl;
  }
}

double SIOnStep::calcSSE() {
  if (modelIndex.Nrow() < 2)
    return 0.0;

  if (handle.getLogLevel() >= LOGMESSAGE)
    handle.logMessage(LOGMESSAGE, "Calculating likelihood score for surveyindex component", this->getSIName());

  int i, j;
  double score = 0.0;
  for (i = 0; i < this->numIndex(); i++) {
    for (j = 0; j < modelIndex.Nrow(); j++) {
      indices[j] = obsIndex[j][i];
      stocksize[j] = modelIndex[j][i];
    }

    //fit the data to the (log) linear regression curve
    LR->storeVectors(stocksize, indices);
    LR->calcFit();

    //and then store the results
    slopes[i] = LR->getSlope();
    intercepts[i] = LR->getIntercept();
    sse[i] = LR->getSSE();
    score += sse[i];
  }

  if (handle.getLogLevel() >= LOGMESSAGE)
    handle.logMessage(LOGMESSAGE, "The likelihood score from the regression line for this component is", score);

  return score;
}
