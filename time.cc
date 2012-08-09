#include "areatime.h"
#include "errorhandler.h"
#include "readfunc.h"
#include "readword.h"
#include "gadget.h"
#include "global.h"

TimeClass::TimeClass(CommentStream& infile, double maxratio) {

  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  int i;

  maxratioconsumed = maxratio;
  readWordAndVariable(infile, "firstyear", firstyear);
  readWordAndVariable(infile, "firststep", firststep);
  readWordAndVariable(infile, "lastyear", lastyear);
  readWordAndVariable(infile, "laststep", laststep);
  readWordAndVariable(infile, "notimesteps", numtimesteps);
  numsubsteps.resize(numtimesteps, 1);

  timesteps.resize(numtimesteps + 1, 0.0);
  for (i = 1; i <= numtimesteps; i++)
    infile >> ws >> timesteps[i];

  infile >> ws;
  if (!(infile.eof())) {
    infile >> text >> ws;
    if (strcasecmp(text, "nrofsubsteps") != 0)
      handle.logFileUnexpected(LOGFAIL, "nrofsubsteps", text);
    for (i = 0; i < numtimesteps; i++)
      infile >> numsubsteps[i] >> ws;
    for (i = 0; i < numtimesteps; i++)
      if (numsubsteps[i] == 0)
        handle.logFileMessage(LOGFAIL, "number of substeps must be non-zero");
  }

  lengthofyear = 0.0;
  for (i = 1; i <= numtimesteps; i++)
    lengthofyear += timesteps[i];

  if (!(isEqual(lengthofyear, 12.0)))
    handle.logFileMessage(LOGWARN, "length of year does not equal 12");
  if (firstyear > lastyear || (firstyear == lastyear && firststep > laststep))
    handle.logFileMessage(LOGFAIL, "time period is empty");

  //JMB store lengthofyear as 1/lengthofyear to save processing time
  if (isZero(lengthofyear))
    handle.logFileMessage(LOGFAIL, "length of year is zero");
  lengthofyear = 1.0 / lengthofyear;
  currentyear = firstyear;
  currentstep = firststep;
  currentsubstep = 1;
  handle.logMessage(LOGMESSAGE, "Read time file - number of timesteps", this->numTotalSteps());
}

void TimeClass::IncrementTime() {
  if (currentyear == lastyear && currentstep == laststep) {
    if (handle.getLogLevel() >= LOGMESSAGE)
      handle.logMessage(LOGMESSAGE, "\nThe simulation has reached the last timestep for the current model run");

  } else {
    currentsubstep = 1;
    if (currentstep == numtimesteps) {
      currentstep = 1;
      currentyear++;
    } else
      currentstep++;

    if (handle.getLogLevel() >= LOGMESSAGE)
      handle.logMessage(LOGMESSAGE, "\nIncreased time in the simulation to timestep", this->getTime());
  }
}

int TimeClass::isWithinPeriod(int year, int step) const {
  if ((year < firstyear) || (year > lastyear))
    return 0;  // year outside range
  if ((step < 1) || (step > numtimesteps))
    return 0;  // step outside range
  if ((year == firstyear) && (step < firststep))
    return 0;  // first year
  if ((year == lastyear) && (step > laststep))
    return 0;  // last year
  return 1;
}

int TimeClass::didStepSizeChange() const {
  if (currentstep == 1)
    return (timesteps[1] != timesteps[numtimesteps]);
  return (timesteps[currentstep] != timesteps[currentstep - 1]);
}

void TimeClass::Reset() {
  currentyear = firstyear;
  currentstep = firststep;
  currentsubstep = 1;
  if (handle.getLogLevel() >= LOGMESSAGE)
    handle.logMessage(LOGMESSAGE, "\nReset time in the simulation to timestep", this->getTime());
}

double TimeClass::getMaxRatioConsumed() const {
  if (numsubsteps[currentstep - 1] == 1)
    return maxratioconsumed;
  return pow(maxratioconsumed, numsubsteps[currentstep - 1]);
}
