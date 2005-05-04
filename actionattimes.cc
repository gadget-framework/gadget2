#include "areatime.h"
#include "actionattimes.h"
#include "errorhandler.h"
#include "gadget.h"

extern ErrorHandler handle;

ActionAtTimes::ActionAtTimes() : everyStep(0) {
}

int ActionAtTimes::readFromFile(CommentStream& infile, const TimeClass* const TimeInfo) {
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);

  /* File format is:
   *  y1 s1
   *  ...
   *  yN sN
   * where y1, ..., yN are either a year or the text "all"
   * and s1, ..., sN either steps or the text "all".
   */

  streampos readPos = infile.tellg();
  infile >> ws;
  if (infile.fail()) {
    infile.seekg(readPos);
    return 0;
  }

  int error = 0;  //error = 0 OK(no error), 1 error occurred or 2 quit
  int year = 0;
  int step = 0;
  IntVector readtext(2, 0);  //Is 1 if text was read in a column.
  int column = 0;            //What column we are reading, no. 0 or 1.
  while (!infile.eof() && (error == 0)) {
    infile >> ws;
    readPos = infile.tellg();

    //First we read from infile.
    if (isdigit(infile.peek())) {
      readtext[column] = 0;
      if (column == 0)
        infile >> year;
      else
        infile >> step;
    } else {
      infile >> text;
      readtext[column] = 1;
      if ((strcasecmp(text, "all") == 0))
        readPos = infile.tellg();
      else
        if (column == 0)
          error = 2;
        else
          error = 1;
    }

    //Now we have read from infile.
    //If we have read the second column, we keep the data.
    if ((error == 0) && (column == 1)) {
      if ((readtext[0]) && (readtext[1])) {
        everyStep = 1;
        error = 2;   //We want to exit this while-loop.
      } else if ((readtext[0]) && !(readtext[1])) {
        if ((TimeInfo->LastYear() != TimeInfo->FirstYear()) ||
            (TimeInfo->FirstStep() <= step && step <= TimeInfo->LastStep())) {
          Steps.resize(1);
          Steps[Steps.Size() - 1] = step;
        }
      } else if (!(readtext[0]) && (readtext[1])) {
        if (TimeInfo->FirstYear() <= year && year <=  TimeInfo->LastYear()) {
          Years.resize(1);
          Years[Years.Size() - 1] = year;
        }
      } else {
        if (TimeInfo->isWithinPeriod(year, step)) {
          TimeSteps.resize(1);
          TimeSteps[TimeSteps.Size() - 1] = TimeInfo->calcSteps(year, step);
        }
      }
    }
    column = !column;  //change column from 0 to 1 or from 1 to 0.
  }
  infile.seekg(readPos);

  //And now the cleanup.
  if (everyStep) {
    while (TimeSteps.Size())
      TimeSteps.Delete(0);
    while (Years.Size())
      Years.Delete(0);
    while (Steps.Size())
      Steps.Delete(0);
  }

  if ((error == 1) || (column == 1 && error != 2))
    return 0;

  return 1;
}

ActionAtTimes::~ActionAtTimes() {
  while (TimeSteps.Size())
    TimeSteps.Delete(0);
  while (Years.Size())
    Years.Delete(0);
  while (Steps.Size())
    Steps.Delete(0);
}

void ActionAtTimes::addActions(const IntVector& years,
  const IntVector& steps, const TimeClass* const TimeInfo) {

  if (years.Size() != steps.Size())
    handle.logMessage(LOGFAIL, "Error in actionattimes - different number of years and steps");
  if (everyStep)
    return;
  int i;
  for (i = 0; i < years.Size(); i++)
    if (TimeInfo->isWithinPeriod(years[i], steps[i])) {
      TimeSteps.resize(1);
      TimeSteps[TimeSteps.Size() - 1] = TimeInfo->calcSteps(years[i], steps[i]);
    }
}

void ActionAtTimes::addActionsAllYears(const IntVector& steps, const TimeClass* const TimeInfo) {

  if (everyStep)
    return;
  int i;
  for (i = 0; i < steps.Size(); i++)
    if ((TimeInfo->LastYear() != TimeInfo->FirstYear()) ||
        (TimeInfo->FirstStep() <= steps[i] && steps[i] <= TimeInfo->LastStep())) {
      Steps.resize(1);
      Steps[Steps.Size() - 1] = steps[i];
    }
}

void ActionAtTimes::addActionsAllSteps(const IntVector& years, const TimeClass* const TimeInfo) {

  if (everyStep)
    return;
  int i;
  for (i = 0; i < years.Size(); i++)
    if (TimeInfo->FirstYear() <= years[i] && years[i] <= TimeInfo->LastYear()) {
      Years.resize(1);
      Years[Years.Size() - 1] = years[i];
    }
}

/* This function could be better implemented -- e.g. sort the vectors
 * TimeSteps, Years and Steps and use that to increase speed. That
 * could be done by insisting that TimeInfo's time is not decreasing
 * between calls, so that we can keep three indices, one for each
 * vector, telling us where we quit our search in the last call -- much
 * like done in Renewal::AddRenewal. */
int ActionAtTimes::AtCurrentTime(const TimeClass* const TimeInfo) const {
  if (everyStep)
    return 1;
  int i;
  for (i = 0; i < Steps.Size(); i++)
    if (Steps[i] == TimeInfo->CurrentStep())
      return 1;
  for (i = 0; i < Years.Size(); i++)
    if (Years[i] == TimeInfo->CurrentYear())
      return 1;
  for (i = 0; i < TimeSteps.Size(); i++)
    if (TimeSteps[i] == TimeInfo->CurrentTime())
      return 1;
  return 0;
}
