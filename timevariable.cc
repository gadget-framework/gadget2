#include "timevariable.h"
#include "errorhandler.h"
#include "readword.h"
#include "gadget.h"
#include "global.h"

void TimeVariable::read(CommentStream& infile,
  const TimeClass* const TimeInfo, Keeper* const keeper) {

  int i, check, tmpint;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);

  i = 0;
  while (!infile.eof()) {
    infile >> ws >> tmpint;
    years.resize(1, tmpint);
    if (infile.fail())
      handle.logFileMessage(LOGFAIL, "failed to read timevariable year");
    infile >> ws >> tmpint;
    steps.resize(1, tmpint);
    if (infile.fail())
      handle.logFileMessage(LOGFAIL, "failed to read timevariable step");

    values.resize(1, keeper);
    infile >> ws >> values[i];
    if (infile.fail())
      handle.logFileMessage(LOGFAIL, "failed to read timevariable value");
    values[i].Inform(keeper);

    infile >> ws;
    i++;
  }

  handle.logMessage(LOGMESSAGE, "Read timevariable data - number of timesteps", years.Size());

  //check if years and steps are ordered in time
  for (i = 0; i < years.Size() - 1; i++)
    if ((years[i + 1] < years[i]) ||
       (years[i + 1] == years[i] && steps[i + 1] <= steps[i]))
      handle.logMessage(LOGFAIL, "Error in timevariable - years and steps are not increasing");

  check = -1;
  for (i = 0; i < years.Size(); i++)
    if (years[i] == TimeInfo->getFirstYear() && steps[i] == TimeInfo->getFirstStep())
      check = i;

  if (check == -1)
    handle.logMessage(LOGFAIL, "Error in timevariable - nothing specified for first timestep of the simulation");
}

int TimeVariable::didChange(const TimeClass* const TimeInfo) {
  if (TimeInfo->getTime() == 1)
    return 1;  //return true for the first timestep

  return ((!(isEqual(lastvalue, value))) && (time == TimeInfo->getTime()));
}

void TimeVariable::Update(const TimeClass* const TimeInfo) {
  if (TimeInfo->getTime() == 1)
    timeid = 0;

  int i;
  for (i = timeid; i < steps.Size(); i++) {
    if (steps[i] == TimeInfo->getStep() && years[i] == TimeInfo->getYear()) {
      timeid = i;
      time = TimeInfo->getTime();
      break;
    }
  }

  lastvalue = value;
  value = values[timeid];
}

void TimeVariable::Delete(Keeper* const keeper) const {
  int i;
  for (i = 0; i < values.Size(); i++)
    values[i].Delete(keeper);
}

void TimeVariable::Interchange(TimeVariable& newTV, Keeper* const keeper) const {
  int i;
  newTV.value = value;
  newTV.lastvalue = lastvalue;
  newTV.timeid = timeid;
  newTV.time = time;
  newTV.years = years;
  newTV.steps = steps;
  newTV.values.resize(values.Size(), keeper);
  for (i = 0; i < steps.Size(); i++)
    values[i].Interchange(newTV.values[i], keeper);
}
