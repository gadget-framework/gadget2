#include "areatime.h"
#include "readfunc.h"
#include "errorhandler.h"
#include "gadget.h"

extern ErrorHandler handle;

AreaClass::AreaClass(CommentStream& infile, const TimeClass* const TimeInfo) {
  int i, j;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);

  infile >> text >> ws;
  if (strcasecmp(text, "areas") == 0) {
    i = 0;
    while (isdigit(infile.peek()) && !infile.eof()) {
      OuterAreas.resize(1);
      infile >> OuterAreas[i] >> ws;
      i++;
    }
  } else
    handle.logFileUnexpected(LOGFAIL, "areas", text);

  int noareas = OuterAreas.Size();
  size.resize(noareas);
  infile >> text >> ws;
  if (strcasecmp(text, "size") == 0) {
    for (i = 0; i < noareas; i++)
      infile >> size[i] >> ws;
  } else
    handle.logFileUnexpected(LOGFAIL, "size", text);

  for (i = 0; i < noareas; i++)
    if (isZero(size[i]))
      handle.logFileMessage(LOGWARN, "found area with a size of zero");

  infile >> text >> ws;
  if (strcasecmp(text, "temperature") != 0)
    handle.logFileUnexpected(LOGFAIL, "temperature", text);

  //Now the data which is in the following format: year step area temperature.
  temperature.AddRows(TimeInfo->numTotalSteps() + 1, noareas, 0.0);
  IntVector Years, Steps;
  int timeid, areaid, keepdata, year, step, area, count;
  double tmpnumber;

  //Check the number of columns in the inputfile
  if (countColumns(infile) != 4)
    handle.logFileMessage(LOGFAIL, "wrong number of columns in inputfile - should be 4");

  count = 0;
  while (!infile.eof()) {
    keepdata = 0;
    infile >> year >> step >> area >> tmpnumber >> ws;

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

    } else {
      //dont keep the data
      keepdata = 1;
    }

    //if area is in OuterAreas find areaid, else dont keep the data
    areaid = -1;
    for (i = 0; i < OuterAreas.Size(); i++)
      if (area == OuterAreas[i])
        areaid = i;

    if (areaid == -1)
      keepdata = 1;

    if (keepdata == 0) {
      //temperature data is required, so store it
      temperature[timeid][areaid] = tmpnumber;
      count++;
    }
  }
  if (count == 0)
    handle.logMessage(LOGWARN, "Warning in area - found no temperature data");
  if (count != (temperature.Nrow() - 1) * noareas)
    handle.logMessage(LOGWARN, "Warning in area - temperature data doesnt span time range");
  handle.logMessage(LOGMESSAGE, "Read temperature data - number of entries", count);
  handle.logMessage(LOGMESSAGE, "Read area file - number of areas", noareas);
}

int AreaClass::InnerArea(int area) const {
  int innerarea = -1;
  int i;
  for (i = 0; i < OuterAreas.Size(); i++)
    if (area == OuterAreas[i])
      innerarea = i;

  if (innerarea == -1)
    handle.logMessage(LOGWARN, "Warning in area - failed to match area", area);
  return innerarea;
}
