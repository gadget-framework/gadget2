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
  for (i = 0; i < obsIndex.Size(); i++) {
    delete obsIndex[i];
    delete modelIndex[i];
  }
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

  //resize to store the regression information
  slopes.AddRows(areaindex.Size(), colindex.Size(), slope);
  intercepts.AddRows(areaindex.Size(), colindex.Size(), intercept);
  sse.AddRows(areaindex.Size(), colindex.Size(), 0.0);
  stocksize.resize(Years.Size(), 0.0);
  indices.resize(Years.Size(), 0.0);
  likelihoodValues.resize(areaindex.Size(), 0.0);
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
        obsIndex.resize(new DoubleMatrix(areaindex.Size(), colindex.Size(), 0.0));
        modelIndex.resize(new DoubleMatrix(areaindex.Size(), colindex.Size(), 0.0));
      }
      (*obsIndex[timeid])[areaid][colid] = tmpnumber;
    }
  }

  AAT.addActions(Years, Steps, TimeInfo);
  if (count == 0)
    handle.logMessage(LOGWARN, "Warning in surveyindex - found no data in the data file for", this->getSIName());
  if (Years.Size() < 2)
    handle.logMessage(LOGWARN, "Warning in surveyindex - insufficient data to fit a regression line for", this->getSIName());

  if ((handle.getLogLevel() >= LOGWARN) && (Steps.Size() > 0)) {
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

void SIOnStep::printLikelihood(ofstream& outfile, const TimeClass* const TimeInfo) {

  int a, i;
  if (AAT.atCurrentTime(TimeInfo)) {
    timeindex = -1;
    for (i = 0; i < Years.Size(); i++)
      if ((Years[i] == TimeInfo->getYear()) && (Steps[i] == TimeInfo->getStep()))
        timeindex = i;
    if (timeindex == -1)
      handle.logMessage(LOGFAIL, "Error in surveyindex - invalid timestep");

    for (a = 0; a < areaindex.Size(); a++) {
      for (i = 0; i < colindex.Size(); i++) {
        outfile << setw(lowwidth) << Years[timeindex] << sep << setw(lowwidth)
          << Steps[timeindex] << sep << setw(printwidth) << areaindex[a] << sep
          << setw(printwidth) << colindex[i] << sep << setprecision(largeprecision)
          << setw(largewidth) << (*modelIndex[timeindex])[a][i] << endl;
      }
    }
  }

  //JMB - this is nasty hack to output the regression information
  if (TimeInfo->getTime() == TimeInfo->numTotalSteps()) {
    for (a = 0; a < areaindex.Size(); a++) {
      outfile << "; Regression information for area " << areaindex[a] << endl;
      for (i = 0; i < colindex.Size(); i++)
        outfile << "; " << colindex[i] << " intercept " << intercepts[a][i]
          << " slope " << slopes[a][i] << " sse " << sse[a][i] << endl;
    }
  }
}

double SIOnStep::calcSSE() {

  if (handle.getLogLevel() >= LOGMESSAGE)
    handle.logMessage(LOGMESSAGE, "Calculating likelihood score for surveyindex component", this->getSIName());

  int a, i, j;
  double score = 0.0;
  for (a = 0; a < areaindex.Size(); a++) {
    likelihoodValues[a] = 0.0;
    for (i = 0; i < colindex.Size(); i++) {
      for (j = 0; j < indices.Size(); j++) {
        indices[j] = (*obsIndex[j])[a][i];
        stocksize[j] = (*modelIndex[j])[a][i];
      }

      //fit the data to the (log) linear regression curve
      LR->storeVectors(stocksize, indices);
      LR->calcFit();

      //and then store the results
      slopes[a][i] = LR->getSlope();
      intercepts[a][i] = LR->getIntercept();
      sse[a][i] = LR->getSSE();
      likelihoodValues[a] += LR->getSSE();
    }
    score += likelihoodValues[a];
  }

  if (handle.getLogLevel() >= LOGMESSAGE)
    handle.logMessage(LOGMESSAGE, "The likelihood score from the regression line for this component is", score);
  return score;
}

void SIOnStep::printSummary(ofstream& outfile, const double weight) {
  int a;
  for (a = 0; a < areaindex.Size(); a++)
    outfile << "all   all " << setw(printwidth) << areaindex[a] << sep
      << setw(largewidth) << this->getSIName() << sep << setprecision(smallprecision)
      << setw(smallwidth) << weight << sep << setprecision(largeprecision)
      << setw(largewidth) << likelihoodValues[a] << endl;
  outfile.flush();
}
