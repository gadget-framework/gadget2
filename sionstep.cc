#include "sionstep.h"
#include "areatime.h"
#include "errorhandler.h"
#include "readfunc.h"
#include "readword.h"
#include "loglinearregression.h"
#include "gadget.h"

SIOnStep::~SIOnStep() {
}

SIOnStep::SIOnStep(CommentStream& infile, const char* datafilename, const char* arealabel,
  const TimeClass* const TimeInfo, const charptrvector& lenindex, const charptrvector& ageindex) {

  ErrorHandler handle;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);

  error = 0;
  NumberOfSums = 0;

  //read the fittype
  infile >> text;
  if (strcasecmp(text, "linearfit") == 0)
    fittype = LinearFit;
  else if (strcasecmp(text, "powerfit") == 0)
    fittype = PowerFit;
  else
    handle.Unexpected("Error in surveyindex - unrecognised fittype", text);

  //set the slope and intercept values to a default of 0
  slope = 0.0;
  intercept = 0.0;

  //read the year and step data from the datafile
  ifstream datafile;
  CommentStream subdata(datafile);

  datafile.open(datafilename);
  CheckIfFailure(datafile, datafilename);
  handle.Open(datafilename);
  ReadSIData(subdata, arealabel, lenindex, ageindex, TimeInfo);
  handle.Close();
  datafile.close();
  datafile.clear();
}

SIOnStep::SIOnStep(CommentStream& infile, const char* datafilename, const char* arealabel,
  const TimeClass* const TimeInfo, const charptrvector& colindex) {

  ErrorHandler handle;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);

  error = 0;
  NumberOfSums = 0;

  //read the fittype
  infile >> text;
  if (strcasecmp(text, "linearfit") == 0)
    fittype = LinearFit;
  else if (strcasecmp(text, "loglinearfit") == 0)
    fittype = LogLinearFit;
  else if (strcasecmp(text, "fixedslopelinearfit") == 0)
    fittype = FixedSlopeLinearFit;
  else if (strcasecmp(text, "fixedslopeloglinearfit") == 0)
    fittype = FixedSlopeLogLinearFit;
  else if (strcasecmp(text, "fixedinterceptlinearfit") == 0)
    fittype = FixedInterceptLinearFit;
  else if (strcasecmp(text, "fixedinterceptloglinearfit") == 0)
    fittype = FixedInterceptLogLinearFit;
  else if (strcasecmp(text, "fixedlinearfit") == 0)
    fittype = FixedLinearFit;
  else if (strcasecmp(text, "fixedloglinearfit") == 0)
    fittype = FixedLogLinearFit;
  else
    handle.Unexpected("Error in surveyindex - unrecognised fittype", text);

  switch (fittype) {
    case LinearFit:
    case LogLinearFit:
      //No reading is necessary. Set the variables although they will not be used:
      slope = 0.0;
      intercept = 0.0;
      break;
    case FixedSlopeLinearFit:
    case FixedSlopeLogLinearFit:
      ReadWordAndVariable(infile, "slope", slope);
      intercept = 0.0;
      break;
    case FixedInterceptLinearFit:
    case FixedInterceptLogLinearFit:
      ReadWordAndVariable(infile, "intercept", intercept);
      slope = 0.0;
      break;
    case FixedLinearFit:
    case FixedLogLinearFit:
      ReadWordAndVariable(infile, "slope", slope);
      ReadWordAndVariable(infile, "intercept", intercept);
      break;
    default:
      handle.Message("Error in surveyindex - unrecognised fittype");
      break;
  }

  //read the survey indices data from the datafile
  ifstream datafile;
  CommentStream subdata(datafile);

  datafile.open(datafilename);
  CheckIfFailure(datafile, datafilename);
  handle.Open(datafilename);
  ReadSIData(subdata, arealabel, colindex, TimeInfo);
  handle.Close();
  datafile.close();
  datafile.clear();

  int ncol = Indices.Ncol();
  if (Indices.Nrow() > 0)
    abundance.AddRows(Indices.Nrow(), ncol, 0.0);
  slopes.resize(ncol);
  intercepts.resize(ncol);
  sse.resize(ncol);
}

void SIOnStep::ReadSIData(CommentStream& infile, const char* arealabel,
  const charptrvector& colindex, const TimeClass* TimeInfo) {

  int i;
  int year, step;
  double tmpnumber;
  char tmparea[MaxStrLength], tmplabel[MaxStrLength];
  strncpy(tmparea, "", MaxStrLength);
  strncpy(tmplabel, "", MaxStrLength);
  int keepdata, timeid, colid;
  ErrorHandler handle;

  //Check the number of columns in the inputfile
  if (!(CheckColumns(infile, 5)))
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

    //if tmparea matches strlabel keep data, else dont keep the data
    if (!(strcasecmp(arealabel, tmparea) == 0))
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
      if (timeid == -1) {
        Years.resize(1, year);
        Steps.resize(1, step);
        timeid = (Years.Size() - 1);
        Indices.AddRows(1, colindex.Size(), 0.0);
      }
      Indices[timeid][colid] = tmpnumber;
    }

    //Keep all years from the input file in a vector [mnaa 25.04.00]
    if (YearsInFile.Size() == 0 || year != YearsInFile[YearsInFile.Size() - 1])
      YearsInFile.resize(1, year);
  }
  AAT.AddActions(Years, Steps, TimeInfo);
}

void SIOnStep::ReadSIData(CommentStream& infile, const char* arealabel,
  const charptrvector& lenindex, const charptrvector& ageindex,
  const TimeClass* TimeInfo) {

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
      //if this is a new timestep, resize to store the data
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

    if (lenid == -1)
      keepdata = 1;

    if (keepdata == 0) {
      //survey indices data is required, so store it
      if (timeid == -1) {
        Years.resize(1, year);
        Steps.resize(1, step);
      }
    }

    //Keep all years from the input file in a vector [mnaa 25.04.00]
    if (YearsInFile.Size() == 0 || year != YearsInFile[YearsInFile.Size() - 1])
      YearsInFile.resize(1, year);
  }
  AAT.AddActions(Years, Steps, TimeInfo);
  Indices.AddRows(Years.Size(),  1, 0.0);
  abundance.AddRows(Years.Size(), 1, 0.0);
}

void SIOnStep::Reset(const Keeper* const keeper) {
  error = 0;
  NumberOfSums = 0;
  int i, j;
  for (i = 0; i < abundance.Nrow(); i++)
    for (j = 0; j < abundance.Ncol(i); j++)
      abundance[i][j] = 0;
}

void SIOnStep::Print(ofstream& outfile) const {
  int i, j;
  outfile << "\tSurvey indices -- error " << error << endl;
  for (i = 0; i < Years.Size(); i++) {
    outfile << TAB << Years[i] << sep << Steps[i] << sep;
    for (j = 0; j < Indices.Ncol(i); j++) {
      outfile.width(printwidth);
      outfile.precision(smallprecision);
      outfile << Indices[i][j] << sep;
    }
    outfile << endl;
  }
  outfile << "\tNumber of sums " << NumberOfSums << "\n\tAbundance numbers\n";
  for (i = 0; i < Years.Size(); i++) {
    outfile << TAB << Years[i] << sep << Steps[i] << sep;
    for (j = 0; j < Indices.Ncol(i); j++) {
      outfile.width(printwidth);
      outfile.precision(smallprecision);
      outfile << abundance[i][j] << sep;
    }
    outfile << endl;
  }
  outfile << flush;
}

void SIOnStep::LikelihoodPrint(ofstream& outfile) const {
  int i, j;
  outfile << "error " << error << "\nAbundance indices\n";
  for (i = 0; i < Years.Size(); i++) {
    outfile << TAB << Years[i] << sep << Steps[i] << sep;
    for (j = 0; j < Indices.Ncol(i); j++) {
      outfile.width(printwidth);
      outfile.precision(smallprecision);
      outfile << Indices[i][j] << sep;
    }
    outfile << endl;
  }
  outfile << "Calculated number in stock\n";
  for (i = 0; i < Years.Size(); i++) {
    outfile << TAB << Years[i] << sep << Steps[i] << sep;
    for (j = 0; j < Indices.Ncol(i); j++) {
      outfile.width(printwidth);
      outfile.precision(smallprecision);
      outfile << abundance[i][j] << sep;
    }
    outfile << endl;
  }

  outfile << "\tNumber of sums " << NumberOfSums
    << "\nRegression information\n\nintercepts" << sep;
  for (j = 0; j < Indices.Ncol(0); j++) {
    outfile.width(printwidth);
    outfile.precision(smallprecision);
    outfile << intercepts[j] << sep;
  }
  outfile << "\nSlopes" << sep;
  for (j = 0; j < Indices.Ncol(0); j++) {
    outfile.width(printwidth);
    outfile.precision(smallprecision);
    outfile << slopes[j] << sep;
  }
  outfile << "\nsse" << sep;
  for (j = 0; j < Indices.Ncol(0); j++) {
    outfile.width(printwidth);
    outfile.precision(smallprecision);
    outfile << sse[j] << sep;
  }
  outfile << flush;
}

int SIOnStep::IsToSum(const TimeClass* const TimeInfo) const {
  return AAT.AtCurrentTime(TimeInfo);
}

double SIOnStep::Regression() {
  if (NumberOfSums < 2) {
    this->SetError();
    return 0.0;
  }
  double likelihood = 0.0;
  int col, index;
  for (col = 0; col < Indices.Ncol(0); col++) {
    //Let LLR figure out what to do in the case of zero stock size.
    doublevector indices(NumberOfSums);
    doublevector stocksize(NumberOfSums);
    for (index = 0; index < NumberOfSums; index++) {
      indices[index] = Indices[index][col];
      stocksize[index] = abundance[index][col];
    }
    //Now fit the log of the abundance indices as a function of stock size.
    likelihood += this->Fit(stocksize, indices, col);
  }
  return likelihood;
}

void SIOnStep::KeepNumbers(const doublevector& numbers) {
  assert(numbers.Size() == abundance.Ncol());
  NumberOfSums++;
  int i;
  for (i = 0; i < numbers.Size(); i++)
    abundance[NumberOfSums - 1][i] = numbers[i];
}

double SIOnStep::Fit(const doublevector& stocksize, const doublevector& indices, int col) {

  LogLinearRegression LLR;
  LinearRegression LR;

  //fit the data to the (log) linear regression curve
  switch(fittype) {
    case LogLinearFit:
      LLR.Fit(stocksize, indices);
      break;
    case FixedSlopeLogLinearFit:
      LLR.Fit(stocksize, indices, slope);
      break;
    case FixedInterceptLogLinearFit:
      LLR.Fit(intercept, stocksize, indices);
      break;
    case FixedLogLinearFit:
      LLR.Fit(stocksize, indices, slope, intercept);
      break;
    case LinearFit:
      LR.Fit(stocksize, indices);
      break;
    case FixedSlopeLinearFit:
      LR.Fit(stocksize, indices, slope);
      break;
    case FixedInterceptLinearFit:
      LR.Fit(intercept, stocksize, indices);
      break;
    case FixedLinearFit:
      LR.Fit(stocksize, indices, slope, intercept);
      break;
    default:
      cerr << "Error in surveyindex - unrecognised fittype\n";
      exit(EXIT_FAILURE);
  }

  //and then store the results
  switch(fittype) {
    case LogLinearFit:
    case FixedSlopeLogLinearFit:
    case FixedInterceptLogLinearFit:
    case FixedLogLinearFit:
      slopes[col] = LLR.slope();
      intercepts[col] = LLR.intersection();
      sse[col] = LLR.SSE();
      if (LLR.Error())
        this->SetError();
      return LLR.SSE();
      break;
    case LinearFit:
    case FixedSlopeLinearFit:
    case FixedInterceptLinearFit:
    case FixedLinearFit:
      slopes[col] = LR.slope();
      intercepts[col] = LR.intersection();
      sse[col] = LR.SSE();
      if (LR.Error())
        this->SetError();
      return LR.SSE();
      break;
    default:
      cerr << "Error in surveyindex - unrecognised fittype\n";
      exit(EXIT_FAILURE);
  }
}
