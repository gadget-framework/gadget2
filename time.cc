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

  infile >> ws;
  timesteps.resize(numtimesteps, 1, 0.0);
  if (!readIndexVector(infile, timesteps))
    handle.logFileMessage(LOGFAIL, "Failure in reading time steps");

  infile >> ws;
  if (infile.eof())
    numsubsteps.resize(numtimesteps, 1);
  else {
    infile >> text >> ws;
    if (!(strcasecmp(text, "nrofsubsteps") == 0))
      handle.logFileMessage(LOGFAIL, "Failure in reading time substeps");
    numsubsteps.resize(numtimesteps, 0);
    if (!readVector(infile, numsubsteps))
      handle.logFileMessage(LOGFAIL, "Failure in reading time substeps");
  }

  lengthofyear = 0.0;
  for (i = timesteps.minCol(); i < timesteps.maxCol(); i++)
    lengthofyear += timesteps[i];

  if (!(isZero(lengthofyear - 12)))
    handle.logFileMessage(LOGWARN, "Warning - length of year does not equal 12");
  if (firstyear > lastyear || (firstyear == lastyear && firststep > laststep))
    handle.logFileMessage(LOGWARN, "Warning - time period is empty");
  currentyear = firstyear;
  currentstep = firststep;
  currentsubstep = 1;
  if (handle.getLogLevel() >= LOGMESSAGE)
    handle.logMessage(LOGMESSAGE, "Read time file - number of timesteps", this->TotalNoSteps());
}

void TimeClass::IncrementTime() {
  if (handle.getLogLevel() >= LOGMESSAGE)
    handle.logMessage(LOGMESSAGE, "");  //write a blank line to the log file
  if (currentyear == lastyear && currentstep == laststep) {
    if (handle.getLogLevel() >= LOGMESSAGE)
      handle.logMessage(LOGMESSAGE, "The simulation has reached the last timestep for the current model run");

  } else if (currentstep == numtimesteps) {
    currentstep = 1;
    currentyear++;
    currentsubstep = 1;
    if (handle.getLogLevel() >= LOGMESSAGE)
      handle.logMessage(LOGMESSAGE, "Increased time in the simulation to timestep", this->CurrentTime());

  } else {
    currentstep++;
    currentsubstep = 1;
    if (handle.getLogLevel() >= LOGMESSAGE)
      handle.logMessage(LOGMESSAGE, "Increased time in the simulation to timestep", this->CurrentTime());
  }
}

int TimeClass::isWithinPeriod(int year, int step) const {
  return ((firstyear < year || (year == firstyear && firststep <= step))
    && (year < lastyear || (year == lastyear && step <= laststep))
    && (1 <= step) && (step <= numtimesteps));
}

int TimeClass::SizeOfStepDidChange() const {
  if (timesteps.Size() == 1)
    return 0;
  else if (currentstep == 1)
    return (timesteps[1] != timesteps[numtimesteps]);
  else
    return (timesteps[currentstep] != timesteps[currentstep - 1]);
}

void TimeClass:: Reset() {
  currentyear = firstyear;
  currentstep = firststep;
  currentsubstep = 1;
  if (handle.getLogLevel() >= LOGMESSAGE)
    handle.logMessage(LOGMESSAGE, "\nReset time in the simulation to timestep", this->CurrentTime());
}
