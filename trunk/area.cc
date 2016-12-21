#include "areatime.h"
#include "readfunc.h"
#include "errorhandler.h"
#include "gadget.h"
#include "global.h"

AreaClass::AreaClass(CommentStream& infile,
  Keeper* const keeper, const TimeClass* const TimeInfo) {

  int i, tmpint = 0;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);

  infile >> text >> ws;
  if (strcasecmp(text, "areas") != 0)
    handle.logFileUnexpected(LOGFAIL, "areas", text);

  while (isdigit(infile.peek()) && !infile.eof()) {
    infile >> tmpint >> ws;
    modelAreas.resize(1, tmpint);
  }

  keeper->addString("area");
  infile >> text >> ws;
  if (strcasecmp(text, "size") != 0)
    handle.logFileUnexpected(LOGFAIL, "size", text);

  size.resize(modelAreas.Size(), keeper);
  for (i = 0; i < modelAreas.Size(); i++)
    if (!(infile >> size[i]))
      handle.logFileMessage(LOGFAIL, "invalid format for area size vector");
  size.Inform(keeper);
  keeper->clearLast();

  infile >> text >> ws;
  if (strcasecmp(text, "temperature") != 0)
    handle.logFileUnexpected(LOGFAIL, "temperature", text);

  //Now the data which is in the following format: year step area temperature.
  temperature.AddRows(TimeInfo->numTotalSteps() + 1, modelAreas.Size(), 0.0);
  IntVector Years, Steps;
  int timeid, areaid, keepdata, year, step, area, count, reject;
  double tmp;

  //Check the number of columns in the inputfile
  if (countColumns(infile) != 4)
    handle.logFileMessage(LOGFAIL, "wrong number of columns in inputfile - should be 4");

  year = step = area = count = reject = 0;
  while (!infile.eof()) {
    keepdata = 1;
    infile >> year >> step >> area >> tmp >> ws;

    //check if the year and step are in the simulation
    timeid = -1;
    if (TimeInfo->isWithinPeriod(year, step)) {
      for (i = 0; i < Years.Size(); i++)
        if ((Years[i] == year) && (Steps[i] == step))
          timeid = i;

      if (timeid == -1) {
        Years.resize(1, year);
        Steps.resize(1, step);
        timeid = Years.Size();  //time=0 isnt in the simulation
      }

    } else
      keepdata = 0;  //dont keep data

    //if area is in modelAreas find areaid, else dont keep the data
    areaid = -1;
    for (i = 0; i < modelAreas.Size(); i++)
      if (area == modelAreas[i])
        areaid = i;

    if (areaid == -1)
      keepdata = 0;

    if (keepdata == 1) {
      //temperature data is required, so store it
      temperature[timeid][areaid] = tmp;
      count++;
    } else
      reject++;  //count number of rejected data points read from file
  }

  if (count == 0)
    handle.logMessage(LOGWARN, "Warning in area - found no temperature data");
  if (count != (temperature.Nrow() - 1) * modelAreas.Size())
    handle.logMessage(LOGWARN, "Warning in area - temperature data doesnt span time range");
  if (reject != 0)
    handle.logMessage(LOGMESSAGE, "Discarded invalid temperature data - number of invalid entries", reject);
  handle.logMessage(LOGMESSAGE, "Read temperature data - number of entries", count);
  handle.logMessage(LOGMESSAGE, "Read area file - number of areas", modelAreas.Size());
}

int AreaClass::getInnerArea(int area) const {
  int i, innerarea = -1;
  for (i = 0; i < modelAreas.Size(); i++)
    if (area == modelAreas[i])
      innerarea = i;

  if (innerarea == -1)
    handle.logMessage(LOGWARN, "Warning in area - failed to match area", area);
  return innerarea;
}
