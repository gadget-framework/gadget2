#include "readmatrix.h"
#include "readfunc.h"
#include "areatime.h"
#include "doublematrix.h"
#include "errorhandler.h"
#include "keeper.h"
#include "gadget.h"

int ReadAmounts(CommentStream& infile, const intvector& tmpareas,
  const TimeClass* const TimeInfo, const AreaClass* const Area,
  Formulamatrix& amount, Keeper* const keeper, const char* givenname) {

  ErrorHandler handle;
  int i;
  int year, step, area;
  Formula number;  //initialised to 0.0
  intvector Years, Steps;
  char tmpname[MaxStrLength];
  char tmpnumber[MaxStrLength];
  strncpy(tmpname, "", MaxStrLength);
  strncpy(tmpnumber, "", MaxStrLength);
  int count = 0;
  int keepdata, timeid, areaid, tmpareaid;

  //Find start of distribution data in datafile
  infile >> ws;
  char c;
  c = infile.peek();
  if (!isdigit(c)) {
    infile.get(c);
    while (c != '\n' && !infile.eof())
      infile.get(c);
  }

  //Check the number of columns in the inputfile
  if (CountColumns(infile) != 5)
    handle.Message("Wrong number of columns in inputfile - should be 5");

  //Create storage for the data - size of the data is known
  amount.AddRows(TimeInfo->TotalNoSteps() + 1, tmpareas.Size(), number);
  Years.resize(TimeInfo->TotalNoSteps() + 1, 0);
  Steps.resize(TimeInfo->TotalNoSteps() + 1, 0);
  year = TimeInfo->FirstYear();
  step = TimeInfo->FirstStep();
  int numsteps = TimeInfo->StepsInYear();
  for (i = 1; i < Years.Size(); i++) {
    //time is counted from timestep 1
    Years[i] = year;
    Steps[i] = step;
    step++;
    if (step > numsteps) {
      year++;
      step = 1;
    }
  }

  while (!infile.eof()) {
    keepdata = 0;
    infile >> year >> step >> area >> tmpname;

    //check if the year and step are in the simulation
    timeid = -1;
    if (TimeInfo->IsWithinPeriod(year, step)) {
      //calculate the timeid index - time is counted from 1
      for (i = 1; i < Years.Size(); i++)
        if ((Years[i] == year) && (Steps[i] == step))
          timeid = i;

    } else
      keepdata = 1;

    //only keep the data if tmpname matches givenname
    if (!(strcasecmp(givenname, tmpname) == 0))
      keepdata = 1;

    //only keep the data if area is a required area
    areaid = -1;
    tmpareaid = Area->InnerArea(area);
    if (tmpareaid == -1)
      handle.UndefinedArea(area);
    for (i = 0; i < tmpareas.Size(); i++)
      if (tmpareas[i] == tmpareaid)
        areaid = i;

    if (areaid == -1)
      keepdata = 1;

    if (keepdata == 0) {
      //data is required, so store it
      count++;
      infile >> amount[timeid][areaid] >> ws;

    } else {
      //data is not required, so read but ignore it
      infile >> tmpnumber >> ws;
    }
  }

  if (count == 0)
    cout << "Warning - found no data in the data file for " << givenname << endl;
  return 1;
}

int ReadContinuousTimeMatrix(CommentStream& infile, int NumberOfColumns,
  const TimeClass* const TimeInfo, Formulamatrix& amount, Keeper* const keeper) {

  //JMB - check reading of whitespace
  ErrorHandler handle;

  infile >> ws;
  if (infile.eof()) {
    handle.Message("Cannot read time ordered data - file empty");
    return 0;
  }

  //From now on, we only expect to find a comment line, and then the data.
  char c = infile.peek();
  if (!isdigit(c)) {
    infile.get(c);
    while (c != '\n' && !infile.eof())
      infile.get(c);
  }

  //Now the data, year in first column, then step and vector of data
  if (!FindContinuousYearAndStepWithNoText(infile, TimeInfo->FirstYear(), TimeInfo->FirstStep())) {
    handle.Message("Unable to find first year and timestep for the data");
    return 0;
  }

  int year, step;
  infile >> year >> step >> ws;

  //now we should have reached TimeInfo->FirstYear() and TimeInfo->FirstStep()
  Formula tempF;   //value of tempF is initiated to 0.0
  amount.AddRows(TimeInfo->TotalNoSteps() + 1, NumberOfColumns, tempF);
  int time, column;

  for (time = 1; time <= TimeInfo->TotalNoSteps(); time++) {
    for (column = 0; column < NumberOfColumns; column++) {
      if (infile.eof()) {
        handle.Eof();
        return 0;
      }
      if (time != TimeInfo->CalcSteps(year, step)) {
        handle.Message("Unable to read time data - unexpected timestep - check data file");
        return 0;
      }

      infile >> amount[time][column];

      if (infile.fail()) {
        handle.Failure("Failed to read data in time order");
        return 0;
      }
    }

    if (!infile.eof() && time != TimeInfo->TotalNoSteps())
      infile >> year >> step;

  }
  return 1;
}

int ReadGrowthAmounts(CommentStream& infile, const TimeClass* const TimeInfo,
  const AreaClass* const Area, Formulamatrixptrvector& amount,
  const charptrvector& lenindex, Keeper* const keeper) {

  ErrorHandler handle;
  int i;
  int year, step, area;
  intvector Years, Steps;
  char tmplength[MaxStrLength];
  char tmpnumber[MaxStrLength];
  strncpy(tmplength, "", MaxStrLength);
  strncpy(tmpnumber, "", MaxStrLength);
  int keepdata, timeid, areaid, lenid;
  int count = 0;

  //Find start of distribution data in datafile
  infile >> ws;
  char c;
  c = infile.peek();
  if (!isdigit(c)) {
    infile.get(c);
    while (c != '\n' && !infile.eof())
      infile.get(c);
  }

  //Check the number of columns in the inputfile
  if (CountColumns(infile) != 5)
    handle.Message("Wrong number of columns in inputfile - should be 5");

  //Create storage for the data - size of the data is known
  Years.resize(TimeInfo->TotalNoSteps() + 1, 0);
  Steps.resize(TimeInfo->TotalNoSteps() + 1, 0);
  year = TimeInfo->FirstYear();
  step = TimeInfo->FirstStep();
  int numsteps = TimeInfo->StepsInYear();
  for (i = 1; i < Years.Size(); i++) {
    //time is counted from timestep 1
    Years[i] = year;
    Steps[i] = step;
    step++;
    if (step > numsteps) {
      year++;
      step = 1;
    }
  }

  while (!infile.eof()) {
    keepdata = 0;
    infile >> year >> step >> area >> tmplength;

    //check if the year and step are in the simulation
    timeid = -1;
    if (TimeInfo->IsWithinPeriod(year, step)) {
      //calculate the timeid index - time is counted from 1
      for (i = 1; i < Years.Size(); i++)
        if ((Years[i] == year) && (Steps[i] == step))
          timeid = i;

    } else {
      //dont keep the data
      keepdata = 1;
    }

    //if tmplength is in lenindex find lengthid, else dont keep the data
    lenid = -1;
    for (i = 0; i < lenindex.Size(); i++)
      if (strcasecmp(lenindex[i], tmplength) == 0)
        lenid = i;

    if (lenid == -1)
      keepdata = 1;

    if (keepdata == 0) {
      //data is required, so store it
      areaid = Area->InnerArea(area);
      if (areaid == -1)
        handle.UndefinedArea(area);

      count++;
      infile >> (*amount[areaid])[timeid][lenid] >> ws;

    } else {
      //data is not required, so read but ignore it
      infile >> tmpnumber >> ws;
    }
  }
  if (count == 0)
    cout << "Warning in GrowthAmounts - found no data in the data file\n";
  return 1;
}
