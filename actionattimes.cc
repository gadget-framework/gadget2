#include "areatime.h"
#include "actionattimes.h"
#include "errorhandler.h"
#include "gadget.h"
#include "global.h"

int ActionAtTimes::readFromFile(CommentStream& infile, const TimeClass* const TimeInfo) {

  /* File format is:
   *  y1 s1
   *  ...
   *  yN sN
   * where y1, ..., yN are either a year or the text 'all'
   *   and s1, ..., sN are either a step or the text 'all'
   */

  infile >> ws;
  if (infile.fail())
    return 0;

  int check = 0;  //check = 0 continue reading, check = 1 quit
  int year, step, column;
  year = step = column = 0;
  IntVector readtext(2, 0);
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  streampos pos = infile.tellg();

  while (!infile.eof() && (check == 0)) {
    //update the value of pos
    pos = infile.tellg();

    if (isdigit(infile.peek())) {
      //OK, we are about to read a number ...
      readtext[column] = 0;
      if (column == 0)
        infile >> year >> ws;
      else
        infile >> step >> ws;
        
    } else {
      //OK, we are about to read a word that should be 'all'
      infile >> text >> ws;
      readtext[column] = 1;
      if (strcasecmp(text, "all") != 0)
        check = 1; //we want to exit this while loop
    }

    //now we have read from infile the second column we store the data
    if (column == 1) {
      if ((readtext[0]) && (readtext[1])) {
        //we have read 'all' 'all'
        everyStep = 1;
        //check = 1; //we want to exit this while loop

      } else if ((readtext[0]) && !(readtext[1])) {
        //we have read 'all' step
        if ((TimeInfo->getLastYear() != TimeInfo->getFirstYear()) ||
            (TimeInfo->getFirstStep() <= step && step <= TimeInfo->getLastStep()))
          Steps.resize(1, step);

      } else if (!(readtext[0]) && (readtext[1])) {
        //we have read year 'all''
        if (TimeInfo->getFirstYear() <= year && year <=  TimeInfo->getLastYear())
          Years.resize(1, year);

      } else {
        //we have read year step
        if (TimeInfo->isWithinPeriod(year, step))
          TimeSteps.resize(1, TimeInfo->calcSteps(year, step));

      }
    }
    column = !column;  //change column to be read
  }

  if (!infile.eof())
    infile.seekg(pos);
  return 1;
}

void ActionAtTimes::addActions(const IntVector& years,
  const IntVector& steps, const TimeClass* const TimeInfo) {

  if (years.Size() != steps.Size())
    handle.logMessage(LOGFAIL, "Error in actionattimes - different number of years and steps");
  if (everyStep)
    return;
  int i;
  for (i = 0; i < years.Size(); i++)
    if (TimeInfo->isWithinPeriod(years[i], steps[i]))
      TimeSteps.resize(1, TimeInfo->calcSteps(years[i], steps[i]));
}

void ActionAtTimes::addActionsAllYears(const IntVector& steps, const TimeClass* const TimeInfo) {

  if (everyStep)
    return;
  int i;
  for (i = 0; i < steps.Size(); i++)
    if ((TimeInfo->getLastYear() != TimeInfo->getFirstYear()) ||
        (TimeInfo->getFirstStep() <= steps[i] && steps[i] <= TimeInfo->getLastStep()))
      Steps.resize(1, steps[i]);
}

void ActionAtTimes::addActionsAllSteps(const IntVector& years, const TimeClass* const TimeInfo) {

  if (everyStep)
    return;
  int i;
  for (i = 0; i < years.Size(); i++)
    if (TimeInfo->getFirstYear() <= years[i] && years[i] <= TimeInfo->getLastYear())
      Years.resize(1, years[i]);
}

/* This function could be better implemented -- e.g. sort the vectors
 * TimeSteps, Years and Steps and use that to increase speed. That
 * could be done by insisting that TimeInfo's time is not decreasing
 * between calls, so that we can keep three indices, one for each
 * vector, telling us where we quit our search in the last call */
int ActionAtTimes::atCurrentTime(const TimeClass* const TimeInfo) const {
  if (everyStep)
    return 1;
  int i;
  for (i = 0; i < Steps.Size(); i++)
    if (Steps[i] == TimeInfo->getStep())
      return 1;
  for (i = 0; i < Years.Size(); i++)
    if (Years[i] == TimeInfo->getYear())
      return 1;
  for (i = 0; i < TimeSteps.Size(); i++)
    if (TimeSteps[i] == TimeInfo->getTime())
      return 1;
  return 0;
}
