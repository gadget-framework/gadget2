#include "sionstep.h"
#include "areatime.h"
#include "errorhandler.h"
#include "readfunc.h"
#include "readword.h"
#include "gadget.h"
#include "global.h"

SIOnStep::~SIOnStep() {
  int i;
  if (LgrpDiv != 0)
    delete LgrpDiv;
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
  for (i = 0; i < weightIndex.Size(); i++)
    delete weightIndex[i];
}

SIOnStep::SIOnStep(CommentStream& infile, const char* datafilename, const CharPtrVector& aindex,
  const TimeClass* const TimeInfo, const IntMatrix& areas, const CharPtrVector& charindex,
  const char* givenname, int bio, SIType type) : HasName(givenname), Areas(areas), alptr(0) {

  biomass = bio;
  sitype = type;
  useweight = 0;
  timeindex = 0;
  slope = 0.0;
  intercept = 0.0;

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
  readSIRegressionData(infile);

  //read the survey indices data from the datafile
  ifstream datafile;
  CommentStream subdata(datafile);
  datafile.open(datafilename, ios::binary);
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
  if (useweight)
    tmpWeight.resize(Years.Size(), 0.0);
  tmpModel.resize(Years.Size(), 0.0);
  tmpData.resize(Years.Size(), 0.0);
  likelihoodValues.resize(areaindex.Size(), 0.0);
}

void SIOnStep::readSIData(CommentStream& infile, const TimeClass* const TimeInfo) {

  double tmpnumber, tmpweight;
  char tmparea[MaxStrLength], tmplabel[MaxStrLength];
  strncpy(tmparea, "", MaxStrLength);
  strncpy(tmplabel, "", MaxStrLength);
  int keepdata, timeid, colid, areaid;
  int i, year, step, count, reject;

  //Check the number of columns in the inputfile
  infile >> ws;
  if ((!useweight) && (countColumns(infile) != 5))
    handle.logFileMessage(LOGFAIL, "wrong number of columns in inputfile - should be 5");
  else if ((useweight) && (countColumns(infile) != 6))
    handle.logFileMessage(LOGFAIL, "wrong number of columns in inputfile - should be 6");

  year = step = count = reject = 0;
  while (!infile.eof()) {
    keepdata = 1;

    if (useweight)
      infile >> year >> step >> tmparea >> tmplabel >> tmpnumber >> tmpweight >> ws;
    else
      infile >> year >> step >> tmparea >> tmplabel >> tmpnumber >> ws;

    //crude check to see if something has gone wrong and avoid infinite loops
    if (strlen(tmparea) == 0)
      handle.logFileMessage(LOGFAIL, "failed to read data from file");

    //if tmparea is in areaindex keep data, else dont keep the data
    areaid = -1;
    for (i = 0; i < areaindex.Size(); i++)
      if (strcasecmp(areaindex[i], tmparea) == 0)
        areaid = i;

    if (areaid == -1)
      keepdata = 0;

    //if tmplabel is in colindex find colid, else dont keep the data
    colid = -1;
    for (i = 0; i < colindex.Size(); i++)
      if (strcasecmp(colindex[i], tmplabel) == 0)
        colid = i;

    if (colid == -1)
      keepdata = 0;

    //check if the year and step are in the simulation
    timeid = -1;
    if ((TimeInfo->isWithinPeriod(year, step)) && (keepdata == 1)) {
      for (i = 0; i < Years.Size(); i++)
        if ((Years[i] == year) && (Steps[i] == step))
          timeid = i;

      if (timeid == -1) {
        Years.resize(1, year);
        Steps.resize(1, step);
        timeid = (Years.Size() - 1);
        obsIndex.resize(new DoubleMatrix(areaindex.Size(), colindex.Size(), 0.0));
        modelIndex.resize(new DoubleMatrix(areaindex.Size(), colindex.Size(), 0.0));
        if (useweight)
          weightIndex.resize(new DoubleMatrix(areaindex.Size(), colindex.Size(), 0.0));
      }

    } else
      keepdata = 0;

    if (keepdata == 1) {
      //survey indices data is required, so store it
      count++;
      (*obsIndex[timeid])[areaid][colid] = tmpnumber;
      if (useweight)
        (*weightIndex[timeid])[areaid][colid] = tmpweight;
    } else
      reject++;  //count number of rejected data points read from file
  }

  AAT.addActions(Years, Steps, TimeInfo);
  if (count == 0)
    handle.logMessage(LOGWARN, "Warning in surveyindex - found no data in the data file for", this->getName());
  if (Years.Size() < 2)
    handle.logMessage(LOGWARN, "Warning in surveyindex - insufficient data to fit a regression line for", this->getName());

  if ((handle.getLogLevel() >= LOGWARN) && (Steps.Size() > 0) && (this->getSIType() != SIEFFORT)) {
    //JMB to be comparable, this should only take place on the same step in each year
    //However the effort data will be on most timesteps so this check isnt required
    step = Steps[0];
    timeid = 0;
    for (i = 1; i < Steps.Size(); i++)
      if (Steps[i] != step)
        timeid++;

    if (timeid != 0)
      handle.logMessage(LOGWARN, "Warning in surveyindex - differing timesteps for", this->getName());
  }

  if (reject != 0)
    handle.logMessage(LOGMESSAGE, "Discarded invalid surveyindex data - number of invalid entries", reject);
  handle.logMessage(LOGMESSAGE, "Read surveyindex data file - number of entries", count);
}

void SIOnStep::readSIRegressionData(CommentStream& infile) {

  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);

  infile >> ws >> text;
  if (strcasecmp(text, "linearfit") == 0) {
    fittype = LINEARFIT;
    LR = new LinearRegression(FREE);
  } else if (strcasecmp(text, "loglinearfit") == 0) {
    fittype = LOGLINEARFIT;
    LR = new LogLinearRegression(FREE);
  } else if (strcasecmp(text, "weightlinearfit") == 0) {
    useweight = 1;
    fittype = WEIGHTLINEARFIT;
    LR = new WeightRegression(FREE);
  } else if (strcasecmp(text, "logweightlinearfit") == 0) {
    useweight = 1;
    fittype = LOGWEIGHTLINEARFIT;
    LR = new LogWeightRegression(FREE);
  } else if (strcasecmp(text, "fixedslopelinearfit") == 0) {
    fittype = FIXEDSLOPELINEARFIT;
    LR = new LinearRegression(FIXEDSLOPE);
  } else if (strcasecmp(text, "fixedslopeloglinearfit") == 0) {
    fittype = FIXEDSLOPELOGLINEARFIT;
    LR = new LogLinearRegression(FIXEDSLOPE);
  } else if (strcasecmp(text, "fixedslopeweightlinearfit") == 0) {
    useweight = 1;
    fittype = FIXEDSLOPEWEIGHTLINEARFIT;
    LR = new WeightRegression(FIXEDSLOPE);
  } else if (strcasecmp(text, "fixedslopelogweightlinearfit") == 0) {
    useweight = 1;
    fittype = FIXEDSLOPELOGWEIGHTLINEARFIT;
    LR = new LogWeightRegression(FIXEDSLOPE);
  } else if (strcasecmp(text, "fixedinterceptlinearfit") == 0) {
    fittype = FIXEDINTERCEPTLINEARFIT;
    LR = new LinearRegression(FIXEDINTERCEPT);
  } else if (strcasecmp(text, "fixedinterceptloglinearfit") == 0) {
    fittype = FIXEDINTERCEPTLOGLINEARFIT;
    LR = new LogLinearRegression(FIXEDINTERCEPT);
  } else if (strcasecmp(text, "fixedinterceptweightlinearfit") == 0) {
    useweight = 1;
    fittype = FIXEDINTERCEPTWEIGHTLINEARFIT;
    LR = new WeightRegression(FIXEDINTERCEPT);
  } else if (strcasecmp(text, "fixedinterceptlogweightlinearfit") == 0) {
    useweight = 1;
    fittype = FIXEDINTERCEPTLOGWEIGHTLINEARFIT;
    LR = new LogWeightRegression(FIXEDINTERCEPT);
  } else if (strcasecmp(text, "fixedlinearfit") == 0) {
    fittype = FIXEDLINEARFIT;
    LR = new LinearRegression(FIXED);
  } else if (strcasecmp(text, "fixedloglinearfit") == 0) {
    fittype = FIXEDLOGLINEARFIT;
    LR = new LogLinearRegression(FIXED);
  } else if (strcasecmp(text, "fixedweightlinearfit") == 0) {
    useweight = 1;
    fittype = FIXEDWEIGHTLINEARFIT;
    LR = new WeightRegression(FIXED);
  } else if (strcasecmp(text, "fixedlogweightlinearfit") == 0) {
    useweight = 1;
    fittype = FIXEDLOGWEIGHTLINEARFIT;
    LR = new LogWeightRegression(FIXED);
  } else
    handle.logFileMessage(LOGFAIL, "\nError in surveyindex - unrecognised fittype", text);

  switch (fittype) {
    case LINEARFIT:
    case LOGLINEARFIT:
    case WEIGHTLINEARFIT:
    case LOGWEIGHTLINEARFIT:
      break;
    case FIXEDSLOPELINEARFIT:
    case FIXEDSLOPELOGLINEARFIT:
    case FIXEDSLOPEWEIGHTLINEARFIT:
    case FIXEDSLOPELOGWEIGHTLINEARFIT:
      readWordAndVariable(infile, "slope", slope);
      LR->setSlope(slope);
      break;
    case FIXEDINTERCEPTLINEARFIT:
    case FIXEDINTERCEPTLOGLINEARFIT:
    case FIXEDINTERCEPTWEIGHTLINEARFIT:
    case FIXEDINTERCEPTLOGWEIGHTLINEARFIT:
      readWordAndVariable(infile, "intercept", intercept);
      LR->setIntercept(intercept);
      break;
    case FIXEDLINEARFIT:
    case FIXEDLOGLINEARFIT:
    case FIXEDWEIGHTLINEARFIT:
    case FIXEDLOGWEIGHTLINEARFIT:
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
          << setw(printwidth) << colindex[i] << sep << setw(largewidth);

        //JMB crude filter to remove the 'silly' values from the output
        if ((*modelIndex[timeindex])[a][i] < rathersmall)
          outfile << 0;
        else
          outfile << setprecision(largeprecision) << (*modelIndex[timeindex])[a][i];

        if (useweight)
          outfile << sep << setw(printwidth) << (*weightIndex[timeindex])[a][i];
        outfile << endl;
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
    handle.logMessage(LOGMESSAGE, "Calculating likelihood score for surveyindex component", this->getName());

  int a, i, j;
  double score = 0.0;
  for (a = 0; a < areaindex.Size(); a++) {
    likelihoodValues[a] = 0.0;
    for (i = 0; i < colindex.Size(); i++) {
      for (j = 0; j < tmpData.Size(); j++) {
        tmpData[j] = (*obsIndex[j])[a][i];
        tmpModel[j] = (*modelIndex[j])[a][i];
      }

      //if the weights are required then pass them to the regression line
      if (useweight) {
        for (j = 0; j < tmpWeight.Size(); j++)
          tmpWeight[j] = (*weightIndex[j])[a][i];
        LR->setWeights(tmpWeight);
      }

      //fit the data to the (log) linear regression line
      LR->storeVectors(tmpModel, tmpData);
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
      << setw(largewidth) << this->getName() << sep << setprecision(smallprecision)
      << setw(smallwidth) << weight << sep << setprecision(largeprecision)
      << setw(largewidth) << likelihoodValues[a] << endl;
  outfile.flush();
}

void SIOnStep::Reset() {
  int i;
  for (i = 0; i < modelIndex.Size(); i++)
    (*modelIndex[i]).setToZero();
}
