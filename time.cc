#include "areatime.h"
#include "errorhandler.h"
#include "readfunc.h"
#include "readword.h"
#include "gadget.h"

TimeClass::TimeClass(CommentStream& infile) {
  ErrorHandler handle;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  int i;

  ReadWordAndVariable(infile, "firstyear", firstyear);
  ReadWordAndVariable(infile, "firststep", firststep);
  ReadWordAndVariable(infile, "lastyear", lastyear);
  ReadWordAndVariable(infile, "laststep", laststep);
  ReadWordAndVariable(infile, "notimesteps", notimesteps);

  infile >> ws;
  timesteps.resize(notimesteps, 1, 0);  //index starts at 1, initialized to 0
  ReadIndexVector(infile, timesteps);

  infile >> ws;
  if (infile.eof())
    nrofsubsteps.resize(notimesteps, 1, 1);
  else {
    infile >> text >> ws;
    if (!(strcasecmp(text, "nrofsubsteps") == 0))
      handle.Message("Failure in reading time substeps");
    nrofsubsteps.resize(notimesteps, 1, 0);
    ReadIndexVector(infile, nrofsubsteps);
  }

  lengthofyear = 0;
  for (i = timesteps.Mincol(); i < timesteps.Maxcol(); i++)
    lengthofyear += timesteps[i];

  if (lengthofyear != 12)
    handle.Warning("Warning - length of year does not equal 12");
  if (firstyear > lastyear || (firstyear == lastyear && firststep > laststep))
    handle.Warning("Warning - time period is empty");
  currentyear = firstyear;
  currentstep = firststep;
}

void TimeClass::IncrementTime() {
  assert(!(currentyear == lastyear && currentstep == laststep));
  if (currentstep == notimesteps) {
    currentstep = 1;
    currentyear++;
  } else
    currentstep++;
  currentsubstep = 1;
}

int TimeClass::IsWithinPeriod(int year, int step) const {
  return((firstyear < year || (year == firstyear && firststep <= step))
    && (year < lastyear || (year == lastyear && step <= laststep))
    && (1 <= step) && (step <= notimesteps));
}

int TimeClass::SizeOfStepDidChange() const {
  if (timesteps.Size() == 1)
    return 0;
  else if (currentstep == 1)
    return timesteps[1] != timesteps[notimesteps];
  else
    return timesteps[currentstep] != timesteps[currentstep - 1];
}

int TimeClass::NrOfSubsteps() const {
  return (int)nrofsubsteps[currentstep];
}
