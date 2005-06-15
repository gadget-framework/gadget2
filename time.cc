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
  readWordAndVariable(infile, "notimesteps", numtimesteps);
  numsubsteps.resize(numtimesteps, 1);

  infile >> ws;
  timesteps.resize(numtimesteps, 1, 0.0);
  if (!readIndexVector(infile, timesteps))
    handle.logFileMessage(LOGFAIL, "Failure in reading time steps");

  infile >> ws;
  if (!(infile.eof())) {
    infile >> text >> ws;
    if (!(strcasecmp(text, "nrofsubsteps") == 0))
      handle.logFileUnexpected(LOGFAIL, "nrofsubsteps", text);
    if (!readVector(infile, numsubsteps))
      handle.logFileMessage(LOGFAIL, "Failure in reading time substeps");
  }

  lengthofyear = 0.0;
  for (i = timesteps.minCol(); i < timesteps.maxCol(); i++)
    lengthofyear += timesteps[i];

  if (!(isZero(lengthofyear - 12)))
    handle.logFileMessage(LOGWARN, "Warning - length of year does not equal 12");
  if (firstyear > lastyear || (firstyear == lastyear && firststep > laststep))
    handle.logMessage(LOGFAIL, "Error in time - time period is empty");

  //JMB store lengthofyear as 1/lengthofyear to save processing time
  if (isZero(lengthofyear))
    handle.logMessage(LOGFAIL, "Error in time - length of year is zero");
  lengthofyear = 1.0 / lengthofyear;
  currentyear = firstyear;
  currentstep = firststep;
  currentsubstep = 1;
  handle.logMessage(LOGMESSAGE, "Read time file - number of timesteps", this->numTotalSteps());
}

void TimeClass::IncrementTime() {
  if (handle.getLogLevel() >= LOGMESSAGE)
    handle.logMessage(LOGMESSAGE, "");  //write a blank line to the log file
  if (currentyear == lastyear && currentstep == laststep) {
    if (handle.getLogLevel() >= LOGMESSAGE)
      handle.logMessage(LOGMESSAGE, "The simulation has reached the last timestep for the current model run");

  } else {
    currentsubstep = 1;
    if (currentstep == numtimesteps) {
      currentstep = 1;
      currentyear++;
    } else
      currentstep++;

    if (handle.getLogLevel() >= LOGMESSAGE)
      handle.logMessage(LOGMESSAGE, "Increased time in the simulation to timestep", this->getTime());
  }
}

int TimeClass::isWithinPeriod(int year, int step) const {
  if ((year < firstyear) || (year > lastyear))
    return 0;  // year outside range
  if ((step < 1) || (step > numtimesteps))
    return 0;  // step outside range
  if ((year == firstyear) && (step >= firststep))
    return 1;  // first year
  if ((year == lastyear) && (step <= laststep))
    return 1;  // last year
  return 1;
}

int TimeClass::didStepSizeChange() const {
  if (currentstep == 1)
    return (timesteps[1] != timesteps[numtimesteps]);
  return (timesteps[currentstep] != timesteps[currentstep - 1]);
}

void TimeClass:: Reset() {
  currentyear = firstyear;
  currentstep = firststep;
  currentsubstep = 1;
  if (handle.getLogLevel() >= LOGMESSAGE)
    handle.logMessage(LOGMESSAGE, "\nReset time in the simulation to timestep", this->getTime());
}
