#include "areatime.h"
#include "errorhandler.h"
#include "readfunc.h"
#include "readword.h"
#include "gadget.h"

extern ErrorHandler handle;

TimeClass::TimeClass(CommentStream& infile) {

  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  int i;

  readWordAndVariable(infile, "firstyear", firstyear);
  readWordAndVariable(infile, "firststep", firststep);
  readWordAndVariable(infile, "lastyear", lastyear);
  readWordAndVariable(infile, "laststep", laststep);
  readWordAndVariable(infile, "notimesteps", notimesteps);

  infile >> ws;
  timesteps.resize(notimesteps, 1, 0.0);
  if (!readIndexVector(infile, timesteps))
    handle.Message("Failure in reading time steps");

  infile >> ws;
  if (infile.eof())
    nrofsubsteps.resize(notimesteps, 1, 1.0);
  else {
    infile >> text >> ws;
    if (!(strcasecmp(text, "nrofsubsteps") == 0))
      handle.Message("Failure in reading time substeps");
    nrofsubsteps.resize(notimesteps, 1, 0.0);
    if (!readIndexVector(infile, nrofsubsteps))
      handle.Message("Failure in reading time substeps");
  }

  lengthofyear = 0.0;
  for (i = timesteps.Mincol(); i < timesteps.Maxcol(); i++)
    lengthofyear += timesteps[i];

  if (lengthofyear != 12)
    handle.Warning("Warning - length of year does not equal 12");
  if (firstyear > lastyear || (firstyear == lastyear && firststep > laststep))
    handle.Warning("Warning - time period is empty");
  currentyear = firstyear;
  currentstep = firststep;
  handle.logMessage("Read time file - number of timesteps", this->TotalNoSteps());
}

void TimeClass::IncrementTime() {
  if (currentyear == lastyear && currentstep == laststep)
    handle.logFailure("Error in timeclass - cannot increment time past last timestep");
  if (currentstep == notimesteps) {
    currentstep = 1;
    currentyear++;
  } else
    currentstep++;
  currentsubstep = 1;
}

int TimeClass::IsWithinPeriod(int year, int step) const {
  return ((firstyear < year || (year == firstyear && firststep <= step))
    && (year < lastyear || (year == lastyear && step <= laststep))
    && (1 <= step) && (step <= notimesteps));
}

int TimeClass::SizeOfStepDidChange() const {
  if (timesteps.Size() == 1)
    return 0;
  else if (currentstep == 1)
    return (timesteps[1] != timesteps[notimesteps]);
  else
    return (timesteps[currentstep] != timesteps[currentstep - 1]);
}

int TimeClass::NrOfSubsteps() const {
  return ((int)nrofsubsteps[currentstep]);
}
